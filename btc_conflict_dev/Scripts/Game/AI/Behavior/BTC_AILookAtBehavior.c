class BTC_AILookAtBehavior : SCR_AIBehaviorBase
{
	ref SCR_BTParamAssignable<vector> m_vLookAtPos = new SCR_BTParamAssignable<vector>("LookAtPos");
	ref SCR_BTParam<AIWaypoint> m_RelatedWaypoint = new SCR_BTParam<AIWaypoint>("Waypoint");

	//------------------------------------------------------------------------------------------------
	void InitParameters(vector lookAtPos, AIWaypoint waypoint)
	{
		m_vLookAtPos.Init(this, lookAtPos);
		m_vLookAtPos.m_AssignedOut = (m_vLookAtPos.m_Value != vector.Zero);
		m_RelatedWaypoint.Init(this, waypoint);
	}

	//------------------------------------------------------------------------------------------------
	void BTC_AILookAtBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, AIWaypoint relatedWaypoint, vector lookAtPos, float priority = PRIORITY_BEHAVIOR_MOVE, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		InitParameters(lookAtPos, relatedWaypoint);
		m_sBehaviorTree = "{AA54A46EB796FD47}Scripts/Game/AI/BehaviorTrees/Chimera/Soldier/BTC_LookAtBehavior.bt";
		SetPriority(priority);
	}

	//------------------------------------------------------------------------------------------------
	override string GetActionDebugInfo()
	{
		return this.ToString() + " looking to " + m_vLookAtPos.m_Value.ToString();
	}
}
