// *****************************************
// Initialize Pythia Parameter 
// Type of event are controled by environment parameters.
// Note that CM energy is defined in InitGenSim function defined
// by GUIMainMacro.C, using a value defined by JSFGUI.ECM parameter.
//$Id$
// *****************************************


void InitHerwig()
{

  HerwigGenerator *hwg=(HerwigGenerator*)jsf->FindModule("HerwigGenerator");
  THerwig *hw=hwg->GetHerwig();

  Double_t hmass;
  sscanf(jsf->Env()->GetValue("HerwigGenerator.HiggsMass","120.0"),"%lg",&hmass);
  hw->SetRMASS(hmass, 201);
  cout << "HerwigGenerator.HiggsMass=" << hw->GetRMASS(201) << endl ;

} 
