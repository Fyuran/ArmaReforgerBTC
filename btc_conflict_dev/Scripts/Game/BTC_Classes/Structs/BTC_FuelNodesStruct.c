class BTC_FuelNodesStruct
{
	private ref map<int, ref BTC_FuelNodeStruct> m_Structs = new map<int, ref BTC_FuelNodeStruct>();

	//------------------------------------------------------------------------------------------------
	static BTC_FuelNodesStruct Serialize(SCR_FuelManagerComponent fuelManager)
	{
		BTC_FuelNodesStruct struct = new BTC_FuelNodesStruct();
		array<SCR_FuelNode> fuelNodes = {};
		fuelManager.GetScriptedFuelNodesList(fuelNodes);

		if (fuelNodes.IsEmpty())
			return struct;

		foreach (SCR_FuelNode fuelNode : fuelNodes)
		{
			struct.m_Structs.Insert(fuelNode.GetFuelTankID(), BTC_FuelNodeStruct.Serialize(fuelNode));
		}

		return struct;
	}

	//------------------------------------------------------------------------------------------------
	void Deserialize(SCR_FuelManagerComponent fuelManager)
	{
		array<SCR_FuelNode> fuelNodes = {};
		fuelManager.GetScriptedFuelNodesList(fuelNodes);

		if (fuelNodes.IsEmpty())
			return;

		foreach (SCR_FuelNode fuelNode : fuelNodes)
		{
			BTC_FuelNodeStruct struct = m_Structs.Get(fuelNode.GetFuelTankID());
			
			if (!struct)
				continue;
			
			struct.Deserialize(fuelNode);
		}
	}
}

class BTC_FuelNodeStruct
{
	protected float m_fHealth = 0;
	protected float m_fFuel = 0;

	//------------------------------------------------------------------------------------------------
	void Deserialize(SCR_FuelNode node)
	{
		node.SetFuel(m_fFuel);
	}

	//------------------------------------------------------------------------------------------------
	static BTC_FuelNodeStruct Serialize(SCR_FuelNode node)
	{
		BTC_FuelNodeStruct struct = new BTC_FuelNodeStruct();
		if (!node)
			return struct;

		struct.m_fHealth = node.GetHealth();
		struct.m_fFuel = node.GetFuel();

		return struct;
	}

	//------------------------------------------------------------------------------------------------
	float GetFuelNodeHealth()
	{
		return m_fHealth;
	}

	//------------------------------------------------------------------------------------------------
	float GetFuelNodeFuel()
	{
		return m_fFuel;
	}
}
