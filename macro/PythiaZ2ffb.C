// *****************************************
// Initialize Pythia Parameter 
//   for e+e- -> Z -> f + fbar
//(Description)
//  This macro is to initialize Pythia event generator so as to 
//  generate e+ e- -> Z -> f fbar, while Z to f fbar decay is
//  restriced to one channel for help debugging vertexing algorithm.
//  Allowed decay mode is selected by one global variable, fIDC.
//  Which number corresponds to which decay mode can be found in the
//  following comment.
// *****************************************

void InitPythia()
{

  TPythia *tpy=py->GetPythia();

  //  IDC code of Z decay  ( From the list obtained by LuList(12) )
  //  IDC=156   d d~ ,   157   u u~ ,   158   s s~
  //      159   c c~ ,   160   b b~ ,   161   t t~
  //      162   l l~ ,   163   h h~   ( These two channel is suppressed as a default )
  //      164   e-e+ ,   165   nu_e ne_e~
  //      166   mu-mu+,  167   nu_mu,nu_mu~
  //      168   tau-tau+, 169  nu_tau,nu_tau~
  //      170   chi-chi+, 171  nu_chi, nu_chi~ 

  for(Int_t idc=156;idc<=171;idc++)   tpy->SetMDME(idc,1,0);
    
  tpy->SetMDME(fIDC,1,1) ;  // Allow only pne decay chanel

  Char_t *dmode[16];
  dmode[0]="d + dbar" ; dmode[1]="u + ubar" ; dmode[2]="s + sbar" ;
  dmode[3]="c + cbar" ; dmode[4]="b + bbar" ; dmode[5]="t + tbar" ;
  dmode[6]="L + Lbar" ; dmode[7]="H + Hbar";
  dmode[8]="e- + e+ " ; dmode[9]="nu_e + nu_e bar";
  dmode[10]="mu- + mu+ " ; dmode[11]="nu_mu + nu_mu bar";
  dmode[12]="tau- + tau+ " ; dmode[13]="nu_tau + nu_tau bar";
  dmode[14]="Chi + Chi bar" ; dmode[15]="nu_CHI + nu_CHI bar";

  printf("Pythia will be initialized to generate e+ e- -> Z event.\n");
  printf("where Z is allowed to decay only to %s.\n",dmode[fIDC-156]);

}
