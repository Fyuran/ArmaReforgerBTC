class BTC_AIGetLookAtActivityParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new BTC_AILookAtActivity(null, null, vector.Zero)).GetPortNames();
	override TStringArray GetVariablesOut() { return s_aVarsOut; }

	protected override bool VisibleInPalette() { return true; }
}

class BTC_AIGetLookAtBehaviorParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new BTC_AILookAtBehavior(null, null, null, vector.Zero)).GetPortNames();
	override TStringArray GetVariablesOut() { return s_aVarsOut; }

	protected override bool VisibleInPalette() { return true; }
}

class BTC_AIGetVehicleDefendActivityParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new BTC_AIVehicleDefendActivity(null, null, vector.Zero)).GetPortNames();
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	override bool VisibleInPalette() { return true; }
}

class BTC_AIGetVehicleDefendBehaviorParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new BTC_AIVehicleDefendBehavior(null, null, null, vector.Zero, 0)).GetPortNames();
	override TStringArray GetVariablesOut() { return s_aVarsOut; }
	
	override bool VisibleInPalette() { return true; }
}
