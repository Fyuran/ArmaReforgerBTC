[BaseContainerProps()]
class BTC_Struct : SCR_JsonApiStruct
{
	protected ref array<ref BTC_VehicleStruct> m_aVehicleStructs = {};
	protected static const int DEFAULT_DELAY = 2000;
	protected static string s_sSaveFileName;

	//------------------------------------------------------------------------------------------------
	override bool Serialize()
	{
		if (!Replication.IsServer())
			return false;

		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return false;

		array<Vehicle> vehicles = {};
		system.GetVehicles(vehicles);

		if (!vehicles || vehicles.IsEmpty())
			return true;

		m_aVehicleStructs.Clear(); //BTC_Struct will exists for the whole duration of the scenario, without clearing, duplicates will exist
		foreach (Vehicle vehicle : vehicles)
		{
			BTC_VehicleStruct struct = new BTC_VehicleStruct();
			struct.Serialize(vehicle);
			m_aVehicleStructs.Insert(struct);
		}

		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		saveContext.EnableTypeDiscriminator();
		saveContext.WriteValue("m_aVehicleStructs", m_aVehicleStructs);

		DeleteOldFile();
		bool hasSaved = saveContext.SaveToFile(s_sSaveFileName);
		if (!hasSaved)
		{
			PrintFormat("Could not save to file %1", s_sSaveFileName, level: LogLevel.ERROR);
			return false;
		}

		return hasSaved;
	}

	//------------------------------------------------------------------------------------------------
	//! Ensures a clean slate
	bool DeleteOldFile()
	{
		if (s_sSaveFileName.IsEmpty())
			return false;

		if (FileIO.FileExists(s_sSaveFileName))
			return FileIO.DeleteFile(s_sSaveFileName);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool Deserialize()
	{
		if (!Replication.IsServer())
			return false;

		GetGame().GetCallqueue().CallLater(OnDeserialize, DEFAULT_DELAY);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	void OnDeserialize()
	{
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		bool hasLoaded = loadContext.LoadFromFile(s_sSaveFileName);
		if (!hasLoaded)
		{
			PrintFormat("Could not load savefile %1", s_sSaveFileName, level: LogLevel.ERROR);
			return;
		}
		loadContext.EnableTypeDiscriminator();
		loadContext.ReadValue("m_aVehicleStructs", m_aVehicleStructs);

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
	void BTC_Struct()
	{
		if (!Replication.IsServer())
			return;

		string scenarioId = SCR_SaveWorkshopManager.GetCurrentScenarioId();
		string missionName = SCR_SaveWorkshopManager.ScenarioGUIDToID(scenarioId);
		s_sSaveFileName = string.Format("$profile:%1-BTC_Struct.save.json", missionName);
		RegV("DEFAULT_DELAY");
	}
}
