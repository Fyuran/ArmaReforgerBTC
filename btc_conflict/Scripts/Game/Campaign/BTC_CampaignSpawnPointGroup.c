//------------------------------------------------------------------------------------------------
modded class SCR_CampaignSpawnPointGroup : SCR_SpawnPoint
{
	//------------------------------------------------------------------------------------------------
	override bool IsSpawnPointVisibleForPlayer(int pid)
	{
		SCR_CampaignMilitaryBaseComponent base = SCR_CampaignMilitaryBaseComponent.Cast(GetParent().FindComponent(SCR_CampaignMilitaryBaseComponent));
		if(base)
		{
			SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
			return campaign.IsSpawnOnlyOnHQ() && base.IsHQ();
		}
		
		return super.IsSpawnPointVisibleForPlayer(pid);
	}
};
