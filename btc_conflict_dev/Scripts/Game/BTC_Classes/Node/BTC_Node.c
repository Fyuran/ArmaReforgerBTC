class BTC_Node : Managed
{
	protected BTC_Node m_PreviousNode;
	protected BTC_Node m_NextNode;

	//------------------------------------------------------------------------------------------------
	BTC_Node GetPrevious()
	{
		return m_PreviousNode;
	}

	//------------------------------------------------------------------------------------------------
	BTC_Node GetNext()
	{
		return m_NextNode;
	}

	//------------------------------------------------------------------------------------------------
	BTC_Node SetNext(BTC_Node newNode)
	{
		m_NextNode = newNode;
		return newNode;
	}

	//------------------------------------------------------------------------------------------------
	BTC_Node SetPrevious(BTC_Node newNode)
	{
		m_PreviousNode = newNode;
		return newNode;
	}
}
