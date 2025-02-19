class BTC_Utilities
{
	//------------------------------------------------------------------------------------------------
	//! Do not use this on Behavior Trees Resource, workbench will CTD without warning
	static bool IsResourceNameValid(ResourceName name)
	{
		Resource res = Resource.Load(name);
		bool isValid = res.IsValid();
		if (!isValid)
			Print(res.ToString() + " is not a valid ResourceName", LogLevel.ERROR);

		return isValid;
	}
}
