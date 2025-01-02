modded class SCR_CampaignMilitaryBaseComponent : SCR_MilitaryBaseComponent
{
	override void OnInitialized()
	{
		if (!m_bInitialized)
			return;

		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();

		if (!campaign)
			return;

		// Register default base spawnpoint
		SCR_SpawnPoint spawnpoint;
		IEntity child = GetOwner().GetChildren();

		while (child)
		{
			spawnpoint = SCR_SpawnPoint.Cast(child);

			if (spawnpoint)
			{
				m_SpawnPoint = spawnpoint;
				
				if(!IsHQ())
				{
					spawnpoint.SetSpawnPointEnabled_S(false);
				}
				
				SCR_CampaignBuildingProviderComponent buildingProvider = SCR_CampaignBuildingProviderComponent.Cast(GetOwner().FindComponent(SCR_CampaignBuildingProviderComponent));
				if (buildingProvider)
					m_SpawnPoint.SetSpawnPositionRange(buildingProvider.GetBuildingRadius());
				else
					m_SpawnPoint.SetSpawnPositionRange(m_iRadius);

				if (m_OnSpawnPointAssigned)
					m_OnSpawnPointAssigned.Invoke(spawnpoint);

				break;
			}

			child = child.GetSibling();
		}

#ifdef ENABLE_DIAG
		if (SCR_RespawnComponent.Diag_IsCLISpawnEnabled())
			HandleSpawnPointFaction();
#endif
		
		// Initialize registered services
		array<SCR_ServicePointComponent> services = {};
		GetServices(services);

		foreach (SCR_ServicePointComponent service : services)
		{
			OnServiceBuilt(service);
		}

		if (RplSession.Mode() != RplMode.Dedicated)
		{
			ConnectToCampaignBasesSystem();

			Faction playerFaction = SCR_FactionManager.SGetLocalPlayerFaction();

			if (!playerFaction)
				playerFaction = campaign.GetBaseManager().GetLocalPlayerFaction();

			if (playerFaction)
				OnLocalPlayerFactionAssigned(playerFaction);
			else
				campaign.GetOnFactionAssignedLocalPlayer().Insert(OnLocalPlayerFactionAssigned);
		}

		campaign.GetBaseManager().GetOnAllBasesInitialized().Insert(OnAllBasesInitialized);

		if (IsProxy())
		{
			campaign.GetBaseManager().AddActiveBase();
			return;
		}
		else
		{
			if (!m_bIsHQ && SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent)))
			{
				Math.Randomize(-1);
				GetGame().GetCallqueue().CallLater(EvaluateDefenders, DEFENDERS_CHECK_PERIOD + Math.RandomIntInclusive(0, DEFENDERS_CHECK_PERIOD * 0.1), true);
			}
		}

		SCR_CampaignSeizingComponent seizingComponent = SCR_CampaignSeizingComponent.Cast(GetOwner().FindComponent(SCR_CampaignSeizingComponent));

		if (seizingComponent)
		{
			seizingComponent.GetOnCaptureStart().Insert(OnCaptureStart);
			seizingComponent.GetOnCaptureInterrupt().Insert(EndCapture);
		}

		if (m_eType == SCR_ECampaignBaseType.BASE && !campaign.IsTutorial())
		{
			// Supplies autoregen loop
			GetGame().GetCallqueue().CallLater(SupplyIncomeTimer, SCR_GameModeCampaign.UI_UPDATE_DELAY, true, false);

			// Refresh spawnpoint faction after loaded data has been applied
			GetGame().GetCallqueue().CallLater(HandleSpawnPointFaction, SCR_GameModeCampaign.BACKEND_DELAY);
		}
	}
	

	//------------------------------------------------------------------------------------------------
	//! \param[in] isHQ
	override void SetAsHQ(bool isHQ)
	{
		if (IsProxy())
			return;

		m_bIsHQ = isHQ;
		SCR_CampaignFaction faction = GetCampaignFaction();

		if (m_bIsHQ)
		{
			SCR_CampaignMilitaryBaseComponent previousHQ = faction.GetMainBase();
			SCR_CampaignMilitaryBaseComponent enemyHQ = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager()).GetEnemyFaction(faction).GetMainBase();

			if (previousHQ && previousHQ != this && previousHQ != enemyHQ)
			{
				if (previousHQ.GetDisableWhenUnusedAsHQ())
					previousHQ.Disable();
			}
			
			SCR_SpawnPoint spawnPoint = GetSpawnPoint();
			if(spawnPoint)
				spawnPoint.SetSpawnPointEnabled_S(true);
		}

		Replication.BumpMe();
		OnHQSet();
	}
}
