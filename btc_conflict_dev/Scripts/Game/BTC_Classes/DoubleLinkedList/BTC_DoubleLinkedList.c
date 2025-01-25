class BTC_NodeFunction
{
	void Apply(BTC_Node node);
}

class BTC_DoubleLinkedList
{
	protected BTC_Node m_Head;
	protected BTC_Node m_Tail;
	protected ref array<ref BTC_Node> m_aNodes = {};

	//------------------------------------------------------------------------------------------------
	//! Inserts value at the end of the list.
	int InsertAtEnd(BTC_Node node)
	{
		if (!m_Head)
		{
			m_Head = node;
			m_Tail = node;
		}
		else
		{
			m_Tail.SetNext(node);
			node.SetPrevious(m_Tail);
			m_Tail = node;
		}

		return m_aNodes.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Inserts value at the start of the list.
	int InsertAtBeginning(BTC_Node node)
	{
		if (!m_Head)
		{
			m_Head = node;
			m_Tail = node;
		}
		else
		{
			m_Head.SetPrevious(node);
			node.SetNext(m_Head);
			m_Head = node;
		}

		return m_aNodes.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Executes functor for each element
	void ForEach(BTC_NodeFunction function)
	{
		foreach (BTC_Node node : m_aNodes)
		{
			function.Apply(node);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Inserts value at the index of the list.
	int InsertAtIndex(BTC_Node node, int index)
	{
		if (index < 0 || index > m_aNodes.Count())
			return null;

		BTC_Node temp = m_Head;
		for (int i = 0; i < index - 1; i++)
		{
			temp = temp.GetNext();
		}
		node.SetNext(temp.GetNext());
		temp.GetNext().SetPrevious(node);
		temp.SetNext(node);
		node.SetPrevious(temp);

		return m_aNodes.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Deletes the starting node of the list.
	void DeleteFirstNode()
	{
		if (!m_aNodes.IsEmpty())
			return;

		m_aNodes.RemoveOrdered(0);
	}

	//------------------------------------------------------------------------------------------------
	void DeleteLastNode()
	{
		if (!m_aNodes.IsEmpty())
			return;

		m_aNodes.RemoveOrdered(m_aNodes.Count() - 1);
	}

	//------------------------------------------------------------------------------------------------
	bool DeleteNodeAtIndex(int index)
	{

		if (index < 0 || index > m_aNodes.Count())
			return false;

		BTC_Node temp = m_Head;
		for (int i = 0; i < index; i++)
		{
			temp = temp.GetNext();
		}

		temp.GetPrevious().SetNext(temp.GetNext());
		temp.GetNext().SetPrevious(temp.GetPrevious());

		return m_aNodes.RemoveItemOrdered(temp);
	}

	//------------------------------------------------------------------------------------------------
	//! Iterates siblings to find node to be deleted
	bool DeleteNode(BTC_Node node)
	{
		int nodeIndex = FindNodeIndex(node);
		if (nodeIndex == -1) //-1 means no node is found
			return false;

		return DeleteNodeAtIndex(nodeIndex);
	}

	//------------------------------------------------------------------------------------------------
	int FindNodeIndex(BTC_Node node)
	{
		if (node == m_Head)
			return 0;
		if (node == m_Tail)
			return m_aNodes.Count() - 1;

		BTC_Node temp = m_Head;
		for (int i = 0; i < Count() - 1; i++)
		{
			temp = temp.GetNext();
			if (temp == node)
				return i;
		}

		return -1;
	}

	//------------------------------------------------------------------------------------------------
	void CopyNodes(out array<BTC_Node> outList)
	{
		foreach (BTC_Node node : m_aNodes)
		{
			BTC_Node clone = BTC_Node.Cast(node.Clone());
			if (clone)
				outList.Insert(clone);
		}
	}

	//------------------------------------------------------------------------------------------------
	BTC_Node Get(int index)
	{
		BTC_Node temp = m_Head;
		for (int i = 0; i < index - 1; i++)
		{
			temp = temp.GetNext();
		}

		return temp;
	}

	//------------------------------------------------------------------------------------------------
	int Count()
	{
		return m_aNodes.Count();
	}

	//------------------------------------------------------------------------------------------------
	bool IsEmpty()
	{
		return m_aNodes.IsEmpty();
	}

	//------------------------------------------------------------------------------------------------
	//! Returns a reference to the first element of the queue.
	BTC_Node Begin()
	{
		return m_Head;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns a reference to the last element of the queue.
	BTC_Node End()
	{
		return m_Tail;
	}
}

class BTC_CircularDoubleLinkedList : BTC_DoubleLinkedList
{
	//------------------------------------------------------------------------------------------------
	//! Inserts value at the end of the list.
	override int InsertAtEnd(BTC_Node node)
	{
		if (!m_Head)
		{
			m_Head = node;
			m_Tail = node;
		}
		else
		{
			m_Tail.SetNext(node);
			node.SetPrevious(m_Tail);
			node.SetNext(m_Head);
			m_Head.SetPrevious(node);

			m_Tail = node;
		}

		return m_aNodes.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Inserts value at the start of the list.
	override int InsertAtBeginning(BTC_Node node)
	{
		if (!m_Head)
		{
			m_Head = node;
			m_Tail = node;
		}
		else
		{
			m_Head.SetPrevious(node);
			node.SetNext(m_Head);
			node.SetPrevious(m_Tail);
			m_Tail.SetNext(node);

			m_Head = node;
		}

		return m_aNodes.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Deletes the starting node of the list.
	override void DeleteFirstNode()
	{
		if (!m_aNodes.IsEmpty())
			return;

		BTC_Node nodeAfterFirst = m_Head.GetNext();

		nodeAfterFirst.SetPrevious(m_Tail);
		m_Tail.SetNext(nodeAfterFirst);

		m_aNodes.RemoveOrdered(0);
	}

	//------------------------------------------------------------------------------------------------
	override void DeleteLastNode()
	{
		if (!m_aNodes.IsEmpty())
			return;

		BTC_Node nodeBeforeLast = m_Tail.GetPrevious();

		nodeBeforeLast.SetNext(m_Head);
		m_Head.SetPrevious(nodeBeforeLast);

		m_aNodes.RemoveOrdered(m_aNodes.Count() - 1);
	}
}
