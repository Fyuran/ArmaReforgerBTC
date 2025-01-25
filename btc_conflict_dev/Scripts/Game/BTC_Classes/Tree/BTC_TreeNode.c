void OnNodeInserted(int key, BTC_TreeNode node);
typedef func OnNodeInserted;
typedef ScriptInvokerBase<OnNodeInserted> ScriptInvokerOnNodeInserted;

void OnNodeRemoved(int key);
typedef func OnNodeRemoved;
typedef ScriptInvokerBase<OnNodeRemoved> ScriptInvokerOnNodeRemoved;

class BTC_TreeNode
{
	protected static int s_iID = -1;
	
	protected int m_iLevel = 0;
	protected int m_iID = 0;
	protected BTC_TreeNode m_Parent;
	protected int m_iParentID = -1;
	protected ref array<ref BTC_TreeNode> m_aChildren = {};
	protected ref ScriptInvokerOnNodeInserted m_OnNodeInserted;
	protected ref ScriptInvokerOnNodeRemoved m_OnNodeRemoved;

	//------------------------------------------------------------------------------------------------
	ScriptInvokerOnNodeInserted GetOnNodeInserted()
	{
		if (!m_OnNodeInserted)
			m_OnNodeInserted = new ScriptInvokerOnNodeInserted();

		return m_OnNodeInserted;
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvokerOnNodeRemoved GetOnNodeRemoved()
	{
		if (!m_OnNodeRemoved)
			m_OnNodeRemoved = new ScriptInvokerOnNodeRemoved();

		return m_OnNodeRemoved;
	}

	//------------------------------------------------------------------------------------------------
	static void ResetIDCounter()
	{
		s_iID = -1;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonSaveContext
	bool SerializationSave(BaseSerializationSaveContext context)
	{
		if (!context.IsValid())
			return false;
		
		context.WriteValue("m_iID", m_iID);
		context.WriteValue("m_iParentID", m_iParentID);
		context.WriteValue("m_aChildren", m_aChildren);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Method used by SCR_JsonLoadContext
	bool SerializationLoad(BaseSerializationLoadContext context)
	{
		if (!context.IsValid())
			return false;

		context.ReadValue("m_iID", m_iID);
		context.ReadValue("m_iParentID", m_iParentID);
		context.ReadValue("m_aChildren", m_aChildren);
		SerializationLoadChildren();
		
		if(m_iID > s_iID) //raise class' static int counter to the highest recorded
			s_iID = m_iID;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void SerializationLoadChildren()
	{
		foreach(BTC_TreeNode child : m_aChildren)
		{
			child.m_Parent = this;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Adds a child and increments the child's level by (current + 1)
	bool InsertChild(BTC_TreeNode node)
	{
		if (!m_aChildren)
			return false;

		node.m_Parent = this;
		node.m_iLevel = this.m_iLevel + 1;
		node.m_iParentID = this.m_iID;

		if (m_OnNodeInserted)
		{
			m_OnNodeInserted.Invoke(node.m_iID, node);
			node.m_OnNodeInserted = m_OnNodeInserted; //pass the invoker down the hierarchy
		}
		if (m_OnNodeRemoved)
			node.m_OnNodeRemoved = m_OnNodeRemoved; //pass the invoker down the hierarchy
		
		return this.m_aChildren.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	bool RemoveChild(BTC_TreeNode node)
	{
		if (m_aChildren.IsEmpty()) //avoid lookup when no element is present
			return false;

		if (m_aChildren.Find(node) == -1)
			return false;

		if (m_OnNodeRemoved)
			m_OnNodeRemoved.Invoke(node.m_iID);

		return m_aChildren.RemoveItemOrdered(node);
	}

	//------------------------------------------------------------------------------------------------
	bool RemoveSibling(BTC_TreeNode node)
	{
		array<ref BTC_TreeNode> temp = m_Parent.m_aChildren;
		if (temp.IsEmpty()) //avoid lookup when no element is present
			return false;

		if (temp.Find(node) == -1)
			return false;

		if (m_OnNodeRemoved)
			m_OnNodeRemoved.Invoke(m_iLevel, node);

		return temp.RemoveItemOrdered(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Changes current level's parent
	void SetParent(BTC_TreeNode node)
	{
		m_Parent = node;
		m_iParentID = node.m_iID;
	}

	//------------------------------------------------------------------------------------------------
	//! appends a new sibling to the current level
	bool InsertSibling(BTC_TreeNode node)
	{
		if (!m_Parent) //do not allow root level siblings
			return false;

		node.m_Parent = this.m_Parent;
		node.m_iLevel = this.m_iLevel;
		node.m_iParentID = this.m_iParentID;

		if (m_OnNodeInserted)
		{
			m_OnNodeInserted.Invoke(m_iLevel, node);
			node.m_OnNodeInserted = m_OnNodeInserted; //pass the invoker to the new sibling
		}

		return this.m_Parent.m_aChildren.Insert(node);
	}

	//------------------------------------------------------------------------------------------------
	//! Copies this node to another node
	void CopyTreeNode(out BTC_TreeNode outNode)
	{
		outNode.m_iID = this.m_iID;
		outNode.m_iLevel = this.m_iLevel;
		outNode.m_Parent = this.m_Parent;
		outNode.m_iParentID = this.m_iParentID;
		outNode.m_aChildren = this.m_aChildren;
		
		if(m_OnNodeInserted)
			outNode.m_OnNodeInserted = this.m_OnNodeInserted;
		if(m_OnNodeRemoved)
			outNode.m_OnNodeRemoved = this.m_OnNodeRemoved;
			
	}	
	
	//------------------------------------------------------------------------------------------------
	//! Gets all children of current node
	array<ref BTC_TreeNode> GetChildren()
	{
		return m_aChildren;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets all siblings at current node level
	array<ref BTC_TreeNode> GetSiblings()
	{
		return this.m_Parent.GetChildren();
	}

	//------------------------------------------------------------------------------------------------
	//! Returns at which tree level this node belongs to
	int GetLevel()
	{
		return m_iLevel;
	}

	//------------------------------------------------------------------------------------------------
	int GetID()
	{
		return m_iID;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetParentID()
	{
		return m_iParentID;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Traverses the hierarchy backwards to reach root node
	BTC_TreeNode GetRoot()
	{
		BTC_TreeNode root = this;
		while (root)
		{
			if (root.GetParent()) //check if traversing node's parent exists
				root = root.GetParent();
			else
				return root;
		}

		return null; //root variable became null during while loop
	}

	//------------------------------------------------------------------------------------------------
	BTC_TreeNode GetParent()
	{
		return m_Parent;
	}
	
	//------------------------------------------------------------------------------------------------
	void BTC_TreeNode()
	{
		s_iID += 1;
		m_iID = s_iID;
	}
}