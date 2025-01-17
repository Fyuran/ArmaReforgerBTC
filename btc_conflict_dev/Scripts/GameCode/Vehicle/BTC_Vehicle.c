modded class VehicleClass : BaseVehicleClass
{
	[Attribute("1", UIWidgets.CheckBox, desc:"Should this vehicle be saved in persistence database", category: "Persistence")]
	protected bool m_bIsAllowedToPersist;

	//------------------------------------------------------------------------------------------------
	bool IsAllowedToPersist()
	{
		return m_bIsAllowedToPersist;
	}

}

modded class Vehicle : BaseVehicle
{
	protected SCR_FuelManagerComponent m_FuelManager;
	protected RplComponent m_RplComponent;
	protected SCR_VehicleDamageManagerComponent m_DamageManager;
	protected SCR_VehicleInventoryStorageManagerComponent m_StorageManager;

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_VehicleController = VehicleControllerComponent.Cast(FindComponent(VehicleControllerComponent));
		m_pFactionComponent = SCR_VehicleFactionAffiliationComponent.Cast(FindComponent(SCR_VehicleFactionAffiliationComponent));
		m_FuelManager = SCR_FuelManagerComponent.Cast(FindComponent(SCR_FuelManagerComponent));
		m_DamageManager = SCR_VehicleDamageManagerComponent.Cast(GetDamageManager());
		m_StorageManager = SCR_VehicleInventoryStorageManagerComponent.Cast(FindComponent(SCR_VehicleInventoryStorageManagerComponent));

		m_RplComponent = GetRplComponent();
		if (!m_RplComponent)
			PrintFormat("%1 Entity has no RplComponent", this, level: LogLevel.ERROR);

		UpdateResourceComponent();

		if (IsProxy())
			return;

		if (m_DamageManager)
		{
			SCR_HitZone defaultHitZone = SCR_HitZone.Cast(m_DamageManager.GetDefaultHitZone());
			defaultHitZone.GetOnDamageStateChanged().Insert(OnStateDestroyed);
		}

		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return;

		system.RegisterVehicle(this);

	}

	//------------------------------------------------------------------------------------------------
	//! Called by invoker, manages unregistration from BTC_VehicleSystem
	void OnStateDestroyed(notnull SCR_HitZone defaultHitZone)
	{
		if (defaultHitZone.GetDamageState() != EDamageState.DESTROYED)
			return;

		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return;

		system.UnregisterVehicle(this);
	}

	//------------------------------------------------------------------------------------------------
	SCR_DamageManagerComponent GetScriptedDamageManager()
	{
		return m_DamageManager;
	}

	//------------------------------------------------------------------------------------------------
	SCR_FuelManagerComponent GetScriptedFuelManager()
	{
		return m_FuelManager;
	}

	//------------------------------------------------------------------------------------------------
	SCR_VehicleInventoryStorageManagerComponent GetStorageManager()
	{
		return m_StorageManager;
	}

	//------------------------------------------------------------------------------------------------
	//! Checks if this entity is locally owned
	bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}

	//------------------------------------------------------------------------------------------------
	//! Handles on server unregistration from BTC_VehicleSystem when deleted
	void ~Vehicle()
	{
		if (IsProxy())
			return;

		BTC_VehicleSystem system = BTC_VehicleSystem.GetInstance();
		if (!system)
			return;

		system.UnregisterVehicle(this);
	}
}
