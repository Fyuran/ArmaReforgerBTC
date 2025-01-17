typedef array<typename> TypeArray;

class BTC_StorageTreeStruct : SCR_JsonApiStruct
{
	protected ref array<ResourceName> m_aAllItems = {};
	protected ref array<ref BTC_StorageTreeNode> m_aNodes = {};


	//------------------------------------------------------------------------------------------------
	bool Serialize(notnull IEntity rootOwner)
	{
		if (!rootOwner.FindComponent(InventoryStorageManagerComponent)) //no storage manager means we can't serialize any item
			return false;

		m_aNodes.Insert(new BTC_StorageTreeNode(this, rootOwner));
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool Deserialize(notnull IEntity entity)
	{
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(entity.FindComponent(InventoryStorageManagerComponent));
		if (!storageManager)
			return false;

		array<IEntity> prefabItems = {};
		storageManager.GetItems(prefabItems); //remove default items spawned through prefab
		foreach (IEntity item : prefabItems)
		{
			storageManager.TryDeleteItem(item);
		}

		foreach (ResourceName resourceName : m_aAllItems)
		{
			EntitySpawnParams params = new EntitySpawnParams();
			Math3D.MatrixIdentity4(params.Transform); //valid transform vector array to all 0's

			Resource resource = Resource.Load(resourceName);
			IEntity item = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
			if (!item)
				continue;

			storageManager.TryInsertItem(item, EStoragePurpose.PURPOSE_DEPOSIT);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_StorageTreeStruct()
	{
		RegV("m_aAllItems");
	}

	//------------------------------------------------------------------------------------------------
	void ~BTC_StorageTreeStruct()
	{
		UnregV("m_aAllItems");
	}

	//------------------------------------------------------------------------------------------------
	void Insert(ResourceName value)
	{
		m_aAllItems.Insert(value);
	}

	//------------------------------------------------------------------------------------------------
	void Insert(BTC_StorageTreeNode value)
	{
		m_aNodes.Insert(value);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(BTC_StorageTreeNode value)
	{
		m_aNodes.RemoveItem(value);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(ResourceName value)
	{
		m_aAllItems.RemoveItem(value);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(int index)
	{
		m_aNodes.Remove(index);
	}

	//------------------------------------------------------------------------------------------------
	void GetItems(out array<ResourceName> outItems)
	{
		outItems.Copy(m_aAllItems);
	}
}

class BTC_StorageTreeNode : Managed
{
	const static ref TypeArray WEAPON_ACTIONS = {SCR_EquipWeaponAction, SCR_WeaponAction, SCR_EquipWeaponHolsterAction};
	const static ref TypeArray CLOTH_ACTIONS = {SCR_EquipClothAction};
	const static ref array<ResourceName> FILTERED_RESOURCE_NAMES =
	{
		"{630FB4AD4A735264}Prefabs/Items/Equipment/Canteens/Canteen_US_01.et",
		"{70BC751317551D9B}Prefabs/Items/Equipment/Canteens/Canteen_Soviet_01.et",
		"{2E0E3D7D1DD0FF8F}Prefabs/Weapons/Core/Attachment_Base.et",
		"{2149560A9BEFBAC2}Prefabs/Items/Core/Equip_Accessory_base.et",
		"{09DEA8153A0C312D}Prefabs/Characters/Core/EquipmentPart_Base.et",
		"{0A9CD090EE3440E7}Prefabs/Weapons/Core/WeaponPart_Base.et"
	};

	protected ResourceName m_sNodeName;

	protected BTC_StorageTreeStruct m_Root;
	protected IEntity m_Owner;
	protected ref array<ResourceName> m_aItems = {};
	protected BTC_StorageTreeNode m_Parent;
	protected ref array<ref BTC_StorageTreeNode> m_aChildren = {};

	void ~BTC_StorageTreeNode()
	{
		if (m_Root)
		{
			m_Root.Remove(this);
			foreach (ResourceName resource : m_aItems)
			{
				m_Root.Remove(resource);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	void BTC_StorageTreeNode(notnull BTC_StorageTreeStruct root, notnull IEntity owner, BTC_StorageTreeNode parent = null)
	{
		m_Owner = owner;
		m_sNodeName = SCR_BaseContainerTools.GetPrefabResourceName(m_Owner.GetPrefabData().GetPrefab());
		m_Root = root;

		if (parent)
			m_Parent = parent;

		ParseItems();
	}

	//------------------------------------------------------------------------------------------------
	//! executes tree parse
	protected void ParseItems()
	{
		BaseInventoryStorageComponent storageOwnerComp = BaseInventoryStorageComponent.Cast(m_Owner.FindComponent(BaseInventoryStorageComponent));
		if (!storageOwnerComp)
			return;

		array<IEntity> content = {};
		storageOwnerComp.GetAll(content);

		foreach (IEntity slot : content)
		{
			if (slot.FindComponent(BaseInventoryStorageComponent))
			{
				BTC_StorageTreeNode newNode = new BTC_StorageTreeNode(m_Root, slot, this);
				Insert(newNode);
			}

			ResourceName slotResourceName = SCR_BaseContainerTools.GetPrefabResourceName(slot.GetPrefabData().GetPrefab());
			if (!HasAnyInheritedResourceName(slot, FILTERED_RESOURCE_NAMES, slotResourceName))
			{
				Insert(slotResourceName);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Insert into both root and current node instance
	void Insert(ResourceName value)
	{
		m_Root.Insert(value);
		m_aItems.Insert(value);
	}

	//------------------------------------------------------------------------------------------------
	//! Insert new node into both root and children
	void Insert(BTC_StorageTreeNode value)
	{
		m_aChildren.Insert(value);
		m_Root.Insert(value);
	}

	//------------------------------------------------------------------------------------------------
	IEntity GetStorage()
	{
		return m_Owner;
	}

	//------------------------------------------------------------------------------------------------
	IEntity GetParentStorage()
	{
		return m_Parent.GetStorage();
	}

	//------------------------------------------------------------------------------------------------
	void GetChildrenStorages(out array<IEntity> outChildren)
	{
		array<IEntity> children = {};
		foreach (BTC_StorageTreeNode struct : m_aChildren)
		{
			children.Insert(struct.GetStorage());
		}

		outChildren.Copy(children);
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetResourceName()
	{
		return m_sNodeName;
	}

	//------------------------------------------------------------------------------------------------
	//! Check if entity has ALL passed user actions
	static bool HasMatchingActions(IEntity entity, array<typename> types)
	{
		ActionsManagerComponent actionManagerComp = ActionsManagerComponent.Cast(entity.FindComponent(ActionsManagerComponent));
		if (!actionManagerComp)
			return false;

		array<BaseUserAction> actions = {};
		actionManagerComp.GetActionsList(actions);
		return BTC_Helpers<BaseUserAction>.HasTypes(actions, types);
	}

	//------------------------------------------------------------------------------------------------
	//! Check if entity has ALL passed components
	static bool HasMatchingComponents(IEntity entity, array<typename> types)
	{
		foreach (typename type : types)
		{
			if (!entity.FindComponent(type))
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Match any ResourceName
	static bool HasAnyInheritedResourceName(IEntity object, array<ResourceName> resourceNames, ResourceName objectResourceName = "")
	{
		if (!object)
			return false;

		if (objectResourceName.IsEmpty())
			objectResourceName = SCR_BaseContainerTools.GetPrefabResourceName(object.GetPrefabData().GetPrefab());

		foreach (ResourceName resourceName : resourceNames)
		{
			if (SCR_BaseContainerTools.IsKindOf(objectResourceName, resourceName))
				return true;
		}
		return false;
	}
}

/*
class BTC_PredicateBase
{
	bool Test(Managed item);
	void PrintDebug(Managed item);
}

class BTC_PredicateTest : BTC_PredicateBase
{
	override bool Test(Managed item)
	{
		Print(item.Type());
		return item.IsInherited(Managed);
	}

	override void PrintDebug(Managed item)
	{
		Print(item);
	}
}

//weapon actions
SCR_EquipWeaponAction
SCR_WeaponAction
SCR_EquipWeaponHolsterAction

//vest components
ClothNodeStorageComponent
*/
class BTC_Helpers<Class T>
{
	//------------------------------------------------------------------------------------------------
	//! Match all types
	static bool HasTypes(array<T> objects, array<typename> types)
	{
		if (!objects || objects.IsEmpty())
			return false;

		array<typename> containerTypeNames = {};
		foreach (T object : objects)
		{
			containerTypeNames.Insert(object.Type());
		}

		foreach (typename type : types)
		{
			bool hasFound = false;
			foreach (typename containerTypeName : containerTypeNames)
			{
				if (containerTypeName.IsInherited(type))
					hasFound = true;
			}
			if (!hasFound) //all matches are negative, halt loop and return
				return false;
		}

		return true;
	}

	private void BTC_Helpers();
	private void ~BTC_Helpers();
}
