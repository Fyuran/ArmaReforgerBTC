
//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	override void EOnInit(IEntity owner)
	{
		// Set Test Game Flags
		#ifdef WORKBENCH
			if (GetGame().GetWorldEntity() && !GetGame().AreGameFlagsObtained())
			{
				GetGame().SetGameFlags(m_eTestGameFlags, false);
			}
		#endif
				// Set Test Game Flags
		#ifdef SAVE_MANAGER_DEBUG_HEADER
			if (GetGame().GetWorldEntity() && !GetGame().AreGameFlagsObtained())
			{
				GetGame().SetGameFlags(m_eTestGameFlags, false);
			}
		#endif
		
		//~ Remove any duplicate entries
		if (m_aDisabledResourceTypes.IsEmpty())
		{
			//~ TODO: Make this cleaner
			
			set<EResourceType> duplicateRemoveSet = new set<EResourceType>();
			
			foreach (EResourceType resourceType : m_aDisabledResourceTypes)
			{
				duplicateRemoveSet.Insert(resourceType);
			}
			
			m_aDisabledResourceTypes.Clear();
			foreach (EResourceType resourceType : duplicateRemoveSet)
			{
				m_aDisabledResourceTypes.Insert(resourceType);
			}
		}

		// Find required components
        m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_pRespawnSystemComponent = SCR_RespawnSystemComponent.Cast(owner.FindComponent(SCR_RespawnSystemComponent));		
		m_RespawnTimerComponent = SCR_RespawnTimerComponent.Cast(owner.FindComponent(SCR_RespawnTimerComponent));
		m_ScoringSystemComponent = SCR_BaseScoringSystemComponent.Cast(owner.FindComponent(SCR_BaseScoringSystemComponent));
		m_pGameModeHealthSettings = SCR_GameModeHealthSettings.Cast(owner.FindComponent(SCR_GameModeHealthSettings));

		if (!m_RplComponent)
			Print("SCR_BaseGameMode is missing RplComponent!", LogLevel.ERROR);
		if (!m_pRespawnSystemComponent)
			Print("SCR_BaseGameMode is missing SCR_RespawnSystemComponent!", LogLevel.WARNING);
		
		if (!m_aAdditionalGamemodeComponents)
			m_aAdditionalGamemodeComponents = new array<SCR_BaseGameModeComponent>();

		array<Managed> additionalComponents = new array<Managed>();
		int count = owner.FindComponents(SCR_BaseGameModeComponent, additionalComponents);

		m_aAdditionalGamemodeComponents.Clear();
		for (int i = 0; i < count; i++)
		{
			SCR_BaseGameModeComponent comp = SCR_BaseGameModeComponent.Cast(additionalComponents[i]);
			m_aAdditionalGamemodeComponents.Insert(comp);
		}

		// Find and sort state components
		array<Managed> stateComponents = new array<Managed>();
		int stateCount = owner.FindComponents(SCR_BaseGameModeStateComponent, stateComponents);
		for (int i = 0; i < stateCount; i++)
		{
			SCR_BaseGameModeStateComponent stateComponent = SCR_BaseGameModeStateComponent.Cast(stateComponents[i]);
			SCR_EGameModeState state = stateComponent.GetAffiliatedState();
			// Invalid state
			if (state < 0)
			{
				Print("Skipping one of SCR_BaseGameStateComponent(s), invalid affiliated state!", LogLevel.ERROR);
				continue;
			}

			if (m_mStateComponents.Contains(state))
			{
				string stateName = SCR_Enum.GetEnumName(SCR_EGameModeState, state);
				Print("Skipping one of SCR_BaseGameStateComponent(s), duplicate component for state: " + stateName + "!", LogLevel.ERROR);
				continue;
			}

			m_mStateComponents.Insert(state, stateComponent);
		}
	}
}
