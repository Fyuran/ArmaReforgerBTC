class BTC_AIVehicleDefendActivity : SCR_AIActivityBase
{
	//------------------------------------------------------------------------------------------------
	void InitParameters(float priorityLevel)
	{
		m_fPriorityLevel.Init(this, priorityLevel);
	}

	//------------------------------------------------------------------------------------------------
	void BTC_AIVehicleDefendActivity(SCR_AIGroupUtilityComponent utility, AIWaypoint relatedWaypoint, vector defendDirection, float priority = PRIORITY_ACTIVITY_DEFEND, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		InitParameters(priorityLevel);
		m_sBehaviorTree = "{8ED3430293D16C93}AI/BehaviorTrees/Chimera/Group/BTC_ActivityVehicleDefend.bt";
		SetPriority(priority);
		if (!relatedWaypoint)
			return;
	}
}
