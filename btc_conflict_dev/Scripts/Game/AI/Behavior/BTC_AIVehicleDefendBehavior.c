class BTC_AIVehicleDefendBehavior : SCR_AIDefendBehavior
{
	//------------------------------------------------------------------------------------------------
	override void InitParameters(AIWaypoint relatedWaypoint, vector defendLocation, float defendAngularRange)
	{
		m_RelatedWaypoint.Init(this, relatedWaypoint);
		m_fDefendLocation.Init(this, defendLocation);
		m_fDefendAngularRange.Init(this, defendAngularRange);
	}

	//------------------------------------------------------------------------------------------------
	void BTC_AIVehicleDefendBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, AIWaypoint relatedWaypoint, vector location, float angularRange, float priority = PRIORITY_BEHAVIOR_DEFEND, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		m_sBehaviorTree = "{421A38F1D2D2BA91}AI/BehaviorTrees/Chimera/Soldier/BTC_BehaviorVehicleDefend.bt";
		SetPriority(priority);
		m_fPriorityLevel.m_Value = priorityLevel;
		InitParameters(relatedWaypoint, location, angularRange);
	}
}
