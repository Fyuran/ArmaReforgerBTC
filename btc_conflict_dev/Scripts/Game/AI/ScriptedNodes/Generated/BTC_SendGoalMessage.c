class BTC_AISendGoalMessage_VehicleDefend : SCR_AISendGoalMessage_Defend
{
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		AIAgent receiver = GetReceiverAgent(owner);
		BTC_AIMessage_VehicleDefend msg = new BTC_AIMessage_VehicleDefend();

		msg.m_RelatedGroupActivity = GetRelatedActivity(owner);

		msg.SetText(m_sText);

		if (!GetVariableIn("PriorityLevel", msg.m_fPriorityLevel))
			msg.m_fPriorityLevel = m_fPriorityLevel;

		if (!GetVariableIn("IsWaypointRelated", msg.m_bIsWaypointRelated))
			msg.m_bIsWaypointRelated = m_bIsWaypointRelated;

		GetVariableIn("DefendDirection", msg.m_vDefendDirection);

		if (!GetVariableIn("DefendAngularRange", msg.m_fDefendAngularRange))
			msg.m_fDefendAngularRange = m_fDefendAngularRange;

		if (msg.m_bIsWaypointRelated)
			msg.m_RelatedWaypoint = GetRelatedWaypoint(owner);

		if (SendMessage(owner, receiver, msg))
			return ENodeResult.SUCCESS;
		else
			return ENodeResult.FAIL;
	}
}

//------------------------------------------------------------------------------------------------
// Generated from class: BTC_AIMessage_LookAt
class BTC_AISendGoalMessage_LookAt : SCR_AISendMessageGenerated
{
	protected static ref TStringArray _s_aVarsIn =
	{
		SCR_AISendMessageGenerated.PORT_RECEIVER,
		"LookAtPos"
	};
	override TStringArray GetVariablesIn() { return _s_aVarsIn; }

	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		AIAgent receiver = GetReceiverAgent(owner);
		BTC_AIMessage_LookAt msg = new BTC_AIMessage_LookAt();

		msg.m_RelatedGroupActivity = GetRelatedActivity(owner);

		msg.SetText(m_sText);

		GetVariableIn("LookAtPos", msg.m_vLookAtPos);

		if (msg.m_bIsWaypointRelated)
			msg.m_RelatedWaypoint = GetRelatedWaypoint(owner);

		if (SendMessage(owner, receiver, msg))
			return ENodeResult.SUCCESS;
		else
			return ENodeResult.FAIL;
	}

	override bool VisibleInPalette() { return true; }
}
