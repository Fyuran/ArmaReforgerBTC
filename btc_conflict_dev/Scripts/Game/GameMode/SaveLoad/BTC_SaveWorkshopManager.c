/*!
Scripted api for handling workshop and backend interactions
*/

modded class SCR_SaveWorkshopManager
{
	//------------------------------------------------------------------------------------------------
	//! Get resource path of current scenario or fallback to path of testing world
	override static ResourceName GetCurrentScenarioId()
	{
		MissionHeader header = GetGame().GetMissionHeader();

		#ifdef WORKBENCH
		if (!header)
			return FALLBACK_SCENARIO_ID;
		#endif
		#ifdef SAVE_MANAGER_DEBUG_HEADER
		if (!header)
			return FALLBACK_SCENARIO_ID;
		#endif

		return header.GetHeaderResourceName();
	}

	//------------------------------------------------------------------------------------------------
	//! Get name of current scenario or fallback to testing world name for save file name
	override static string GetCurrentScenarioNameFile()
	{
		// Get ws item from mission
		MissionWorkshopItem missionItem = GetCurrentScenario();

		#ifdef WORKBENCH
		if (!missionItem)
			return FALLBACK_SCENARIO_NAME;
		#endif
		#ifdef SAVE_MANAGER_DEBUG_HEADER
		if (!missionItem)
			return FALLBACK_SCENARIO_NAME;
		#endif

		return GetScenarioNameFile(missionItem);
	}

	//------------------------------------------------------------------------------------------------
	//! Get translated name of current scenario or fallback to testing world name
	override static string GetCurrentScenarioNameTranslated()
	{
		// Get ws item from mission
		MissionWorkshopItem missionItem = GetCurrentScenario();

		#ifdef WORKBENCH
		if (!missionItem)
			return FALLBACK_SCENARIO_NAME;
		#endif
		#ifdef SAVE_MANAGER_DEBUG_HEADER
		if (!missionItem)
			return FALLBACK_SCENARIO_NAME;
		#endif

		if (!missionItem)
			return string.Empty;

		string name = missionItem.Name();

		if (name.Length() > 0 && name[0] == "#")
			name = WidgetManager.Translate(name);

		return name;
	}

	//------------------------------------------------------------------------------------------------
	//! Get thumbnail of current scenario or use fallback back image
	override ResourceName CurrentScenarioImage()
	{
		MissionWorkshopItem missionItem = GetCurrentScenario();

		#ifdef WORKBENCH
		if (!missionItem)
			return "E:/aRMA4/A4Data/UI/Textures/MissionLoadingScreens/Reforger_ConflictArt_UI.png";
		#endif
		#ifdef SAVE_MANAGER_DEBUG_HEADER
		if (!missionItem)
			return "E:/aRMA4/A4Data/UI/Textures/MissionLoadingScreens/Reforger_ConflictArt_UI.png";
		#endif

		return missionItem.Thumbnail().GetLocalScale(THUMBNAIL_WIDTH).Path();
	}

}
