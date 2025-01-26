class BTC_HitZonesStruct
{
	private ref map<string, ref BTC_HitZoneStruct> m_Structs = new map<string, ref BTC_HitZoneStruct>();

	//------------------------------------------------------------------------------------------------
	static BTC_HitZonesStruct Serialize(DamageManagerComponent damageManager)
	{
		BTC_HitZonesStruct struct = new BTC_HitZonesStruct();
		array<HitZone> hitZones = {};
		damageManager.GetAllHitZones(hitZones);

		if (hitZones.IsEmpty())
			return struct;

		foreach (HitZone hitZone : hitZones)
		{
			struct.m_Structs.Insert(hitZone.GetName(), BTC_HitZoneStruct.Serialize(hitZone));
		}

		return struct;
	}

	//------------------------------------------------------------------------------------------------
	void Deserialize(DamageManagerComponent damageManager)
	{
		array<HitZone> hitZones = {};
		damageManager.GetAllHitZones(hitZones);

		if (hitZones.IsEmpty())
			return;

		BTC_HitZoneStruct struct;
		foreach (HitZone hitZone : hitZones)
		{
			struct = m_Structs.Get(hitZone.GetName());

			if (!struct)
				continue;

			struct.Deserialize(hitZone);
		}
	}
}

class BTC_HitZoneStruct
{
	private float m_fHealth = 0;

	//------------------------------------------------------------------------------------------------
	float GetHitZoneHealth()
	{
		return m_fHealth;
	}

	//------------------------------------------------------------------------------------------------
	void SetHitZoneHealth(float health)
	{
		m_fHealth = health;
	}

	//------------------------------------------------------------------------------------------------
	static BTC_HitZoneStruct Serialize(HitZone hitZone)
	{
		BTC_HitZoneStruct struct = new BTC_HitZoneStruct();
		struct.m_fHealth = hitZone.GetHealth();

		return struct;
	}

	//------------------------------------------------------------------------------------------------
	void Deserialize(HitZone hitZone)
	{
		hitZone.SetHealth(m_fHealth);
	}
}
