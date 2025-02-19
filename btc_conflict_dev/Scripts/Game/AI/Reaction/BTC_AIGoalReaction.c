[BaseContainerProps()]
class BTC_AIGoalReaction_LookAt : SCR_AIGoalReaction
{
	//------------------------------------------------------------------------------------------------
	override void PerformReaction(notnull SCR_AIUtilityComponent utility, SCR_AIMessageBase message)
	{
		auto msg = BTC_AIMessage_LookAt.Cast(message);
		if (!msg)
			return;

		auto behavior = new BTC_AILookAtBehavior(utility, msg.m_RelatedGroupActivity, msg.m_RelatedWaypoint, msg.m_vLookAtPos);

		utility.AddAction(behavior);
	}

	//------------------------------------------------------------------------------------------------
	override void PerformReaction(notnull SCR_AIGroupUtilityComponent utility, SCR_AIMessageBase message)
	{
		auto msg = BTC_AIMessage_LookAt.Cast(message);
		if (!msg)
			return;

		auto activity = new BTC_AILookAtActivity(utility, msg.m_RelatedWaypoint, msg.m_vLookAtPos);

		utility.AddAction(activity);
	}
}

[BaseContainerProps()]
class BTC_AIGoalReaction_VehicleDefend : SCR_AIGoalReaction
{
	//------------------------------------------------------------------------------------------------
	override void PerformReaction(notnull SCR_AIUtilityComponent utility, SCR_AIMessageBase message)
	{
		auto msg = BTC_AIMessage_VehicleDefend.Cast(message);
		if (!msg)
			return;

		auto behavior = new BTC_AIVehicleDefendBehavior(utility, msg.m_RelatedGroupActivity, msg.m_RelatedWaypoint,
			msg.m_vDefendDirection, msg.m_fDefendAngularRange, priorityLevel: msg.m_fPriorityLevel);

		utility.AddAction(behavior);
	}

	//------------------------------------------------------------------------------------------------
	override void PerformReaction(notnull SCR_AIGroupUtilityComponent utility, SCR_AIMessageBase message)
	{
		auto msg = BTC_AIMessage_VehicleDefend.Cast(message);
		if (!msg)
			return;

		auto activity = new BTC_AIVehicleDefendActivity(utility, msg.m_RelatedWaypoint, msg.m_vDefendDirection,
			priorityLevel: msg.m_fPriorityLevel);

		utility.AddAction(activity);
	}
}
