// *****************************************
// Initialize Herwig Parameter 
// Type of event are controled by environment parameters.
// Note that CM energy is defined in InitGenSim function defined
// by GUIMainMacro.C, using a value defined by JSFGUI.ECM parameter.
//$Id$
// *****************************************


void InitHerwig()
{
  // Note that this function is called during the THerwig::Initialize
  // .i.e, after beam parameters and processes are set.
  // To set additional parameter of Herwig

  HerwigGenerator *hwg=(HerwigGenerator*)jsf->FindModule("HerwigGenerator");
  THerwig *hw=hwg->GetHerwig();

  Double_t hmass;
  sscanf(jsf->Env()->GetValue("HerwigGenerator.HiggsMass","120.0"),"%lg",&hmass);
  hw->SetRMASS(hmass, 201);
  cout << "HerwigGenerator.HiggsMass=" << hw->GetRMASS(201) << endl ;

} 

