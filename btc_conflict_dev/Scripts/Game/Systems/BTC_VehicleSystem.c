class BTC_VehicleSystem : GameSystem
{
	protected ref array<Vehicle> m_aVehicles = {};

	//------------------------------------------------------------------------------------------------
	void RegisterVehicle(notnull Vehicle vehicle)
	{
		if (!IsAllowedToPersist(vehicle))
			return;

		if (!m_aVehicles.Contains(vehicle))
			m_aVehicles.Insert(vehicle);

		#ifdef BTC_DEBUG
		OnRegistered(vehicle);
		#endif
	}

	//------------------------------------------------------------------------------------------------
	void UnregisterVehicle(notnull Vehicle vehicle)
	{
		if (m_aVehicles.Contains(vehicle))
			m_aVehicles.RemoveItem(vehicle);

		#ifdef BTC_DEBUG
		OnUnregistered(vehicle);
		#endif

	}

	//------------------------------------------------------------------------------------------------
	void GetVehicles(out array<Vehicle> vehicles)
	{
		vehicles.Copy(m_aVehicles);
	}

	//------------------------------------------------------------------------------------------------
	bool IsAllowedToPersist(Vehicle vehicle)
	{
		VehicleClass prefabData = VehicleClass.Cast(vehicle.GetPrefabData());
		if (!prefabData)
			return true;

		return prefabData.IsAllowedToPersist();
	}

	#ifdef BTC_DEBUG
	//------------------------------------------------------------------------------------------------
	void OnRegistered(Vehicle vehicle)
	{
		PrintFormat("%1 has been inserted into BTC_VehicleSystem", vehicle, level: LogLevel.WARNING);
	}

	//------------------------------------------------------------------------------------------------
	void OnUnregistered(Vehicle vehicle)
	{
		PrintFormat("%1 has been removed from BTC_VehicleSystem", vehicle, level: LogLevel.WARNING);
	}
	#endif

	//------------------------------------------------------------------------------------------------
	static BTC_VehicleSystem GetInstance()
	{
		World world = GetGame().GetWorld();

		if (!world)
			return null;

		return BTC_VehicleSystem.Cast(world.FindSystem(BTC_VehicleSystem));
	}
}
