[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class SCR_SpawnTriggerEntityClass : SCR_BaseTriggerEntityClass
{
}

enum SCR_ESpawnTriggerActivation
{
	PLAYER = 0,
	ANY_CHARACTER
}

class SCR_SpawnTriggerEntity : SCR_BaseTriggerEntity
{
	[Attribute(desc: "Faction which is used for area control calculation. Leave empty for any faction.", category: "Trigger")]
	protected FactionKey m_sOwnerFactionKey;

	[Attribute("0", UIWidgets.ComboBox, "By whom the trigger is activated", "", ParamEnumArray.FromEnum(SCR_ESpawnTriggerActivation), category: "Trigger")]
	protected SCR_ESpawnTriggerActivation m_eActivationPresence;

	[Attribute(defvalue: "1", UIWidgets.CheckBox, desc: "Activate the trigger once or everytime the activation condition is true?", category: "Trigger")]
	protected bool m_bOnce;

	[Attribute(defvalue: "0", UIWidgets.Slider, desc: "Minimum players needed to activate this trigger when PLAYER Activation presence is selected", params: "0 1 0.01", precision: 2, category: "Trigger")]
	protected float m_fMinimumPlayersNeededPercentage;

	[Attribute(defvalue: "0", UIWidgets.Slider, desc: "For how long the trigger conditions must be true in order for the trigger to activate. If conditions become false, timer resets", params: "0 86400 1", category: "Trigger")]
	protected float m_fActivationCountdownTimer;

	protected ref ScriptInvoker m_OnChange;
	protected Faction m_OwnerFaction;
	protected float m_fTempWaitTime = m_fActivationCountdownTimer;
	protected bool m_bInitSequenceDone = false;
	protected ref array<IEntity> m_aEntitiesInside = {};
	protected ref array<IEntity> m_aPlayersInside = {};
	protected bool m_bTriggerConditionsStatus;
	protected int m_iCountInsideTrigger;

	static ref ScriptInvokerBase<ScriptInvokerTriggerUpdated> s_OnTriggerUpdated = new ScriptInvokerBase<ScriptInvokerTriggerUpdated>();
	static ref ScriptInvokerInt s_OnTriggerUpdatedPlayerNotPresent = new ScriptInvokerInt();

	//------------------------------------------------------------------------------------------------
	//! Sets Activation Presence
	void SetActivationPresence(SCR_ESpawnTriggerActivation EActivationPresence)
	{
		m_eActivationPresence = EActivationPresence;
	}

	//------------------------------------------------------------------------------------------------
	//! Sets specific classnames to be searched in the trigger
	void SetSpecificClassName(notnull array<string> aClassName)
	{
		foreach (ResourceName className : aClassName)
		{
			AddClassType(className.ToType());
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Sets specific prefab filters to be searched in the trigger
	void SetPrefabFilters(notnull array<ref SCR_ScenarioFrameworkPrefabFilter> aPrefabFilter)
	{
		PrefabFilter prefabFilter;
		foreach (SCR_ScenarioFrameworkPrefabFilter prefabFilterInput : aPrefabFilter)
		{
			prefabFilter = new PrefabFilter();
			prefabFilter.SetPrefab(prefabFilterInput.m_sSpecificPrefabName);
			prefabFilter.SetCheckPrefabHierarchy(prefabFilterInput.m_bIncludeChildren);
			AddPrefabFilter(prefabFilter);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Sets if trigger can be finished just once
	void SetOnce(bool bOnce)
	{
		m_bOnce = bOnce;
	}

	//------------------------------------------------------------------------------------------------
	//! Sets trigger conditions status
	void SetTriggerConditionsStatus(bool status)
	{
		m_bTriggerConditionsStatus = status;
	}

	//------------------------------------------------------------------------------------------------
	//! Sets minimum player percentage needed to finish this trigger
	void SetMinimumPlayersNeeded(float minimumPlayersNeededPercentage)
	{
		m_fMinimumPlayersNeededPercentage = minimumPlayersNeededPercentage;
	}

	//------------------------------------------------------------------------------------------------
	//! Sets activation coundown timer
	void SetActivationCountdownTimer(float activationCountdownTimer)
	{
		m_fActivationCountdownTimer = activationCountdownTimer;
	}

	//------------------------------------------------------------------------------------------------
	//! Sets faction that "owns" this trigger
	void SetOwnerFaction(FactionKey sFaction)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (factionManager)
			m_OwnerFaction = factionManager.GetFactionByKey(sFaction);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns if this is executed in a server environment
	bool IsMaster()
	{
		RplComponent comp = RplComponent.Cast(FindComponent(RplComponent));
		return comp && comp.IsMaster();
	}

	//------------------------------------------------------------------------------------------------
	//! Returns trigger faction owner
	Faction GetOwnerFaction()
	{
		return m_OwnerFaction;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns activation countdown timer
	float GetActivationCountdownTimer()
	{
		return m_fActivationCountdownTimer;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns activation countdown timer temporary value which is calculated by the trigger but changes over time
	float GetActivationCountdownTimerTemp()
	{
		return m_fTempWaitTime;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns minimum players needed percentage
	float GetMinimumPlayersNeededPercentage()
	{
		return m_fMinimumPlayersNeededPercentage;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns trigger conditions status
	bool GetTriggerConditionsStatus()
	{
		return m_bTriggerConditionsStatus;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns number of all the entities that are inside this trigger
	int GetCountInsideTrigger()
	{
		return m_iCountInsideTrigger;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns all entities that are inside this trigger
	int GetCountEntitiesInside()
	{
		GetEntitiesInside(m_aEntitiesInside);
		return m_aEntitiesInside.Count();
	}

	//------------------------------------------------------------------------------------------------
	//! Returns number of players in game by the faction set for this trigger
	int GetPlayersCountByFaction()
	{
		int iCnt = 0;
		array<int> aPlayerIDs = {};
		SCR_PlayerController playerController;
		GetGame().GetPlayerManager().GetPlayers(aPlayerIDs);
		foreach (int iPlayerID : aPlayerIDs)
		{
			if (!m_OwnerFaction)
			{
				iCnt++;			//Faction not set == ANY faction
			}
			else
			{
				playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(iPlayerID));
				if (!playerController)
					continue;

				if (playerController.GetLocalControlledEntityFaction() == m_OwnerFaction)
					iCnt++;
			}
		}

		return iCnt;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns number of specific class that is inside of this trigger
	int GetSpecificClassCountInsideTrigger(string className, int targetCount = -1)
	{
		if (className.IsEmpty())
			return 0;

		int iCnt = 0;
		GetEntitiesInside(m_aEntitiesInside);
		foreach (IEntity entity : m_aEntitiesInside)
		{
			if (!entity)
				continue;

			if (entity.IsInherited(className.ToType()))
				iCnt++;

			if (iCnt == targetCount)
				break;
		}

		return iCnt;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns number of specific prefab that is inside of this trigger
	int GetSpecificPrefabCountInsideTrigger(BaseContainer prefabContainer, int targetCount = -1, bool includeInheritance = false)
	{
		int iCnt;
		GetEntitiesInside(m_aEntitiesInside);

		foreach (IEntity entity : m_aEntitiesInside)
		{
			if (!entity)
				continue;

			EntityPrefabData prefabData = entity.GetPrefabData();
			if (!prefabData)
				continue;

			BaseContainer container = prefabData.GetPrefab();
			if (!container)
				continue;

			if (!includeInheritance)
			{
				if (container != prefabContainer)
					continue;

				iCnt++;
			}
			else
			{
				while (container)
				{
					if (container == prefabContainer)
					{
						iCnt++;
						break;
					}

					container = container.GetAncestor();
				}
			}

			if (iCnt == targetCount)
				break;
		}

		return iCnt;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns number of characters from the selected faction that are inside this trigger
	int GetCharacterCountByFactionInsideTrigger(Faction faction, int targetCount = -1)
	{
		int iCnt = 0;
		SCR_ChimeraCharacter chimeraCharacter;
		GetEntitiesInside(m_aEntitiesInside);
		foreach (IEntity entity : m_aEntitiesInside)
		{
			if (!entity)
				continue;

			chimeraCharacter = SCR_ChimeraCharacter.Cast(entity);
			if (!chimeraCharacter)
				continue;

			if (faction && chimeraCharacter.GetFaction() != faction)
				continue;

			iCnt++;
			if (iCnt == targetCount)
				break;
		}

		return iCnt;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns number of players from the selected faction that are inside this trigger
	int GetPlayersCountByFactionInsideTrigger(Faction faction)
	{
		int iCnt = 0;
		m_aPlayersInside.Clear();
		GetEntitiesInside(m_aEntitiesInside);
		SCR_ChimeraCharacter chimeraCharacter;
		foreach (IEntity entity : m_aEntitiesInside)
		{
			if (!entity)
				continue;

			chimeraCharacter = SCR_ChimeraCharacter.Cast(entity);
			if (!chimeraCharacter)
				continue;

			if (faction && chimeraCharacter.GetFaction() != faction)
				continue;

			if (!EntityUtils.IsPlayer(entity))
				continue;

			iCnt++;
			m_aPlayersInside.Insert(entity);
		}

		return iCnt;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns all the players by the faction set for this trigger
	void GetPlayersByFactionInsideTrigger(notnull out array<IEntity> aOut)
	{
		SCR_ChimeraCharacter chimeraCharacter;
		GetEntitiesInside(m_aEntitiesInside);
		foreach (IEntity entity : m_aEntitiesInside)
		{
			if (!entity)
				continue;

			chimeraCharacter = SCR_ChimeraCharacter.Cast(entity);
			if (!chimeraCharacter)
				continue;

			//Faction not set == ANY faction
			if (!m_OwnerFaction)
			{
				if (EntityUtils.IsPlayer(entity))
					aOut.Insert(entity)
			}
			else
			{
				if (chimeraCharacter.GetFaction() != m_OwnerFaction)
					continue;

				if (EntityUtils.IsPlayer(entity))
					aOut.Insert(entity);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Returns all the players in the game
	void GetPlayersByFaction(notnull out array<IEntity> aOut)
	{
		array<int> aPlayerIDs = {};
		SCR_PlayerController playerController;
		GetGame().GetPlayerManager().GetPlayers(aPlayerIDs);
		foreach (int iPlayerID : aPlayerIDs)
		{
			playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(iPlayerID));
			if (!playerController)
				continue;

			if (playerController.GetLocalControlledEntityFaction() == m_OwnerFaction)
				aOut.Insert(playerController.GetLocalMainEntity());
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Override this method in inherited class to define a new filter.
	override bool ScriptedEntityFilterForQuery(IEntity ent)
	{
		if (m_eActivationPresence == SCR_ESpawnTriggerActivation.PLAYER || m_eActivationPresence == SCR_ESpawnTriggerActivation.ANY_CHARACTER)
		{
			SCR_ChimeraCharacter chimeraCharacter = SCR_ChimeraCharacter.Cast(ent);
			if (!chimeraCharacter)
				return false;

			if (m_OwnerFaction && chimeraCharacter.GetFaction() != m_OwnerFaction)
				return false;

			if (!IsAlive(ent))
				return false;

			if (m_eActivationPresence == SCR_ESpawnTriggerActivation.PLAYER)
				return EntityUtils.IsPlayer(ent);

			return true;
		}

		//In case of vehicle, we first need to check if it is alive and then check the faction
		Vehicle vehicle = Vehicle.Cast(ent);
		if (vehicle)
		{
			if (!IsAlive(ent))
				return false;

			if (!m_OwnerFaction)
				return true;

			return vehicle.GetFaction() == m_OwnerFaction;
		}

		if (!m_OwnerFaction)
			return true;

		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(ent.FindComponent(FactionAffiliationComponent));
		if (!factionAffiliation)
			return true;

		return factionAffiliation.GetAffiliatedFaction() == m_OwnerFaction;
	}

	//------------------------------------------------------------------------------------------------
	//! Finishes trigger
	void FinishTrigger(IEntity ent)
	{
		if (m_bOnce)
			Deactivate();

		m_OnActivate.Invoke(ent);
		OnChange(ent);
	}

	//------------------------------------------------------------------------------------------------
	//! Checks activation presesence conditions
	void ActivationPresenceConditions()
	{
		m_bTriggerConditionsStatus = false;
		if (m_eActivationPresence == SCR_ESpawnTriggerActivation.PLAYER)
		{
			m_iCountInsideTrigger = GetPlayersCountByFactionInsideTrigger(m_OwnerFaction);
			float minPlayersNeeded = Math.Ceil(GetPlayersCountByFaction() * m_fMinimumPlayersNeededPercentage);
			if (m_iCountInsideTrigger >= minPlayersNeeded && m_iCountInsideTrigger > 0)
				m_bTriggerConditionsStatus = true;

			return;
		}

		m_bTriggerConditionsStatus = true;
	}

	//------------------------------------------------------------------------------------------------
	override protected event void OnActivate(IEntity ent)
	{
	}

	//------------------------------------------------------------------------------------------------
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		super.OnQueryFinished(bIsEmpty);

		if (bIsEmpty)
			return;

		if (!IsMaster())
			return;

		if (!m_bInitSequenceDone)
			return;

		ActivationPresenceConditions();

		if (m_bTriggerConditionsStatus && m_fTempWaitTime <= 0)
			FinishTrigger(this);
	}

	//------------------------------------------------------------------------------------------------
	override protected event void OnDeactivate(IEntity ent)
	{
		//This method is triggered when the trigger is deactivated but the said entity is still inside.
		//We need to perform this method after the entity leaves the trigger.
		GetGame().GetCallqueue().CallLater(OnDeactivateCalledLater, 100, false, ent);
	}

	//------------------------------------------------------------------------------------------------
	void OnDeactivateCalledLater(IEntity ent)
	{
		if (!m_bInitSequenceDone)
			return;

		m_aPlayersInside.RemoveItem(ent);

		ActivationPresenceConditions();

		m_OnDeactivate.Invoke();
		OnChange(ent);
	}

	//------------------------------------------------------------------------------------------------
	void OnChange(IEntity ent)
	{
		if (m_OnChange)
		{
			SCR_ScenarioFrameworkParam<IEntity> param = new SCR_ScenarioFrameworkParam<IEntity>(ent);
			m_OnChange.Invoke(param);
		}
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnChange()
	{
		if (!m_OnChange)
			m_OnChange = new ScriptInvoker();

		return m_OnChange;
	}

	//------------------------------------------------------------------------------------------------
	//! Sets Init sequence as done or not
	void SetInitSequenceDone(bool init)
	{
		m_bInitSequenceDone = init;
	}

	//------------------------------------------------------------------------------------------------
	//! Initializes the trigger
	override protected void EOnInit(IEntity owner)
	{
		SetOwnerFaction(m_sOwnerFactionKey);
		
		m_bInitSequenceDone = true;
	}
}
