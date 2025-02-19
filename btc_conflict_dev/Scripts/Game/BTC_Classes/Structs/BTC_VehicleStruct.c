class BTC_VehicleStruct : BTC_EntityStruct
{
	protected ref BTC_FuelNodesStruct m_FuelStruct;

	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonSaveContext
	override bool SerializationSave(BaseSerializationSaveContext context)
	{
		if (!context.IsValid())
			return false;

		context.WriteValue("m_iID", m_iID);
		context.WriteValue("m_sPrefabResourceName", m_sPrefabResourceName);
		context.WriteValue("m_HitZonesStruct", m_HitZonesStruct);
		context.WriteValue("m_StoragesTree", m_StoragesTree);
		context.WriteValue("m_vTransform", m_vTransform);
		context.WriteValue("m_FuelStruct", m_FuelStruct);

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
		context.ReadValue("m_sPrefabResourceName", m_sPrefabResourceName);
		context.ReadValue("m_HitZonesStruct", m_HitZonesStruct);
		context.ReadValue("m_StoragesTree", m_StoragesTree);
		context.ReadValue("m_vTransform", m_vTransform);
		context.ReadValue("m_FuelStruct", m_FuelStruct);

		if (m_iID > s_iID) //raise class' static int counter to the highest recorded
			s_iID = m_iID;

		ResetIDCounter();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void Serialize(notnull IEntity entity)
	{
		m_sPrefabResourceName = GetEntityResourceName(entity);
		entity.GetTransform(m_vTransform);

		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle)
			return;

		SerializeHitZones(vehicle);
		SerializeStorages(vehicle);
		SerializeFuelNodes(vehicle);
	}

	//------------------------------------------------------------------------------------------------
	override void Deserialize()
	{
		Vehicle vehicle = Vehicle.Cast(SpawnEntity());
		if (!vehicle)
			return;

		DeserializeHitZones(vehicle);
		DeserializeStorages(vehicle);
		DeserializeFuelNodes(vehicle);
	}

	//------------------------------------------------------------------------------------------------
	void SerializeFuelNodes(Vehicle vehicle)
	{
		SCR_FuelManagerComponent fuelManager = vehicle.GetScriptedFuelManager();
		m_FuelStruct = BTC_FuelNodesStruct.Serialize(fuelManager);
	}

	//------------------------------------------------------------------------------------------------
	void DeserializeFuelNodes(Vehicle vehicle)
	{
		SCR_FuelManagerComponent fuelManager = vehicle.GetScriptedFuelManager();
		m_FuelStruct.Deserialize(fuelManager);
	}

	//------------------------------------------------------------------------------------------------
	override IEntity SpawnEntity()
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = m_vTransform;

		Resource resource = Resource.Load(m_sPrefabResourceName);
		if (!resource || !resource.IsValid())
			return null;

		Vehicle vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params));
		if (!vehicle)
			return null;

		CarControllerComponent carController = CarControllerComponent.Cast(vehicle.FindComponent(CarControllerComponent));

		// Activate handbrake so the vehicles don't go downhill on their own when spawned
		if (carController)
			carController.SetPersistentHandBrake(true);

		Physics physicsComponent = vehicle.GetPhysics();

		// Snap to terrain
		if (physicsComponent)
		{
			physicsComponent.SetVelocity("0 -1 0");
			physicsComponent.SetActive(ActiveState.ACTIVE);
		}

		return vehicle;
	}
}
