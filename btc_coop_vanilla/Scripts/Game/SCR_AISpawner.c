class SCR_AISpawner
{
/*
		//trigger spawn code
	
		super.OnActivate(ent);
		
  		override void OnActivate(IEntity ent)
  		{
   			string spawnGroup = "{C8622D0595B48437}Prefabs/Groups/OPFOR/Group_USSR_AmmoTeam.et";
    		string waypointType = "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et";
    
    		IEntity spawnPosition = GetGame().GetWorld().FindEntityByName("Test_Spawn");
    		IEntity waypointPosition = GetGame().GetWorld().FindEntityByName("Test_Move");
    
    		GetGame().GetCallqueue().CallLater(SCR_AISpawner.SpawnAI, 10000, false, spawnGroup, spawnPosition.GetOrigin(), waypointType, waypointPosition.GetOrigin());
 		}
	
*/	
	//------------------------------------------------------------------------------------------------
	//! Spawns AI
	//! \param[in] spawnGroup prefab entity for the group to be spawned
	//! \param[in] spawnPosition where to spawn the group
	//! \param[in] waypointType what kind of waypoint should AI execute on spawn
	//! \param[in] waypointPosition where should AI go in case of move waypoints
	static void SpawnAI(string spawnGroup, vector spawnPosition, string waypointType, vector waypointPosition)
	{
		// Generate the resource 
		Resource resource = GenerateAndValidateResource(spawnGroup);
		
		if (!resource)
		{
			Print(("[AI Spawner] Unable able to load resource for the spawn group: " + spawnGroup), LogLevel.ERROR);
			return;
		}
		
		// Generate the spawn parameters and spawn the group 
		SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, GenerateSpawnParameters(spawnPosition)));
		
		if (!group)
		{
			Print("[AI Spawner] Unable to spawn group!", LogLevel.ERROR);
			return;
		}
		
		Print("Group" + group);
		
		// Create a waypoint for this group
		CreateWaypoint(group, waypointType, waypointPosition);
	}
	
	//------------------------------------------------------------------------------------------------
	protected static void CreateWaypoint(SCR_AIGroup group, string waypointType, vector waypointPosition)
	{
		// Generate the resource 
		Resource resource = GenerateAndValidateResource(waypointType);
		
		if (!resource)
		{
			Print(("[Create Waypoint] Unable able to load resource for the waypoint: " + waypointType), LogLevel.ERROR);
			return;
		}
		
		// Generate the spawn parameters and create the waypoint 
		AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource, null, GenerateSpawnParameters(waypointPosition)));
		
		if (!waypoint)
		{
			Print("[Create Waypoint] Unable to create waypoint!", LogLevel.ERROR);
			return;
		}
		
		Print("Waypoint created" + waypoint);
		
		// Assign the waypoint to the group 
		group.AddWaypoint(waypoint);
	}
	
	//------------------------------------------------------------------------------------------------
	protected static Resource GenerateAndValidateResource(string resourceToLoad)
	{
		// Load the resource 
		Resource resource = Resource.Load(resourceToLoad);
		
		// Validate the prefab and show an appropriate error if invalid 
		if (!resource.IsValid())
		{
			Print(("[Generate And Validate Resource] Resource is invalid: " + resourceToLoad), LogLevel.ERROR);
			return null;
		}
		
		// Return the resource 
		return resource;
	}
	
	//------------------------------------------------------------------------------------------------
	protected static EntitySpawnParams GenerateSpawnParameters(vector position)
	{
		// Create a new set of spawn parameters 
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		// Assign the position to those parameters 
		params.Transform[3] = position;
		
		// Return this set of spawn parameters
		return params;
	}
};