modded enum EMessageType_Goal
{
	VEHICLE_DEFEND,
	LOOK_AT
}

class BTC_AIMessage_VehicleDefend : SCR_AIMessageGoal // MESSAGE_CLASS(GenerateSendGoalMessage, BTC_AISendGoalMessage_VehicleDefend)
{
	vector m_vDefendDirection; 		// VARIABLE(NodePort, DefendDirection)
	float m_fDefendAngularRange; 	// VARIABLE(NodePort, DefendAngularRange, NodeProperty, m_fDefendAngularRange)

	override void SetMessageParameters(SCR_AISendGoalMessage node, SCR_AIActivityBase relatedActivity)
	{
		super.SetMessageParameters(node, relatedActivity);
		node.GetVariableIn(node.PORT_ENTITY, m_RelatedWaypoint);
	}

	//------------------------------------------------------------------------------------------------
	static BTC_AIMessage_VehicleDefend Create(vector defendDirection, float defendAngularRange, bool waypointRelated,
									float priorityLevel, AIWaypoint relatedWaypoint, SCR_AIActivityBase relatedActivity)
	{
		BTC_AIMessage_VehicleDefend msg = new BTC_AIMessage_VehicleDefend();
		msg.m_vDefendDirection = defendDirection;
		msg.m_fDefendAngularRange = defendAngularRange;
		msg.m_bIsWaypointRelated = waypointRelated;
		msg.m_fPriorityLevel = priorityLevel;
		msg.m_RelatedWaypoint = relatedWaypoint;
		msg.m_RelatedGroupActivity = relatedActivity;
		return msg;
	}

	//------------------------------------------------------------------------------------------------
	void BTC_AIMessage_VehicleDefend()
	{
		m_MessageType = EMessageType_Goal.VEHICLE_DEFEND;
	}
}

class BTC_AIMessage_LookAt : SCR_AIMessageGoal // MESSAGE_CLASS(GenerateSendGoalMessage, BTC_AISendGoalMessage_LookAt)
{
	vector m_vLookAtPos; // VARIABLE(NodePort, LookAtPos)

	//------------------------------------------------------------------------------------------------
	void BTC_AIMessage_LookAt()
	{
		m_MessageType = EMessageType_Goal.LOOK_AT;
	}
}
