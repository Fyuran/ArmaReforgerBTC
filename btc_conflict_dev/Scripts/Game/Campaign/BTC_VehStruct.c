#define BTC_DEBUG
class BTC_VehStruct : SCR_JsonApiStruct
{
	protected ResourceName m_sPrefabResourceName;
	protected vector m_vUpVector = vector.Up;
	protected vector m_vRightVector = vector.Right;
	protected vector m_vForwardVector = vector.Forward;
	protected vector m_vOriginVector = vector.Zero;
	protected ref array<ref BTC_FuelNodeStruct> m_aFuelNodesStructs = {};
	protected ref array<ref BTC_HitZoneStruct> m_aHitZonesStructs = {};
	protected ref BTC_StorageTreeStruct m_Storage;
	private static const vector EXTRA_HEIGHT = "0 5 0";

	//------------------------------------------------------------------------------------------------
	ResourceName GetPrefabResourceName()
	{
		return m_sPrefabResourceName;
	}

	//------------------------------------------------------------------------------------------------
	void GetTransform(out vector transform[4])
	{
		transform[0] = m_vRightVector;
		transform[1] = m_vUpVector;
		transform[2] = m_vForwardVector;
		transform[3] = m_vOriginVector;
	}

	//------------------------------------------------------------------------------------------------
	array<ref BTC_FuelNodeStruct> GetFuelNodesStructs()
	{
		return m_aFuelNodesStructs;
	}

	//------------------------------------------------------------------------------------------------
	array<ref BTC_HitZoneStruct> GetHitZonesStruct()
	{
		return m_aHitZonesStructs;
	}

	//------------------------------------------------------------------------------------------------
	bool Serialize(Vehicle vehicle)
	{
		if (!vehicle)
			return false;

		m_sPrefabResourceName = SCR_BaseContainerTools.GetPrefabResourceName(vehicle.GetPrefabData().GetPrefab());

		vector transform[4];
		vehicle.GetWorldTransform(transform);
		m_vUpVector = transform[0];
		m_vRightVector = transform[1];
		m_vForwardVector = transform[2];
		m_vOriginVector = transform[3];

		array<SCR_FuelNode> nodes = {};
		vehicle.GetScriptedFuelManager().GetScriptedFuelNodesList(nodes);

		foreach (SCR_FuelNode node : nodes)
		{
			BTC_FuelNodeStruct struct = new BTC_FuelNodeStruct();
			if (!struct.Serialize(node))
				return false;
			m_aFuelNodesStructs.Insert(struct);
		}

		array<HitZone> hitZones = {};
		vehicle.GetScriptedDamageManager().GetAllHitZones(hitZones);

		foreach (HitZone hitZone : hitZones)
		{
			BTC_HitZoneStruct struct = new BTC_HitZoneStruct();
			if (!struct.Serialize(hitZone))
				return false;
			m_aHitZonesStructs.Insert(struct);
		}

		m_Storage = new BTC_StorageTreeStruct();
		if (!m_Storage.Serialize(vehicle))
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool Deserialize()
	{
		Vehicle vehicle = SpawnVehicle();

		#ifdef BTC_DEBUG
		PrintFormat("BTC_VehStruct Deserializing for %1", vehicle, level: LogLevel.WARNING);
		#endif

		if (!vehicle)
			return false;

		foreach (BTC_HitZoneStruct struct : m_aHitZonesStructs)
		{
			if (!struct.Deserialize(vehicle))
				return false;
		}

		foreach (BTC_FuelNodeStruct struct : m_aFuelNodesStructs)
		{
			if (!struct.Deserialize(vehicle))
				return false;
		}


		if (!m_Storage.Deserialize(vehicle))
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	Vehicle SpawnVehicle()
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetTransform(params.Transform);
		params.Transform[3] = params.Transform[3] + EXTRA_HEIGHT; //add some height or else they'll spawn under the terrain

		Resource resource = Resource.Load(GetPrefabResourceName());
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
			physicsComponent.SetVelocity("0 -1 0");

		return vehicle;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_VehStruct()
	{
		RegV("m_vRightVector");
		RegV("m_vUpVector");
		RegV("m_vForwardVector");
		RegV("m_vOriginVector");
		RegV("m_sPrefabResourceName");
		RegV("m_aFuelNodesStructs");
		RegV("m_aHitZonesStructs");
		RegV("m_Storage");
	}

	//------------------------------------------------------------------------------------------------
	void ~BTC_VehStruct()
	{
		UnregV("m_vUpVector");
		UnregV("m_vRightVector");
		UnregV("m_vForwardVector");
		UnregV("m_vOriginVector");
		UnregV("m_sPrefabResourceName");
		UnregV("m_aFuelNodesStructs");
		UnregV("m_aHitZonesStructs");
		UnregV("m_Storage");
	}
}

class BTC_HitZoneStruct : SCR_JsonApiStruct
{
	protected string m_sName = "";
	protected float m_fHealth = 0;

	//------------------------------------------------------------------------------------------------
	bool Deserialize(notnull Vehicle vehicle)
	{
		HitZone hitZone = vehicle.GetScriptedDamageManager().GetHitZoneByName(GetHitZoneName());
		hitZone.SetHealth(GetHitZoneHealth());

		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool Serialize(HitZone hitZone)
	{
		if (!hitZone)
			return false;

		m_sName = hitZone.GetName();
		m_fHealth = hitZone.GetHealth();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	string GetHitZoneName()
	{
		return m_sName;
	}

	//------------------------------------------------------------------------------------------------
	float GetHitZoneHealth()
	{
		return m_fHealth;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_HitZoneStruct()
	{
		RegV("m_sName");
		RegV("m_fHealth");
	}

	//------------------------------------------------------------------------------------------------
	void ~BTC_HitZoneStruct()
	{
		UnregV("m_sName");
		UnregV("m_fHealth");
	}
}

class BTC_FuelNodeStruct : SCR_JsonApiStruct
{
	protected float m_fHealth = 0;
	protected float m_fFuel = 0;
	protected int m_iTankId = -1;

	//------------------------------------------------------------------------------------------------
	bool Deserialize(notnull Vehicle vehicle)
	{
		array<SCR_FuelNode> nodes = {};
		vehicle.GetScriptedFuelManager().GetScriptedFuelNodesList(nodes);

		foreach (SCR_FuelNode node : nodes)
		{
			if (node.GetFuelTankID() == GetFuelNodeTankId())
				node.SetFuel(GetFuelNodeFuel());
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool Serialize(SCR_FuelNode node)
	{
		if (!node)
			return false;

		m_fHealth = node.GetHealth();
		m_fFuel = node.GetFuel();
		m_iTankId = node.GetFuelTankID();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	float GetFuelNodeHealth()
	{
		return m_fHealth;
	}

	//------------------------------------------------------------------------------------------------
	float GetFuelNodeFuel()
	{
		return m_fFuel;
	}

	//------------------------------------------------------------------------------------------------
	int GetFuelNodeTankId()
	{
		return m_iTankId;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_FuelNodeStruct()
	{
		//RegV("m_fHealth");
		RegV("m_fFuel");
		RegV("m_iTankId");
	}

	//------------------------------------------------------------------------------------------------
	void ~BTC_FuelNodeStruct()
	{
		//UnregV("m_fHealth");
		UnregV("m_fFuel");
		UnregV("m_iTankId");
	}
}
