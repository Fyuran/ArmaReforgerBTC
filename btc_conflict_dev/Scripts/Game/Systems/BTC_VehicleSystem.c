void OnVehicleRegistered(Vehicle vehicle);
typedef func OnVehicleRegistered;
typedef ScriptInvokerBase<OnVehicleRegistered> OnVehicleRegisteredInvoker;

void OnVehicleUnregistered(Vehicle vehicle);
typedef func OnVehicleUnregistered;
typedef ScriptInvokerBase<OnVehicleUnregistered> OnVehicleUnregisteredInvoker;

//------------------------------------------------------------------------------------------------
class BTC_VehicleSystem : GameSystem
{
	protected ref array<Vehicle> m_Vehicles;
	
	void RegisterVehicle(notnull Vehicle vehicle)
	{
		
		
	} 
}