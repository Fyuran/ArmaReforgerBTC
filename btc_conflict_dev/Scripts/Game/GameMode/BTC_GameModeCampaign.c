modded class SCR_GameModeCampaign : SCR_BaseGameMode
{

	[Attribute("1", desc: "Prevent removal of Vehicles", category: "Garbage System")]
	protected bool m_bPreventRemoveVehicles;

	[Attribute("1", desc: "Allow Spawn only on Main HQ", category: "Campaign")]
	protected bool m_bAllowSpawnOnlyOnHQ;

	//------------------------------------------------------------------------------------------------
	bool GetPreventRemoveVehicles()
	{
		return m_bPreventRemoveVehicles;
	}

	bool IsSpawnOnlyOnHQ()
	{
		return m_bAllowSpawnOnlyOnHQ;
	}

}
