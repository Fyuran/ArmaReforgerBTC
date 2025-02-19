class BTC_AISetVehicleDefendActivityParameters : SCR_AISetActionParameters
{
	protected static ref TStringArray s_aVarsIn = (new BTC_AIVehicleDefendActivity(null, null, vector.Zero)).GetPortNames();	
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	override bool VisibleInPalette() { return true; }
}
