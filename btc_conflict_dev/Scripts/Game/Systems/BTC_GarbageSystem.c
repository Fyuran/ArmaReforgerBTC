modded class SCR_GarbageSystem : GarbageSystem
{
	//------------------------------------------------------------------------------------------------
	protected bool IsEntityAlive(IEntity entity)
	{
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(entity);
		if (damageManager)
			return damageManager.GetState() != EDamageState.DESTROYED;
		else
			return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float OnInsertRequested(IEntity entity, float lifetime)
	{
		SCR_GameModeCampaign gameMode = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		
		lifetime = super.OnInsertRequested(entity, lifetime);
		
		if(gameMode) 
		{	
			if (Vehicle.Cast(entity) && IsEntityAlive(entity))
			{
				if(gameMode.GetPreventRemoveVehicles())
				{
					lifetime = -1;
				}
			}
		}

		return lifetime;
	}
}
