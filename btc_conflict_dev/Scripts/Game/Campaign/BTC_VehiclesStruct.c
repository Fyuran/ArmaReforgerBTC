#define BTC_DEBUG
[BaseContainerProps()]
class BTC_VehiclesStruct : SCR_JsonApiStruct
{
	protected ref array<ref BTC_VehStruct> m_aVehStructs = {};
	private static const int DEFAULT_DELAY = 2000;

	//------------------------------------------------------------------------------------------------
	override bool Serialize()
	{
		#ifdef BTC_DEBUG
		Print("BTC_VehiclesStruct Serialize called", LogLevel.WARNING);
		#endif

		Clear();

		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return false;

		array<Vehicle> vehicles = {};
		system.GetVehicles(vehicles);

		if (!vehicles || vehicles.IsEmpty())
			return true;

		foreach (Vehicle vehicle : vehicles)
		{
			BTC_VehStruct struct = new BTC_VehStruct();
			if (!struct.Serialize(vehicle))
				return false;
			m_aVehStructs.Insert(struct);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool Deserialize()
	{

		#ifdef BTC_DEBUG
		Print("BTC_VehiclesStruct Deserialize called", LogLevel.WARNING);
		#endif

		GetGame().GetCallqueue().CallLater(OnDeserialize, DEFAULT_DELAY * 2);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	void OnDeserialize()
	{
		#ifdef BTC_DEBUG
		Print("BTC_VehiclesStruct OnDeserialize called", LogLevel.WARNING);
		#endif

		if (m_aVehStructs.IsEmpty())
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

		foreach (BTC_VehStruct struct : m_aVehStructs)
		{
			struct.Deserialize();
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnSuccess(int errorCode)
	{
		PrintFormat("BTC_VehiclesStruct success: %1", errorCode);
	}

	//------------------------------------------------------------------------------------------------
	override void OnError(int errorCode)
	{
		PrintFormat("BTC_VehiclesStruct error: %1", errorCode);
	}

	//------------------------------------------------------------------------------------------------
	void Clear()
	{
		m_aVehStructs.Clear();
	}

	//------------------------------------------------------------------------------------------------
	array<ref BTC_VehStruct> GetVehiclesStruct()
	{
		return m_aVehStructs;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_VehiclesStruct()
	{
		RegV("m_aVehStructs");
	}

	//------------------------------------------------------------------------------------------------
	void ~BTC_VehiclesStruct()
	{
		UnregV("m_aVehStructs");
	}

}
