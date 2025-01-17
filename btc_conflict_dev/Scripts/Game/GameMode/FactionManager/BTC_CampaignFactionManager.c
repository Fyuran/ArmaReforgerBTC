modded class SCR_CampaignFactionManager : SCR_FactionManager
{
	[Attribute("USSR", UIWidgets.EditBox, "Faction Key of enemy faction.")]
	protected FactionKey m_EnemyFactionKey;

	//------------------------------------------------------------------------------------------------
	//! \param[in] alliedFaction
	//! \return
	override SCR_CampaignFaction GetEnemyFaction(notnull SCR_CampaignFaction alliedFaction)
	{
		array<Faction> factions = {};
		GetFactionsList(factions);

		SCR_CampaignFaction enemyFaction = SCR_CampaignFaction.Cast(GetFactionByKey(m_EnemyFactionKey));
		if (enemyFaction)
			return enemyFaction;

		for (int i = factions.Count() - 1; i >= 0; i--)
		{
			SCR_Faction factionCast = SCR_Faction.Cast(factions[i]);

			if (factionCast && factionCast.IsMilitary() && factionCast != alliedFaction)
				return SCR_CampaignFaction.Cast(factionCast);
		}

		return null;
	}

}
