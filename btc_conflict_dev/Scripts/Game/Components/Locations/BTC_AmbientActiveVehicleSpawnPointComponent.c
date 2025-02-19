class BTC_AmbientActiveVehicleSpawnPointComponentClass : SCR_AmbientVehicleSpawnPointComponentClass
{
}

class BTC_AmbientActiveVehicleSpawnPointComponent : SCR_AmbientVehicleSpawnPointComponent
{
	[Attribute("0", UIWidgets.ResourceNamePicker, category: "Customization", desc:"Which Prefabs should be randomly selected when spawning a vehicle", params: "et")]
	protected ref array<ResourceName> m_aVehiclePrefabs;

	[Attribute("0", UIWidgets.ComboBox, "Select which vehicle compartments should be filled", "", ParamEnumArray.FromEnum(ECompartmentType), "Customization")]
	protected ref array<ECompartmentType> m_aCompartmentTypes;

	[Attribute("{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et", UIWidgets.ResourceNamePicker, "Cycle waypoint to be used for waypoints in hierarchy.", "et")]
	protected ResourceName m_sCycleWaypointPrefab;

	[Attribute("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, "Waypoint to be used if no waypoints are found in hierarchy.", "et")]
	protected ResourceName m_sDefaultWaypointPrefab;

	protected ref array<ResourceName> m_aVehicleOccupantsPrefabs = {};

	protected AIWaypoint m_Waypoint;

	//------------------------------------------------------------------------------------------------
	//! \return
	ResourceName GetCycleWaypointPrefab()
	{
		return m_sCycleWaypointPrefab;
	}

	//------------------------------------------------------------------------------------------------
	//! \return
	ResourceName GetDefaultWaypointPrefab()
	{
		return m_sDefaultWaypointPrefab;
	}

	//------------------------------------------------------------------------------------------------
	override protected void Update(SCR_Faction faction)
	{
		SCR_EntityCatalog entityCatalog;
		m_SavedFaction = faction;

		if (m_aVehiclePrefabs.IsEmpty())
		{
			PrintFormat("No vehicle prefabs set for %1, initializing to entities from catalog", this, level: LogLevel.WARNING);
			entityCatalog = faction.GetFactionEntityCatalogOfType(EEntityCatalogType.VEHICLE);
			array<SCR_EntityCatalogEntry> data = {};
			entityCatalog.GetFullFilteredEntityListWithLabels(data, {EEditableEntityLabel.VEHICLE_TURRET, EEditableEntityLabel.VEHICLE_CAR, EEditableEntityLabel.VEHICLE_APC}, needsAllIncluded:false);

			if (data.IsEmpty())
			{
				PrintFormat("No vehicle prefabs found for %1", this, level: LogLevel.WARNING);
				RemoveSpawnPoint();
				return;
			}

			foreach (SCR_EntityCatalogEntry entry : data)
			{
				m_aVehiclePrefabs.Insert(entry.GetPrefab());
			}
		}

		if (faction)
		{
			entityCatalog = faction.GetFactionEntityCatalogOfType(EEntityCatalogType.CHARACTER);
		}
		else
		{
			SCR_EntityCatalogManagerComponent comp = SCR_EntityCatalogManagerComponent.GetInstance();

			if (!comp)
				return;

			entityCatalog = comp.GetEntityCatalogOfType(EEntityCatalogType.CHARACTER);
		}

		if (!entityCatalog)
			return;

		array<SCR_EntityCatalogEntry> data = {};
		entityCatalog.GetFullFilteredEntityListWithLabels(data, {EEditableEntityLabel.ROLE_RIFLEMAN});

		if (data.IsEmpty())
		{
			PrintFormat("No character prefabs found for %1 in entity catalog", this, level: LogLevel.WARNING);
			RemoveSpawnPoint();
			return;
		}

		foreach (SCR_EntityCatalogEntry entry : data)
		{
			m_aVehicleOccupantsPrefabs.Insert(entry.GetPrefab());
		}

		Math.Randomize(-1);
		m_sPrefab = m_aVehiclePrefabs.GetRandomElement();
	}

	//------------------------------------------------------------------------------------------------
	//!
	//! \return the created vehicle
	override Vehicle SpawnVehicle()
	{
		if (m_aCompartmentTypes.IsEmpty())
			return null;

		SCR_FactionAffiliationComponent factionComp = SCR_FactionAffiliationComponent.Cast(GetOwner().FindComponent(SCR_FactionAffiliationComponent));

		if (!factionComp)
			return null;

		SCR_Faction faction = SCR_Faction.Cast(factionComp.GetAffiliatedFaction());

		if (!faction)
			faction = SCR_Faction.Cast(factionComp.GetDefaultAffiliatedFaction());

		//! last check, should this fail, no use iterating through this instance
		if (!faction)
		{
			PrintFormat("No faction could be found for %1", this, level: LogLevel.WARNING);
			RemoveSpawnPoint();
		}

		if (faction != m_SavedFaction || m_sPrefab.IsEmpty())
			Update(faction);

		if (m_aVehicleOccupantsPrefabs.IsEmpty())
		{
			PrintFormat("No occupant prefabs could be found for %1", this, level: LogLevel.WARNING);
			RemoveSpawnPoint();
			return null;
		}

		Resource vehiclePrefab = Resource.Load(m_sPrefab);

		if (!vehiclePrefab || !vehiclePrefab.IsValid())
			return null;

		vector pos;

		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetOwner().GetTransform(params.Transform);

		m_Vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(vehiclePrefab, GetGame().GetWorld(), params));
		m_fRespawnTimestamp = null;
		m_bFirstSpawnDone = true;
		m_bSpawnProcessed = true;

		if (!m_Vehicle)
			return null;

		SCR_BaseCompartmentManagerComponent compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(m_Vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		if (!compartmentManager)
			return null;

		compartmentManager.SpawnCatalogOccupants(m_aCompartmentTypes, m_aVehicleOccupantsPrefabs);


		Physics physicsComponent = m_Vehicle.GetPhysics();

		if (physicsComponent)
			physicsComponent.SetActive(ActiveState.ACTIVE);

		EventHandlerManagerComponent handler = EventHandlerManagerComponent.Cast(m_Vehicle.FindComponent(EventHandlerManagerComponent));

		if (handler)
			handler.RegisterScriptHandler("OnDestroyed", this, OnVehicleDestroyed);

		return m_Vehicle;
	}

	bool RemoveSpawnPoint()
	{
		//! remove this invalid spawn point from managing system
		SCR_AmbientVehicleSystem system = SCR_AmbientVehicleSystem.GetInstance();
		if (system)
		{
			system.UnregisterSpawnpoint(this);
			return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_AmbientActiveVehicleSpawnPointComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_iRespawnPeriod = 0;
		m_aIncludedEditableEntityLabels.Clear();
		m_aExcludedEditableEntityLabels.Clear();
		m_bRequireAllIncludedLabels = false;
	}
}
