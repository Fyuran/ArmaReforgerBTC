class BTC_AISendVehicleDefendMsgs : AITaskScripted
{
	static const string PORT_WAYPOINT = "WaypointIn";
	static const string PORT_PRIORITY_LEVEL = "PriorityLevel";
	static const string PORT_DEFEND_DIRECTION = "DefendDirection";
	static const string PORT_MOVE_POSITION = "MovePosition";
	
	protected SCR_AIGroup m_GroupOwner;
	protected float m_fPriorityLevel;
	protected SCR_AIGroupUtilityComponent m_Utility;
	protected SCR_MailboxComponent m_Mailbox;
	protected AIWaypoint m_RelatedWaypoint;
	protected vector m_vDefendDirection;
	
	#ifdef WORKBENCH
	protected Shape m_Arrow;
	protected Shape m_Sphere;
	#endif
	
	//------------------------------------------------------------------------------------------------
	override bool VisibleInPalette() {return true; }

	//------------------------------------------------------------------------------------------------
	override void OnInit(AIAgent owner)
	{
		m_GroupOwner = SCR_AIGroup.Cast(owner);
		if (!m_GroupOwner)
		{
			m_GroupOwner = SCR_AIGroup.Cast(owner.GetParentGroup());
			if (!m_GroupOwner)
			{
				SCR_AgentMustBeAIGroup(this, owner);
				return;
			}
		}
		m_Utility = SCR_AIGroupUtilityComponent.Cast(m_GroupOwner.FindComponent(SCR_AIGroupUtilityComponent));
		m_Mailbox = m_Utility.m_Mailbox;
		m_RelatedWaypoint = m_GroupOwner.GetCurrentWaypoint();
	}

	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		if (!m_GroupOwner)
			return ENodeResult.FAIL;
		
		if (!m_Mailbox || !m_Utility)
			return ENodeResult.FAIL;

		if(!m_RelatedWaypoint)
			return NodeError(this, owner, "Waypoint is required");
		
		GetVariableIn(PORT_PRIORITY_LEVEL, m_fPriorityLevel);
		
		float waypointRadius = m_RelatedWaypoint.GetCompletionRadius();
		if(!GetVariableIn(PORT_DEFEND_DIRECTION, m_vDefendDirection))
			m_vDefendDirection = GetDefendDirection(m_RelatedWaypoint, waypointRadius);

		array<AIAgent> groupMembers = {};
		m_GroupOwner.GetAgents(groupMembers);
		
		if (groupMembers.IsEmpty())
			return ENodeResult.FAIL;
		
		if(!AllocateNewPosition())
			return ENodeResult.FAIL;
		
		return ENodeResult.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	bool AllocateNewPosition()
	{
		vector originOfLocalSpace = m_RelatedWaypoint.GetOrigin();
		float angleToDefend = m_vDefendDirection.ToYaw();
		float length = m_vDefendDirection.Length();
		float angleRange = 360 / 2;
		int directionAngle = Math.Round(angleToDefend + 360);
		
		vector directionAxis;
		directionAngle = directionAngle % 360;
		directionAxis[0] = Math.Sin(directionAngle * Math.DEG2RAD) * length;
		directionAxis[2] = Math.Cos(directionAngle * Math.DEG2RAD) * length;
		
		directionAxis[1] = SCR_TerrainHelper.GetTerrainY(directionAxis, GetGame().GetWorld(), true);
		
		#ifdef WORKBENCH
		IEntity leader = m_GroupOwner.GetLeaderEntity();
			if (DiagMenu.GetBool(SCR_DebugMenuID.DEBUGUI_AI_SHOW_DEBUG_SHAPES))
			{
				m_Arrow = Shape.CreateArrow(leader.GetOrigin(), directionAxis, 0.2, ARGB(100,255,0,0), 0);
				m_Sphere = Shape.CreateSphere(ARGB(100,255,0,0), ShapeFlags.TRANSP | ShapeFlags.NOOUTLINE, leader.GetOrigin(), 0.05);
			}		
		#endif
		
		BTC_AIVehicleDefendActivity relatedActivity = BTC_AIVehicleDefendActivity.Cast(m_Utility.GetCurrentAction());
		SCR_AIMessage_Move msg = SCR_AIMessage_Move.Create(null, directionAxis, EMovementType.RUN, true, relatedActivity);
		
		AICommunicationComponent mailbox = m_GroupOwner.GetCommunicationComponent();
		return mailbox.RequestBroadcast(msg, m_GroupOwner);
	}

	//------------------------------------------------------------------------------------------------
	//! calculates a direction vector from centralEntity towards frontal direction of the centralEntity
	static vector GetDefendDirection(IEntity centralEntity, float directionDistance)
	{
		vector startPositionWorld = centralEntity.GetOrigin();
		float rotationAngle = centralEntity.GetAngles()[1] * Math.DEG2RAD;
		vector endPositionWorld;
		endPositionWorld[0] = startPositionWorld[0] + Math.Sin(rotationAngle) * directionDistance;
		endPositionWorld[1] = startPositionWorld[1];
		endPositionWorld[2] = startPositionWorld[2] + Math.Cos(rotationAngle) * directionDistance;
		return endPositionWorld;
	}
	
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = {
		PORT_PRIORITY_LEVEL
	};

	//------------------------------------------------------------------------------------------------
	override TStringArray GetVariablesIn()
	{
		return s_aVarsIn;
	}
	
	//------------------------------------------------------------------------------------------------
	override string GetOnHoverDescription()
	{
		return "BTC_AISendVehicleDefendMsgs: Goes over every vehicle in the group and assigns their cover waypoints.\n Works only inside defend activity under defend waypoint.";
	}
}
