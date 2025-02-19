class BTC_EntityStruct
{
	protected static int s_iID = -1;

	protected int m_iID = -1;
	protected ResourceName m_sPrefabResourceName;
	protected ref BTC_HitZonesStruct m_HitZonesStruct;
	protected ref BTC_StorageTreeNode m_StoragesTree;
	protected vector m_vTransform[4];

	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonSaveContext
	bool SerializationSave(BaseSerializationSaveContext context)
	{
		if (!context.IsValid())
			return false;

		context.WriteValue("m_iID", m_iID);
		context.WriteValue("m_sPrefabResourceName", m_sPrefabResourceName);
		context.WriteValue("m_HitZonesStruct", m_HitZonesStruct);
		context.WriteValue("m_StoragesTree", m_StoragesTree);
		context.WriteValue("m_vTransform", m_vTransform);

		ResetIDCounter();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonLoadContext
	bool SerializationLoad(BaseSerializationLoadContext context)
	{
		if (!context.IsValid())
			return false;

		context.ReadValue("m_iID", m_iID);
		context.ReadValue("m_sPrefabResourceName", m_sPrefabResourceName);
		context.ReadValue("m_HitZonesStruct", m_HitZonesStruct);
		context.ReadValue("m_StoragesTree", m_StoragesTree);
		context.ReadValue("m_vTransform", m_vTransform);

		if (m_iID > s_iID) //raise class' static int counter to the highest recorded
			s_iID = m_iID;

		ResetIDCounter();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	void Serialize(notnull IEntity entity)
	{
		m_sPrefabResourceName = GetEntityResourceName(entity);
		entity.GetTransform(m_vTransform);
		SerializeHitZones(entity);
		SerializeStorages(entity);
	}

	//------------------------------------------------------------------------------------------------
	protected void SerializeHitZones(notnull IEntity entity)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (!damageManager)
			return;

		m_HitZonesStruct = BTC_HitZonesStruct.Serialize(damageManager);
	}

	//------------------------------------------------------------------------------------------------
	protected void SerializeStorages(notnull IEntity entity)
	{
		BaseInventoryStorageComponent storageComp = BaseInventoryStorageComponent.Cast(entity.FindComponent(BaseInventoryStorageComponent));
		if (!storageComp)
			return;

		m_StoragesTree = new BTC_StorageTreeNode();
		m_StoragesTree.Serialize(entity);
	}

	//------------------------------------------------------------------------------------------------
	void Deserialize()
	{
		IEntity entity = SpawnEntity();
		if (!entity)
			return;

		DeserializeHitZones(entity);
		DeserializeStorages(entity);
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
	IEntity SpawnEntity()
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = m_vTransform;

		Resource resource = Resource.Load(m_sPrefabResourceName);
		if (!resource || !resource.IsValid())
			return null;

		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		if (!entity)
			return null;

		Physics physics = entity.GetPhysics();
		if (physics)
			physics.SetActive(ActiveState.ACTIVE);

		return entity;
	}

	//------------------------------------------------------------------------------------------------
	protected void DeserializeHitZones(notnull IEntity entity)
	{
		if (!m_HitZonesStruct)
			return;

		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (!damageManager)
			return;

		m_HitZonesStruct.Deserialize(damageManager);
	}

	//------------------------------------------------------------------------------------------------
	protected void DeserializeStorages(notnull IEntity entity)
	{
		if (!m_StoragesTree)
			return;

		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(entity.FindComponent(InventoryStorageManagerComponent));
		if (!storageManager)
			return;

		m_StoragesTree.Deserialize(entity);
	}

	//------------------------------------------------------------------------------------------------
	int GetID()
	{
		return m_iID;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetResourceName()
	{
		return m_sPrefabResourceName;
	}

	//------------------------------------------------------------------------------------------------
	void SetResourceName(ResourceName name)
	{
		m_sPrefabResourceName = name;
	}

	//------------------------------------------------------------------------------------------------
	void SetHitZonesStruct(BTC_HitZonesStruct struct)
	{
		m_HitZonesStruct = struct;
	}

	//------------------------------------------------------------------------------------------------
	BTC_HitZonesStruct GetHitZonesStruct()
	{
		return m_HitZonesStruct;
	}

	//------------------------------------------------------------------------------------------------
	void GetTransform(out vector outTransform[4])
	{
		outTransform[0] = m_vTransform[0];
		outTransform[1] = m_vTransform[1];
		outTransform[2] = m_vTransform[2];
		outTransform[3] = m_vTransform[3];
	}

	//------------------------------------------------------------------------------------------------
	BTC_StorageTreeNode GetStorages()
	{
		return m_StoragesTree;
	}

	//------------------------------------------------------------------------------------------------
	void SetStorages(BTC_StorageTreeNode node)
	{
		m_StoragesTree = node;
	}

	//------------------------------------------------------------------------------------------------
	//! internally used to reset ID counter back to default
	protected static void ResetIDCounter()
	{
		s_iID = -1;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_EntityStruct()
	{
		s_iID += 1;
		m_iID = s_iID;
	}

}
