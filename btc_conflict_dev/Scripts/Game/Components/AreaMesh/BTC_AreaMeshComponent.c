[ComponentEditorProps(category: "GameScripted/Area Mesh", description: "")]
class BTC_BaseAreaMeshComponentClass : SCR_BaseAreaMeshComponentClass
{
}

class BTC_BaseAreaMeshComponent : SCR_BaseAreaMeshComponent
{
	[Attribute("1")]
	protected float m_fRadius;

	//------------------------------------------------------------------------------------------------
	override float GetRadius()
	{
		return m_fRadius;
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		// This should only help with radius setup, do not show in play mode.
		if (GetGame().InPlayMode())
			return;

		GenerateAreaMesh();
	}
}
