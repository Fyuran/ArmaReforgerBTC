class BTC_Struct : SCR_JsonApiStruct
{
	protected ref array<ref BTC_VehicleStruct> m_aVehicleStructs = {};
	protected static const int DEFAULT_DELAY = 2000;
	protected static string s_sMissionName;
	
	//------------------------------------------------------------------------------------------------
	override bool Serialize()
	{
		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return false;

		array<Vehicle> vehicles = {};
		system.GetVehicles(vehicles);

		if (!vehicles || vehicles.IsEmpty())
			return true;

		foreach (Vehicle vehicle : vehicles)
		{
			BTC_VehicleStruct struct = new BTC_VehicleStruct();
			struct.Serialize(vehicle);
			m_aVehicleStructs.Insert(struct);
		}
		
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		saveContext.EnableTypeDiscriminator();
		saveContext.WriteValue("m_aVehicleStructs", m_aVehicleStructs);
		
		bool hasSaved = saveContext.SaveToFile(string.Format("$profile:%1-BTC_Struct.save.json", s_sMissionName));
		if(!hasSaved)
		{
			PrintFormat("Could not save to file %1", string.Format("$profile:%1-BTC_Struct.save.json", s_sMissionName), level: LogLevel.ERROR);
		}
		
		return hasSaved;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnDeserialize()
	{
		if (m_aVehicleStructs.IsEmpty())
			return;

		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return;

		array<Vehicle> vehicles = {};
		system.GetVehicles(vehicles);

		foreach (Vehicle vehicle : vehicles)
		{
			if (vehicle)
				SCR_EntityHelper.DeleteEntityAndChildren(vehicle);
		}

		foreach (BTC_VehicleStruct struct : m_aVehicleStructs)
		{
			struct.Deserialize();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool Deserialize()
	{
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		bool hasLoaded = loadContext.LoadFromFile(string.Format("$profile:%1-BTC_Struct.save.json", s_sMissionName));
		if(!hasLoaded)
		{
			PrintFormat("Could not load savefile %1", string.Format("$profile:%1-BTC_Struct.save.json", s_sMissionName), level: LogLevel.ERROR);
		}
		loadContext.EnableTypeDiscriminator();
		loadContext.ReadValue("m_aVehicleStructs", m_aVehicleStructs);
		
		OnDeserialize();

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void BTC_Struct()
	{
		string scenarioId = SCR_SaveWorkshopManager.GetCurrentScenarioId();
		s_sMissionName = SCR_SaveWorkshopManager.ScenarioGUIDToID(scenarioId);
	}
}