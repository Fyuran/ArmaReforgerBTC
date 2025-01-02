//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class BTC_VehicleStruct : SCR_JsonApiStruct
{
	protected vector m_Position;
	protected vector m_Rotation;
	protected EVehicleType type;
	protected Faction m_Faction;
	protected SCR_ResourceComponent m_ResourceComponent;
}

/*
BaseWorld world = GetGame().GetWorld();

IEntity vehicle = world.FindEntityByName("TestVeh");
ResourceName resourceName = SCR_BaseContainerTools.GetPrefabResourceName(vehicle.GetPrefabData().GetPrefab());
Resource resource = Resource.Load(resourceName);

IEntity player = SCR_PlayerController.Cast(GetGame().GetPlayerController()).GetLocalControlledEntity();

CameraManager cameraManager = GetGame().GetCameraManager();
CameraBase camera = cameraManager.CurrentCamera();

vector rot[4];		
camera.GetWorldTransform(rot);
rot[3] = vector.Zero;

vector forward = vector.Forward;	
forward = forward.Multiply3(rot);

vector mat[4];
camera.GetWorldTransform(mat);
mat[3] = mat[3] + (forward * 10);	

EntitySpawnParams params = EntitySpawnParams();
params.TransformMode = ETransformMode.WORLD;
params.Transform = mat;	

IEntity newVeh = GetGame().SpawnEntityPrefab(resource, world, params);
if(!newVeh)
	Print("Entity not spawned", LogLevel.ERROR);

*/