class BTC_AILookAtActivity : SCR_AIActivityBase
{
	ref SCR_BTParamAssignable<vector> m_vLookAtPos = new SCR_BTParamAssignable<vector>("LookAtPos");

	//------------------------------------------------------------------------------------------------
	void InitParameters(vector lookAtPos, AIWaypoint waypoint)
	{
		m_vLookAtPos.Init(this, lookAtPos);
		m_vLookAtPos.m_AssignedOut = (m_vLookAtPos.m_Value != vector.Zero);
		m_RelatedWaypoint = waypoint;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_AILookAtActivity(SCR_AIGroupUtilityComponent utility, AIWaypoint relatedWaypoint, vector lookAtPos, float priority = PRIORITY_ACTIVITY_MOVE, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		InitParameters(lookAtPos, relatedWaypoint);
		m_sBehaviorTree = "{E8F97EB099EAD2DE}Scripts/Game/AI/BehaviorTrees/Chimera/Group/BTC_LookAtActivity.bt";
		SetPriority(priority);
	}

	//------------------------------------------------------------------------------------------------
	override string GetActionDebugInfo()
	{
		return this.ToString() + " looking to " + m_vLookAtPos.m_Value.ToString();
	}
}
