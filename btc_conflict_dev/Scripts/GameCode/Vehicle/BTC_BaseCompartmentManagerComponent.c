modded class SCR_BaseCompartmentManagerComponent : BaseCompartmentManagerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Spawns characters as defined in prefabs passed (Server only)
	//! Places all spawned characters in the same group
	//! Note that it can only spawn the same compartment type once and will fail if you try to do it twice.
	//! If the system is already spawning default characters and you call it again for other department types it will send them to a queue and execute them after the current compartment types are filled.
	//! \param compartmentTypes Given compartment types that need to be filled with characters
	//! \return True if start spawning occupants
	bool SpawnCatalogOccupants(notnull array<ECompartmentType> compartmentTypes, array<ResourceName> occupantPrefabs)
	{
		//~ No compartment types given to spawn
		if (compartmentTypes.IsEmpty())
		{
			if (Event_OnDoneSpawningDefaultOccupants)
				Event_OnDoneSpawningDefaultOccupants.Invoke(this, null, true);

			return false;
		}

		if (occupantPrefabs.IsEmpty())
			return false;

		//~ Check if is already spawning
		for (int i = 0; i < m_aDefaultOccupantsCompartmentTypesToSpawn.Count(); i++)
		{
			if (!m_aDefaultOccupantsCompartmentTypesToSpawn[i] || m_aDefaultOccupantsCompartmentTypesToSpawn[i].IsEmpty())
			{
				m_aDefaultOccupantsCompartmentTypesToSpawn.RemoveOrdered(i);
				i--;
				continue;
			}

			foreach (ECompartmentType type : compartmentTypes)
			{
				//~ Is already spawning the same type
				if (m_aDefaultOccupantsCompartmentTypesToSpawn[i].Contains(type))
				{
					Print(string.Format("'SCR_BaseCompartmentManagerComponent' is already spawning default occupants of type %1!", typename.EnumToString(ECompartmentType, type)), LogLevel.WARNING);
					return false;
				}
			}
		}

		//~ Add to spawn list
		m_aDefaultOccupantsCompartmentTypesToSpawn.Insert(compartmentTypes);

		//~ Directly spawn in vehicle as not currently spawning anything
		if (!m_bIsSpawningDefaultOccupants)
		{
			return InitiateSpawnCatalogOccupants(compartmentTypes, occupantPrefabs);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Actually initialize
	//! Compiles an array of compartment slots that have to be later filled
	protected bool InitiateSpawnCatalogOccupants(notnull array<ECompartmentType> compartmentTypes, array<ResourceName> occupantPrefabs)
	{
		m_bIsSpawningDefaultOccupants = true;
		m_aCompartmentsToSpawnDefaultOccupant = {};

		foreach (ECompartmentType compartmentType : compartmentTypes)
		{
			GetFreeCompartmentsOfType(m_aCompartmentsToSpawnDefaultOccupant, compartmentType, true);
		}

		//~ Nothing to spawn in so cancel
		if (m_aCompartmentsToSpawnDefaultOccupant.IsEmpty())
		{
			FinishedSpawningCatalogOccupants(true);
			return false;
		}

		m_aSpawnedDefaultOccupants = {};
		m_SpawnedOccupantsAIGroup = null;

		GetGame().GetCallqueue().CallLater(SpawnCatalogOccupantEachFrame, 0, true, occupantPrefabs);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Spawns a character each frame to take the load of the server
	//! Will parse each compartment slot and remove it when filled
	protected void SpawnCatalogOccupantEachFrame(array<ResourceName> occupantPrefabs)
	{
		BaseCompartmentSlot compartmentToFill = null;

		for (int i = 0; i < m_aCompartmentsToSpawnDefaultOccupant.Count(); i++)
		{
			//~ Invalid compartment so remove it from the list
			if (!m_aCompartmentsToSpawnDefaultOccupant[i] || m_aCompartmentsToSpawnDefaultOccupant[i].IsOccupied() || !m_aCompartmentsToSpawnDefaultOccupant[i].IsCompartmentAccessible())
			{
				m_aCompartmentsToSpawnDefaultOccupant.RemoveOrdered(i);
				i--;
				continue;
			}

			compartmentToFill = m_aCompartmentsToSpawnDefaultOccupant[i];
			m_aCompartmentsToSpawnDefaultOccupant.RemoveOrdered(i);
			break;
		}

		//~ No compartments found
		if (!compartmentToFill)
		{
			FinishedSpawningCatalogOccupants(false, occupantPrefabs);
			return;
		}

		IEntity spawnedCharacter = compartmentToFill.SpawnCharacterInCompartment(occupantPrefabs.GetRandomElement(), m_SpawnedOccupantsAIGroup);
		if (!spawnedCharacter)
			return;


		m_aSpawnedDefaultOccupants.Insert(spawnedCharacter);

		if (m_aCompartmentsToSpawnDefaultOccupant.IsEmpty())
			FinishedSpawningCatalogOccupants(false, occupantPrefabs);
	}

	//~ Called when finished spawning. Note that it will spawn more passengers if it has a queue
	protected void FinishedSpawningCatalogOccupants(bool wasCanceled, array<ResourceName> occupantPrefabs = null)
	{
		m_bIsSpawningDefaultOccupants = false;

		if (Event_OnDoneSpawningDefaultOccupants)
			Event_OnDoneSpawningDefaultOccupants.Invoke(this, m_aSpawnedDefaultOccupants, wasCanceled);

		m_aSpawnedDefaultOccupants = null;
		m_aCompartmentsToSpawnDefaultOccupant = null;
		GetGame().GetCallqueue().Remove(SpawnCatalogOccupantEachFrame);

		//~ Clear queue
		if (wasCanceled)
		{
			m_aDefaultOccupantsCompartmentTypesToSpawn.Clear();
		}
		//~ next in queue
		else
		{
			//~ Delete the one just spawned
			m_aDefaultOccupantsCompartmentTypesToSpawn.RemoveOrdered(m_aDefaultOccupantsCompartmentTypesToSpawn.Count() -1);
			//~ Spawn the entities that where waiting to be spawned
			if (!m_aDefaultOccupantsCompartmentTypesToSpawn.IsEmpty() && occupantPrefabs)
				InitiateSpawnCatalogOccupants(m_aDefaultOccupantsCompartmentTypesToSpawn[0], occupantPrefabs);
		}
	}
}
