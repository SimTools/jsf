// This macro defines a User menu, which appears in JSF Control Panel.
// There are two environment parameters related to User Menu.  Those are,
//   JSFGUI.UserMenu [ 0 ]
//   JSFGUI.UserMenuMacro [UserMenuMacro.C]
// Values in [] are default value. If JSFGUI.UserMenu is 0 or No, 
// user-defined menu is not drawn.  If JSFGUI.UserMenu is 1 or Yes,
// it is included in the menu bar of JSF Control Pannel.
// User menu is managed by global functions,
//   void UserMenu(TGPopupMenu*)
//   void UserMenuUpdate(TGPopupMenu*)
//   void UserMenuDelete(TGPopupMenu*)
//   void UserMenuProcessMessage(TGPopupMenu*, UInt_t)
// They are defined in a file, UserMenuMacro.C.  Their examples are attached
// below.
//   
//$id$


enum EUserMenuID {
  M_USER_COSTH=1001,
  M_USER_PHI=1002, 
  M_USER_ECM=1003,
  M_USER_PID=1004,
  M_USER_PRINFO=1005,
  M_USER_PRHIST=1006
};

//**************************************************************************
void UserMenu(TGPopupMenu *fMenu)
{
  // Define user menu.  This function is called by the constructor of JSF 
  // control pannel.  fMenu is a poiner to the user menu object.
  //
  fMenu->AddLabel("User Menu");
  fMenu->AddSeparator();
  fMenu->AddEntry("Range of Costh",M_USER_COSTH);
  fMenu->AddEntry("Range of Phi",M_USER_PHI);
  fMenu->AddEntry("ECM (GeV)",M_USER_ECM);
  fMenu->AddEntry("Particle ID",M_USER_PID);
  fMenu->AddEntry("Print Info.",M_USER_PRINFO);
  fMenu->AddEntry("Print Hist",M_USER_PRHIST);

}

void UserMenuUpdate(TGPopupMenu *fMenu)
{
  // Set check mark, so on, when menu bar is checked or selected.
  // This is called to re-create menu values according to the 
  // click action.

  fMenu->UnCheckEntry(M_USER_PRINFO);
  fMenu->UnCheckEntry(M_USER_PRHIST);

  if( jsf->Env()->GetValue("FFbarBases.PrintInfo",0) ) 
    fMenu->CheckEntry(M_USER_PRINFO);

  if( jsf->Env()->GetValue("FFbarBases.PrintHist",0) ) 
    fMenu->CheckEntry(M_USER_PRHIST);

}

void UserMenuDelete()
{
  // Called prior to the construction of JSF Control Panel.

}

void UserMenuProcessMessage(TGPopupMenu *fMenu, Long_t parm)
{
  // Prcoess menu-select action.  To avoid confusion of menu ID used by
  // JSFGUIFrame class, menu ID for user defined menus should be greater
  // than 1000.

  InputDialog *dialog;
  Char_t wrkstr[256];
  Int_t iflg;
  switch(parm) {
    case M_USER_COSTH:
      strcpy(wrkstr,jsf->Env()->GetValue("FFbarBases.CosthRange","-0.7 0.7"));
      dialog = new InputDialog("FFbarBases.CosthRange:
Minimum and maximum of fermion production
angle (costh)",wrkstr,wrkstr,3);
      if(wrkstr[0]!=0) jsf->Env()->SetValue("FFbarBases.CosthRange",wrkstr);
      break;

    case M_USER_PHI:
      strcpy(wrkstr,jsf->Env()->GetValue("FFbarBases.PhiOverPiRange","0.0 2.0"));
      dialog = new InputDialog("FFbarBases.PhiOverPiRange:
Minimum and maximum of azimuthal angle of 
fermion production angle (phi) in unit
of radian.",wrkstr,wrkstr,4);
      if(wrkstr[0]!=0) jsf->Env()->SetValue("FFbarBases.PhiOverPiRange",wrkstr);
      break;

    case M_USER_ECM:
      strcpy(wrkstr,jsf->Env()->GetValue("FFbarBases.ECM","300.0"));
      dialog = new InputDialog("FFbarBases.ECM:
Center of mass energy (GeV).",wrkstr,wrkstr,2);
      if(wrkstr[0]!=0) jsf->Env()->SetValue("FFbarBases.ECM",wrkstr);
      break;

    case M_USER_PID:
      strcpy(wrkstr,jsf->Env()->GetValue("FFbarBases.ID","13"));
      dialog = new InputDialog("FFbarBases.ID:
Particle ID of generated particle.",wrkstr,wrkstr,2);
      if(wrkstr[0]!=0) jsf->Env()->SetValue("FFbarBases.ID",wrkstr);
      break;

    case M_USER_PRINFO:
      iflg=jsf->Env()->GetValue("FFbarBases.PrintInfo",0);
      if( iflg ) jsf->Env()->SetValue("FFbarBases.PrintInfo","0");
      else  jsf->Env()->SetValue("FFbarBases.PrintInfo","1");
      break;

    case M_USER_PRHIST:
      iflg=jsf->Env()->GetValue("FFbarBases.PrintHist",0);
      if( iflg ) jsf->Env()->SetValue("FFbarBases.PrintHist","0");
      else  jsf->Env()->SetValue("FFbarBases.PrintHist","1");
      break;
    default:
      break;
  }
  UserMenuUpdate(fMenu);
}

