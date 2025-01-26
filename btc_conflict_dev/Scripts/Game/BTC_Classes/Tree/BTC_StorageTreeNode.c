class BTC_StorageTreeNode : BTC_TreeNode
{
	//Until Vest loadout modification is implemented if it will ever be, discard said items
	protected const static ref array<ResourceName> FILTERED_NAMES =
	{
		"{09DEA8153A0C312D}Prefabs/Characters/Core/EquipmentPart_Base.et",
		"{70BC751317551D9B}Prefabs/Items/Equipment/Canteens/Canteen_Soviet_01.et",
		"{630FB4AD4A735264}Prefabs/Items/Equipment/Canteens/Canteen_US_01.et",
		"{0A9CD090EE3440E7}Prefabs/Weapons/Core/WeaponPart_Base.et",
		"{28866E98F3970DFE}Prefabs/Weapons/Core/MuzzleDevice_base.et",
		"{2149560A9BEFBAC2}Prefabs/Items/Core/Equip_Accessory_base.et",
	};
	protected ref array<ResourceName> m_aItems = {};

	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonSaveContext
	override bool SerializationSave(BaseSerializationSaveContext context)
	{
		if (!context.IsValid())
			return false;

		context.WriteValue("m_iID", m_iID);
		context.WriteValue("m_iParentID", m_iParentID);
		context.WriteValue("m_aItems", m_aItems);
		context.WriteValue("m_aChildren", m_aChildren);

		ResetIDCounter();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonLoadContext
	override bool SerializationLoad(BaseSerializationLoadContext context)
	{
		if (!context.IsValid())
			return false;

		context.ReadValue("m_iID", m_iID);
		context.ReadValue("m_iParentID", m_iParentID);
		context.ReadValue("m_aItems", m_aItems);
		context.ReadValue("m_aChildren", m_aChildren);
		SerializationLoadChildren();

		if (m_iID > s_iID) //raise class' static int counter to the highest recorded
			s_iID = m_iID;

		ResetIDCounter();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	int AddItem(ResourceName name)
	{
		return m_aItems.Insert(name);
	}

	//------------------------------------------------------------------------------------------------
	bool RemoveItem(ResourceName name)
	{
		return m_aItems.RemoveItemOrdered(name);
	}

	//------------------------------------------------------------------------------------------------
	int GetItems(out array<ResourceName> outArray)
	{
		return outArray.Copy(m_aItems);
	}

	//------------------------------------------------------------------------------------------------
	static ResourceName GetEntityResourceName(IEntity entity)
	{
		if (!entity)
			return "NULL_OWNER_PREFAB_NAME";

		ResourceName entityResourceName = SCR_BaseContainerTools.GetPrefabResourceName(entity.GetPrefabData().GetPrefab());
		return entityResourceName;
	}

	//------------------------------------------------------------------------------------------------
	void Serialize(notnull IEntity rootEntity)
	{

		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(rootEntity.FindComponent(InventoryStorageManagerComponent));
		if (!storageManager)
			return;

		array<IEntity> items = {};
		storageManager.GetItems(items);
		foreach (IEntity item : items)
		{
			ResourceName itemName = GetEntityResourceName(item);
			if (!HasAnyFilteredResourceName(itemName))
				m_aItems.Insert(itemName);
		}
	}

	//------------------------------------------------------------------------------------------------
	void Deserialize(notnull IEntity rootEntity)
	{
		if (m_aItems.IsEmpty())
			return;

		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(rootEntity.FindComponent(InventoryStorageManagerComponent));
		if (!storageManager)
			return;

		array<IEntity> defaultItems = {};
		storageManager.GetItems(defaultItems);
		foreach (IEntity defaultItem : defaultItems)
		{
			storageManager.TryDeleteItem(defaultItem);
		}

		foreach (ResourceName item : m_aItems)
		{
			IEntity itemEntity = SpawnItem(item);
			if (storageManager.CanInsertItem(itemEntity))
			{
				if (!storageManager.TryInsertItem(itemEntity))
					SCR_EntityHelper.DeleteEntityAndChildren(itemEntity);
			}
			else
			{
				SCR_EntityHelper.DeleteEntityAndChildren(itemEntity);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static IEntity SpawnItem(ResourceName resourceName)
	{
		Resource resource = Resource.Load(resourceName);
		if (!resource)
		{
			PrintFormat("Bad prefab name found %1", resourceName, level: LogLevel.ERROR);
			return null;
		}

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		Math3D.MatrixIdentity4(params.Transform);

		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		if (!entity)
			return null;

		return entity;
	}

	//------------------------------------------------------------------------------------------------
	//! Check if there is a match with filtered ResourceName prefabs ancestors
	static bool HasAnyFilteredResourceName(ResourceName name)
	{
		foreach (ResourceName resourceName : FILTERED_NAMES)
		{
			if (SCR_BaseContainerTools.IsKindOf(name, resourceName))
				return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------
	static void PrintDebug(BTC_StorageTreeNode tree, string tabulations = "")
	{
		if (tree.m_iLevel != 0) //avoid root level tabulation
			tabulations += "\t";

		foreach (int index, ResourceName item : tree.m_aItems)
		{
			PrintFormat("%1%2.%3.%4--%5", tabulations, tree.m_iLevel, tree.m_iID, index, item);
		}
	}

	void ~BTC_StorageTreeNode()
	{
		if (!m_Parent)
			return;

		m_Parent.RemoveChild(this);
	}
}
