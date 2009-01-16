//*LastUpdate :  jsf-1-11  27-July-1999  By A.Miyamoto
//*-- Author  : A.Miyamoto  27-Julyr-1998

////////////////////////////////////////////////////////////////////////
//
// JSFReadPaarton
//
// Read Parton data from a file, and store them into JSFSpringParton class
// for subsequent hadronization by JSFHadronize.
// Here parton means quarks, leptons and EW gauge bosons.
// Input data could be Fortran binary file or ASC text file.
// In case of a Fortran binary file, format of each event data is as follows:
//   
// INTEGER*4 NENDIAN
// INTEGER*4 NVERS
// INTEGER*4 NEVENT
// INTEGER*4 NPART
// INTEGER*4 IDAT(7,NPART)
// REAL*4    RDAT(6,NPART)
//
//   IU=1
//   NENDIAN=1296651082
//   NVERS  =101
//   WRITE(IU) NENDIAN, NVERS, NEVENT, NPART,
//  .          ((IDAT(I,J),I=1,7),J=1,NPART),
//  .          ((RDAT(I,J),I=1,6),J=1,NPART)
//
//   where
//    NEVENT : Event number
//    NPART  : Number of partons
//    IDAT(1,i) : Particle ID a la PDG of i-th parton
//        (2,i) : Number of child particles
//        (3,i) : Serial number of 1st child particle. 
//        (4,i) : Serial number of parent particle.
//        (5,i) : helicity of parton.
//        (6,i) : Color singlet group ID.
//        (7,i) : Shower infomation ( 100*Shower level + Shower pair ID)
//      ( Serial number is a number from 1 to NPART )
//    RDAT(1,i) : Mass of i-th parton (GeV)
//        (2,i) : Charge
//        (3,i) : Px (GeV)
//        (4,i) : Py (GeV)
//        (5,i) : Pz (GeV)
//        (6,i) : E(GeV)  
//
//   If RDAT(6,i) is negative, it is calculated by this class using mass and P.
//
//   For more information about IDAT, please see
//   begin_html <a href="http://www-jlc.kek.jp/subg/offl/lib/docs/luhadr/node8.html">How to prepare Psring:Parton_List</a> end_html.
// 
//   In case of an ASC File, input data must be prepared so that they can
//   be read by following fortran statements.
//
//     READ(IU,*,END=300) NVERS, NEVENT, NPART
//     DO 200 J = 1, NPART
//       READ(IU,*) (IDAT(I,J),I=1,7),(RDAT(K,J),K=1,6)
// 200 CONTINUE
//
// 300 continue  ! end of data  
//
//   Comment lines can be inserted anywhere.  It is identified by a character "#"
//   at first column.
// 
//(11-Feb-2006)
// Input data format for this class was extended to read GRACE data 
// prepared by Yoshimasa Kurihara san.  In this format, each line contains
//   px, py, pz, e, mass
// first two line is initial electron and positron information.
// rest lines are information of final particles.
// This format is very simple when using, twophpton eeee event for example
// Sample data of one event is shown below.
//   0.000000E+00   0.000000E+00   250.0000       250.0000       5.109991E-04
//   0.000000E+00   0.000000E+00  -250.0000       250.0000       5.109991E-04
//  -1.934790      -14.43530       41.80130       44.26590       5.109991E-04
//  -8.556629      -64.47403       186.4988       197.5144       5.109991E-04
//   5.336050       81.13042      -235.4707       249.1126      0.1056584    
//   5.155369      -2.221091       7.170572       9.107104      0.1056584    
// JSFEnv parameters related to this format are,
//   JSFReadParton.Format: 3  # 3 means simple Grace format
//   JSFReadParton.NParticles: 6 # Number of particles per event.
//   JSFReadParton.PID:  11 -11 11 -11 13 -13 
//   JSFReadParton.Charge: -1.0 1.0 -1.0 1.0 -1.0 1.0
//
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include <TObjectTable.h>
#include <TKey.h>
#include "TRandom.h"
#include "JSFSteer.h"
#include "JSFSpring.h"
#include "JSFSpringParton.h"
#include "JSFReadParton.h"
#include "TObjString.h"
#include <map>
#include <iomanip>

extern "C" {
extern void readpartonopen_(Int_t *unit, Int_t *type, Char_t *name, Int_t len);
extern void readpartonclose_(Int_t *unit);
extern void readparton_(Int_t *iunit, Int_t *type, 
	Int_t *endian, Int_t *nvers, Int_t *nevent, Int_t *npart,
        Int_t idat[][7], Float_t rdat[][6], Int_t *nret);
};

ClassImp(JSFReadPartonBuf)
ClassImp(JSFReadParton)

  ifstream JSFReadParton::fInStream;

//_____________________________________________________________________________
JSFReadParton::JSFReadParton(const char *name, const char *title)
  : JSFSpring(name, title, NULL)
{
  fEventBuf = new JSFReadPartonBuf("JSFReadPartonBuf", 
	 "JSFReadParton event buffer", this);
 
  sscanf(gJSF->Env()->GetValue("JSFReadParton.DataFile","parton.dat"),
	 "%s",fDataFileName);
  sscanf(gJSF->Env()->GetValue("JSFReadParton.Unit","10"),"%d",&fUnit);
  fType=gJSF->Env()->GetValue("JSFReadParton.Format",1);

  fNPGen=gJSF->Env()->GetValue("JSFReadParton.NParticles",6);
  fKYPID=gJSF->Env()->GetIValue("JSFReadParton.PID","11 -11 11 -11 13  -13",fNPGen);
  fKYCharge=gJSF->Env()->GetDValue("JSFReadParton.Charge","-1.0 1.0 -1.0 1.0 -1.0 1.0",fNPGen);

}


//_____________________________________________________________________________
JSFReadParton::~JSFReadParton()
{
  if( fEventBuf ) { delete fEventBuf; fEventBuf=NULL ; }
}

//_____________________________________________________________________________
Bool_t JSFReadPartonBuf::SetPartons()
{

  //  Read one record from a file and save it in a JSFSpringParton class.

  TClonesArray &partons = *fPartons;

  //  Int_t endian=1296651082 ; 
  Int_t nvers=101;
  Int_t nevent, npart, ivers, iendian;
  static const Int_t MAXPRT=50;
  Int_t idat[MAXPRT][7];
  Float_t rdat[MAXPRT][6];
  Int_t nret;
  Int_t unit=((JSFReadParton*)Module())->GetUnit();
  Int_t type=((JSFReadParton*)Module())->GetType();

  if( type == 1 || type == 2 ) {  
    readparton_(&unit, &type, &iendian, &ivers, &nevent, &npart,
	      idat, rdat, &nret);
    fNparton=npart;

    // *************** Check return status
    if(nret == -1) { 
      printf("In JSFReadPartonBuf::SetParton .. ");
      printf("read end-of-file of the input file.\n");
      return kFALSE;
    }
    else if( nret == -2 ) {
      printf("In JSFReadPartonBuf::SetParton .. ");
      printf("read error detected.\n");
      return kFALSE;
    }
    else if( nvers != ivers ) {
      printf("Errror at JSFReadPartonBuf.. Version number of ");
      printf("data is %d, while program can process only ",nvers);
      printf("version %d\n",ivers);
      return kFALSE;
    }
  }
  else if ( type==4) { 
    // Les Houches format .lhe
    // written by daniel jeans 7 feb 08
    // warning: you need to decay top quarks to at least W+b
    // no need to decay higgs, W, Z
    JSFReadParton *rdp=(JSFReadParton*)Module();
    ifstream *fin=rdp->GetInStream();
    string line;
    bool inevent = false;
    int npart = 0;
    if ( fin->good() && !fin->eof() ) {
      while (!fin->eof()) {
	getline(*fin, line);
	TString lline(line);
	if (lline.Contains("<event>")) { // start of new event
	  inevent = true;
	  getline(*fin, line); // not used info line
	  npart = 0;
	} else if (lline.Contains("</LesHouchesEvents>")) {
	  cout << "reached end of .lhe file!" << endl;
	  return kFALSE;
	} else if (lline.Contains("</event>")) {
	  // end of event
	  // do some end-of-event stuff (color flow)
	  // first find color singlets

	  //cout << "before: " << endl;
	  //for (int i=0; i<npart; i++) {
	  //  cout << std::setw(4) << i << " " << std::setw(4) << i+1 << " , ";
	  //  for (int j=0; j<7; j++) cout << std::setw(4) << idat[i][j] << " ";
	  //  cout << endl;
	  //}

	  int maxcol = 0;
          for (int i=0; i<npart; i++) {
	    if (idat[i][5]>maxcol) maxcol=idat[i][5];
	  }
	  maxcol++;

	  int toplevel = 9;
	  int middlelevel = 5;
	  int bottomlevel = 1;
	  int ilevel[11];
	  for (int i=0; i<11; i++) ilevel[i]=1;
	  int nextra = 0;

	  std::vector <int> tops; tops.clear();
	  

	  // first look for top quarks, they need special treatment
          for (int i=0; i<npart; i++) {
	    if (TMath::Abs(idat[i][0])==6) {
	      //cout << "got top " << i << endl;

	      tops.push_back(i);

	      // adjust it's color info, should be different to its b decay products
	      idat[i][5] -= 99;

	      // find it's daughters
	      int dau_b = -1;
	      int dau_w = -1;
	      int dau_wd[2] = {-1, -1};
	      int id=0;
	      for (int j=0; j<npart+nextra; j++) {
		if (idat[j][3]==i+1) {
		  if (TMath::Abs(idat[j][0])==5 ) dau_b=j;
		  else if (TMath::Abs(idat[j][0])==24) dau_w=j;
		  else if (id<2) dau_wd[id++] = j;
		} else if ( idat[ idat[j][3]-1 ][3] == i+1 ) { // grandchild of top, child of w
		  if (id<2) dau_wd[id++] = j;
		  else cout << "too many w grandchildren found!" << endl;
		}
	      }
	      if (id==1) cout << "ERROR only one W daughter found in top decay!" << endl;
	      bool Whad = dau_wd[0]>=0 && TMath::Abs(idat[dau_wd[0]][0])<6;
	      //cout << "found top " << i << " " << dau_b << " " << dau_w << " " << 
	      //	dau_wd[0] << " " << dau_wd[1] << " " << Whad << endl;
	      if (dau_w<0 && dau_wd[0]<0 && dau_wd[1]<0) {
		cout << "ERROR could not find W and/or W decay products from t decay" << endl;
		cout << "you must decay t(bar) before JSFHadronizer" << endl;
		continue;
	      }
	      if (dau_b<0) {
		cout << "ERROR could not find b from top decay!" << endl;
		cout << "you must decay t(bar) before JSFHadronizer" << endl;
		continue;
	      }
	      if (dau_wd[0]>=0 && !Whad) {
		idat[dau_wd[0]][5] = maxcol;
		idat[dau_wd[1]][5] = maxcol;
		maxcol++;
	      }

	      tops.push_back(dau_b);
	      tops.push_back(dau_w);
	      tops.push_back(dau_wd[0]);
	      tops.push_back(dau_wd[1]);

	      // we need two shower infos
	      // one for W and b (lower index), other one (high) for daughters of W (if hadronic)
	      //int shower_Wb = bottomlevel++*100 + 1;

              //int shower_Wb = 5*100 + ilevel[5]++;

	      int shower_Wb=-1;

	      // is there already a top with same colour?
	      // if yes, put into same showering level
	      // if not, put in new level
	      int ntop = tops.size()/5;
	      if (ntop>1) {
		for (int kk=0; kk<ntop-1; kk++) {
		  if (idat[tops[5*kk]][5]==idat[i][5]) {
		    int level = idat[tops[5*kk+1]][6]/100; // this is the b (from top) info
		    shower_Wb = level*100 + ilevel[level];
		  }
		}
	      }

	      if (shower_Wb<0) {
		shower_Wb = middlelevel*100 + 1;
		ilevel[middlelevel]++;
		middlelevel++;
	      }

	      int shower_Wdec = Whad ? toplevel--*100 + 1 : 0;

	      //cout << "shower info: Wb, Wdec = " << shower_Wb << " " << shower_Wdec << endl;

	      idat[dau_b][6] = shower_Wb;

	      if (dau_w>=0) idat[dau_w][6] = shower_Wb;

	      for (int iij=0; iij<2; iij++) {
		if (dau_wd[iij]>=0) idat[dau_wd[iij]][6] = shower_Wdec;
	      }

	      if (dau_w>=0 && dau_wd[0]<0) { // W present, but not it's decay daughters
		if (dau_wd[1]>=0) cout << "ERROR, only one W decay product found!!!" << endl;
		idat[dau_w][5] = maxcol++;
	      } else if (dau_w>=0 && dau_wd[0]>=0) {
		if (dau_wd[1]<0) cout << "ERROR, only one W decay product found!!!" << endl;
                idat[dau_w][5] = idat[dau_wd[0]][5]; // color singlet group: same as daughters
	      } else {	// if no W listed, make one from daughters
		if (dau_wd[0]<0 || dau_wd[1]<0) {
		  cout << "ERROR found less than 2 W decay products! " << dau_wd[0] << " " << dau_wd[1] << endl;
		  continue;
		}

		int totchg = 0;
		for (int hj=0; hj<2; hj++) {
		  int id = TMath::Abs(idat[dau_wd[hj]][0]);
		  int chg = int(rdat[dau_wd[hj]][1]);
		  if      (id==1  || id==3  || id==5 ) totchg+= 1*chg;
		  else if (id==2  || id==4)            totchg+= 2*chg;
		  else if (id==11 || id==13 || id==15) totchg+= 3*chg;
		  else if (id==12 || id==14 || id==16) totchg+= 3*chg; // always zero i hope!
		  else cout << "strange w decay particle! id, chg = " << id << " "<< chg << endl;
		}
		totchg/=3;

		if (TMath::Abs(totchg)!=1) {
		  cout << "ERROR w decay charge = " << totchg << " : ";
		  cout << rdat[dau_wd[0]][1] << " " <<  rdat[dau_wd[1]][1] << endl;
		}		


		idat[npart+nextra][0] = totchg*24;
		idat[npart+nextra][1] = 2;
		idat[npart+nextra][2] = TMath::Min(dau_wd[0], dau_wd[1]) + 1;
		idat[npart+nextra][3] = i + 1;              // parent: top quark index
		idat[npart+nextra][4] = 0;                  // helicity not important
		idat[npart+nextra][5] = idat[dau_wd[0]][5]; // color singlet group: same as daughters
		idat[npart+nextra][6] = shower_Wb;     //shower info: same as b
		
		rdat[npart+nextra][1] = totchg;
		rdat[npart+nextra][2] = rdat[dau_wd[0]][2] + rdat[dau_wd[1]][2];
		rdat[npart+nextra][3] = rdat[dau_wd[0]][3] + rdat[dau_wd[1]][3];
		rdat[npart+nextra][4] = rdat[dau_wd[0]][4] + rdat[dau_wd[1]][4];
		rdat[npart+nextra][5] = rdat[dau_wd[0]][5] + rdat[dau_wd[1]][5];
		rdat[npart+nextra][0] = TMath::Sqrt( rdat[npart+nextra][5]*rdat[npart+nextra][5] 
						     - rdat[npart+nextra][2]*rdat[npart+nextra][2] 
						     - rdat[npart+nextra][3]*rdat[npart+nextra][3] 
						     - rdat[npart+nextra][4]*rdat[npart+nextra][4]);

		// need to adjust daughters and parent of new W
		for (int jj=0; jj<2; jj++) idat[dau_wd[jj]][3] = npart+nextra+1;
		idat[i][1]--;

		// put W in proper position, shuffle others around

		// temp saving position
		for (int ij=0; ij<7; ij++) idat[npart+nextra+1][ij] = idat[npart+nextra][ij];
		for (int ij=0; ij<6; ij++) rdat[npart+nextra+1][ij] = rdat[npart+nextra][ij];

		// what index should i put it at?
		int parentindex        = idat[npart+nextra][3];
		int firstDaughterindex = idat[parentindex-1][2];
		int ndaughter          = idat[parentindex-1][1];


		//cout << parentindex << " " << firstDaughterindex << " " << ndaughter << endl;

		int insertPos = -1;
		int movePos = -1;

		for (int ij=0; ij<ndaughter; ij++) {

		  //cout << ij << " " << firstDaughterindex-1+ij << " " << idat[firstDaughterindex-1+ij][3] << endl;

		  if (idat[firstDaughterindex-1+ij][3]!=parentindex) {
		    insertPos = firstDaughterindex-1+ij;
		    movePos = firstDaughterindex-1+ndaughter;
		    break;
		  }
		}

		//cout << "insert, move pos = " << insertPos << " " << movePos << endl;

		// shift all down
		for (int ij=npart+nextra; ij>movePos; ij--) {
		  for (int k=0; k<7; k++) idat[ij][k] = idat[ij-1][k];
		  for (int k=0; k<6; k++) rdat[ij][k] = rdat[ij-1][k];
		}
		// move the one which would get overwritten
		for (int k=0; k<7; k++) idat[movePos][k] = idat[insertPos][k];
		for (int k=0; k<6; k++) rdat[movePos][k] = rdat[insertPos][k];
		// and write in the new W 
		for (int k=0; k<7; k++) idat[insertPos][k] = idat[npart+nextra+1][k];
		for (int k=0; k<6; k++) rdat[insertPos][k] = rdat[npart+nextra+1][k];
		// clean up the temp position
		for (int ij=0; ij<7; ij++) idat[npart+nextra+1][ij] = 0;
		for (int ij=0; ij<6; ij++) rdat[npart+nextra+1][ij] = 0;

		// shuffled 
		//cout << "suffled:" << endl;
		//for (int ii=0; ii<npart+nextra+1; ii++) {
		//  cout << std::setw(4) << ii << " " << std::setw(4) << ii+1 << " , ";
		//  for (int jj=0; jj<7; jj++) cout << std::setw(4) << idat[ii][jj] << " ";
		//  cout << endl;
		//}

		// update parent/daughter indices
		std::vector <int> indices; indices.push_back(2); indices.push_back(3);
		for (int ij=0; ij<npart+nextra+1; ij++) {
		  for (uint jjj=2; jjj<4; jjj++) {
		    if      (idat[ij][jjj]==npart+nextra+1)   idat[ij][jjj]=insertPos+1;
		    else if (idat[ij][jjj]==insertPos+1) idat[ij][jjj]=movePos+1;
		    else if (idat[ij][jjj]>movePos)      idat[ij][jjj]++;
		  }
		}

		// shuffled 
		//cout << "corrected indices:" << endl;
		//for (int i=0; i<npart+nextra+1; i++) {
		//  cout << std::setw(4) << i << " " << std::setw(4) << i+1 << " , ";
		//  for (int j=0; j<7; j++) cout << std::setw(4) << idat[i][j] << " ";
		//  cout << endl;
		//}

		nextra++;

	      }

	    }

	  } // loop looking for tops


	  //cout << "after dealing with tops" << endl;
	  //for (int i=0; i<npart+nextra; i++) {
	  //  cout << std::setw(4) << i << " " << std::setw(4) << i+1 << " , ";
	  //  for (int j=0; j<7; j++) cout << std::setw(4) << idat[i][j] << " ";
	  //  cout << endl;
	  //}





	  // now look at non-top (daughters)
          for (int i=0; i<npart+nextra; i++) {

	    //cout << "nontop " << i << " " << idat[i][0] << " " << idat[i][5] << " " << idat[i][6] << endl;

	    if (TMath::Abs(idat[i][0])>5) continue; // not quark
	    if (idat[i][5]<=0) continue; // not coloured
	    if (idat[i][6]>0) continue; // already dealt with
	    if (find(tops.begin(), tops.end(), i)<tops.end()) continue; // already dealt with top and decay prods
	    //cout << "trying to pair " << i << " " << idat[i][0] << endl;
	    bool paired = false;
	    // look for pairing with non-top
	    for (int j=i+1; j<npart; j++) {
	      if (i==j) continue;
	      if (TMath::Abs(idat[j][0])>5) continue; // not quark
	      if (idat[j][5]<=0) continue; // not coloured
	      if (idat[j][6]>0) continue; // already dealt with
	      if (find(tops.begin(), tops.end(), j)<tops.end()) continue;
	      //cout << "******* pairing " << i << " " << j << endl;
	      if (idat[i][5]==idat[j][5]) {          // color pair
		if (idat[i][3]==idat[j][3]) { // same parent		 
		  // simple case, come from same parent, put together and hadronise in isolation
		  //cout << "doing simple pair " << i << " " << j << " " << 
		  //  idat[i][3] << " " << idat[j][3] << " " << idat[i][6] << " " << idat[j][6] << endl;
		  idat[i][6]=toplevel*100 + 1;
		  idat[j][6]=toplevel*100 + 1;
		  toplevel--;
		  paired = true;
		}
	      }
	    }

	    //cout << "paired with non-top? " << paired << endl;

	    if (!paired) { // look for matching top

	      int ntop = tops.size()/5;
	      for (int it=0; it<ntop; it++) {
		int ttop = tops[5*it];
		int btop = tops[5*it + 1];
		int bcol = idat[btop][5];
		if (bcol==idat[i][5]) {

		  bool indepFrag = true;

		  if (indepFrag) {
		    // for independent fragmentation of b
		    int level = idat[btop][6]/100;
		    idat[i][6] = level*100 + ilevel[level];
		    ilevel[level]++;
		  } else {
		    //daniel can't get following approach to work...

		    // fragment with matching top quark
		    bool useOrigCol = true;
		    if (useOrigCol) idat[ttop][5] = bcol; // put top color back to orig		  
		    else            idat[i][5] = idat[ttop][5]; // top color has been adjusted...

		    idat[ttop][6] = bottomlevel*100 + ilevel[bottomlevel];
		    idat[i][6]    = bottomlevel*100 + ilevel[bottomlevel];

		    ilevel[bottomlevel]++;
		    bottomlevel++;
		  }
		  paired = true;
		}


	      }


	      //if (!pairWithTop) {
	      //	// fragment separately
	      //	//idat[i][6]=bottomlevel*100 + ilevel[bottomlevel];
	      //	//ilevel[bottomlevel]++;
	      //	// bottomlevel++;
	      //
	      //	idat[i][6] = 5*100 + ilevel[5]++;
	      //
	      //
	      //
	      //	paired = true;
	      //} else {
	      //	for (int j=0; j<npart; j++) {
	      //	  if (j==i) continue;
	      //	  if (TMath::Abs(idat[j][0])!=6) continue;
	      //	  cout << "considering top " << j << " " << endl;
	      //	  if (idat[j][5]==idat[i][5]) {
	      //	    cout << "pairing with top..." << j << endl;
	      //	    idat[i][6]=bottomlevel*100 + ilevel[bottomlevel];
	      //	    idat[j][6]=bottomlevel*100 + ilevel[bottomlevel];
	      //	    ilevel[bottomlevel]++;
	      //	    bottomlevel++;
	      //	    paired = true;
	      //	    break;
	      //	  }
	      //	}
	      //}
	    }
	    //cout << "final paired? " << paired << endl;
	  } // nontop loop



	  //cout << "after" << endl;
	  //for (int i=0; i<npart+nextra; i++) {
	  //  cout << std::setw(4) << i << " " << std::setw(4) << i+1 << " , ";
	  //  for (int j=0; j<7; j++) cout << std::setw(4) << idat[i][j] << " ";
	  //  cout << endl;
	  //}



	  // reshuffle list to put showering pairs next to each other (maybe necessary? lets try...)
	  bool changed = true;
	  while (changed) {
	    changed = false;
	    for (int i=0; i<npart+nextra-1; i++) {
	      for (int j=i+1; j<npart+nextra; j++) {

		if (idat[i][6]>0 && idat[i][6]==idat[j][6] && j-i>1) {
		  //cout << "resuffle candidate! " << i << " " << j << endl;

		  int imovefrom = j;
		  int imoveto = i+1;

		  //cout << imovefrom << " " << imoveto << endl;

		
		  int keepidat[7];
		  float keeprdat[6];
		
		  for (int jj=0; jj<7; jj++) keepidat[jj] = idat[imovefrom][jj];
		  for (int jj=0; jj<6; jj++) keeprdat[jj] = rdat[imovefrom][jj];

		  for (int k=imovefrom; k>imoveto; k--) {

		    //cout << "copying " << k-1 << " to " << k << endl;

		    for (int jj=0; jj<7; jj++) idat[k][jj] = idat[k-1][jj];
		    for (int jj=0; jj<6; jj++) rdat[k][jj] = rdat[k-1][jj];
		  }		  

		  
		  //cout << "copying " << imovefrom << " to " << imoveto << endl;

		  for (int jj=0; jj<7; jj++) idat[imoveto][jj] = keepidat[jj];
		  for (int jj=0; jj<6; jj++) rdat[imoveto][jj] = keeprdat[jj];


		  for (int jj=2; jj<4; jj++) {
		    for (int k=0; k<npart+nextra; k++) {

		      if (idat[k][jj]==imovefrom+1) idat[k][jj]=imoveto+1;
		      else if (idat[k][jj]>=imoveto+1 && idat[k][jj]<imovefrom+1) idat[k][jj]++;
		    }
		  }



		  //cout << "after temp" << endl;
		  //for (int ig=0; ig<npart+nextra; ig++) {
		  //  cout << std::setw(4) << ig << " " << std::setw(4) << ig+1 << " , ";
		  //  for (int jg=0; jg<7; jg++) cout << std::setw(4) << idat[ig][jg] << " ";
		  //  cout << endl;
		  //}



		  changed = true;
		  break;
		}
	      }
	      if (changed) break;
	    }
	  }




//
//
//
//
//	  for (int i=0; i<npart-1; i++) {
//	    for (int j=i+1; j<npart; j++) {
//
//	      cout << "******* pairing " << i << " " << j << endl;
//
//	      if (idat[i][5]>0 &&                    // colored
//		  idat[i][5]==idat[j][5] &&          // color pair
//		  TMath::Abs(idat[i][0])!=6 && TMath::Abs(idat[i][0])!=23 && TMath::Abs(idat[i][0])!=24 &&
//		  TMath::Abs(idat[j][0])!=6 && TMath::Abs(idat[j][0])!=23 && TMath::Abs(idat[j][0])!=24  ) { // not top, W, Z
//
//		if (idat[i][3]==idat[j][3]) { // same parent		 
//		  // simple case, come from same parent, put together and hadronise in isolation
//		  if (idat[i][6]==0 || idat[i][6]!=idat[j][6]) { // not already joined
//
//		    if (TMath::Abs(idat[i][0])<6 && TMath::Abs(idat[j][0])<6) {
//
//		      cout << "doing simple pair " << i << " " << j << " " << idat[i][3] << " " << idat[j][3] << " " << 
//			idat[i][6] << " " << idat[j][6] << endl;
//
//		      idat[i][6]=toplevel*100 + 1;
//		      idat[j][6]=toplevel*100 + 1;
//		      toplevel--;
//
//
//		      for (int ii=0; ii<npart+nextra; ii++) {
//		      	cout << std::setw(4) << ii << " " << std::setw(4)  << ii+1 << " , ";
//		      	for (int jj=0; jj<7; jj++) cout << std::setw(4) << idat[ii][jj] << " ";
//		      	cout << endl;
//		      }
//
//
//		    }
//		  }
//		} else { // more complicated, need to make sure we use reasonable q2 in showering
//		  // this implements the approach in 
//		  // http://www-jlc.kek.jp/subg/offl/lib/docs/luhadr/node8.html
//		  // it tries to do in automatic way
//		  //     but only really tested in ttbar events...
//		  idat[i][6]=bottomlayer*100 + 1;
//		  idat[j][6]=bottomlayer*100 + 2;
//		  // get two parents of these two members of color singlet
//		  int par[2];
//		  par[0] = idat[i][3];
//		  par[1] = idat[j][3];
//		  for (int ip=0; ip<2; ip++) { // loop over parents
//		    cout << "parent " << ip << " " << par[ip] << endl;
//		    // get other daughters of this parent
//		    std::vector <int> daughters;
//		    cout << "all daughters ";
//		    for (int k=0; k<npart+nextra; k++) {
//		      if (idat[k][3]==par[ip]) cout << k << " ";
//		      if (k==i || k==j) continue;
//		      if (idat[k][3]==par[ip]) daughters.push_back(k);
//		    }
//		    cout << endl;
//		    cout << "sel daughters ";
//		    for (uint kk=0; kk<daughters.size(); kk++) cout << daughters[kk] << " ";
//		    cout << endl;
//
//		    if (daughters.size()==1) { // maybe a W or Z or gamma ?
//		      if ( idat[daughters[0]][5]!=0 ) cout << "strange, single colored daughter..." << endl;
//		      else {
//			idat[daughters[0]][6] = bottomlayer*100 + ip + 1; // connect to colored particle
//		      }
//		    } else if (daughters.size()==2) {
//
//		      cout << "other daughters " << daughters[0] << " " << daughters[1] << endl;
//
//		      // make intermediate boson to connect these two daughters to the parent
//		      rdat[npart+nextra][2] = rdat[daughters[0]][2] + rdat[daughters[1]][2];
//		      rdat[npart+nextra][3] = rdat[daughters[0]][3] + rdat[daughters[1]][3];
//		      rdat[npart+nextra][4] = rdat[daughters[0]][4] + rdat[daughters[1]][4];
//		      rdat[npart+nextra][5] = rdat[daughters[0]][5] + rdat[daughters[1]][5];
//		      rdat[npart+nextra][0] = TMath::Sqrt( rdat[npart+nextra][5]*rdat[npart+nextra][5] 
//							      - rdat[npart+nextra][2]*rdat[npart+nextra][2] 
//							      - rdat[npart+nextra][3]*rdat[npart+nextra][3] 
//							      - rdat[npart+nextra][4]*rdat[npart+nextra][4]);
//		      rdat[npart+nextra][1] = 0; // charge (not important??)
//		      int totchg = int(rdat[daughters[0]][1] + rdat[daughters[1]][1]);
//		      if ( totchg == 0 ) {
//			idat[npart+nextra][0] = 23; // assume z0
//			rdat[npart+nextra][1] = 0; // charge 
//		      } else if ( totchg < 0 ) {
//			idat[npart+nextra][0] = -24; // assume W
//			rdat[npart+nextra][1] = -1;
//		      } else {
//			idat[npart+nextra][0] = 24;
//			rdat[npart+nextra][1] = 1;
//		      }
//		      idat[npart+nextra][1] = 2;
//
//		      //idat[npart+nextra][2] = i+1;
//		      idat[npart+nextra][2] = min(daughters[0],daughters[1])+1;
//
//		      //idat[npart+nextra][3] = par[ip]+1;
//		      idat[npart+nextra][3] = par[ip];
//		      idat[npart+nextra][4] = 0;
//		      		      
//		      cout << "made new parent, whose parent is " << idat[npart+nextra][3] << " " << par[ip] << endl;
//
//		      if (idat[daughters[0]][5]>0) {
//			idat[npart+nextra][5] = idat[daughters[0]][5]; // color singlet
//		      } else {
//			idat[npart+nextra][5] = 200+nextra;
//			idat[daughters[0]][5] = idat[npart+nextra][5];
//			idat[daughters[1]][5] = idat[npart+nextra][5];
//		      }
//		      idat[npart+nextra][6] = bottomlayer*100 + ip + 1; // should be same as for b or bbar
//
//		      idat[daughters[0]][6] = toplevel*100 + 1;
//		      idat[daughters[1]][6] = toplevel*100 + 1;
//		      toplevel--;
//
//
//		      // adjust parent of daughters to point to this new intermediate state
//		      idat[daughters[0]][3] = npart+nextra+1;
//		      idat[daughters[1]][3] = npart+nextra+1;
//
//		      // adjust # daughters of original parent particle
//		      idat[par[ip]-1][1]--;
//
//		      // un shuffled 
//		      cout << "unsuffled:" << npart << " " << nextra << endl;
//		      for (int ii=0; ii<npart+nextra+1; ii++) {
//			cout << std::setw(4) << ii << " " << ii+1 << " , ";
//			for (int jj=0; jj<7; jj++) cout << idat[ii][jj] << " ";
//			cout << endl;
//		      }
//
//		      // need to adjust position in list so that daughters are neighbors
//
//		      for (int ij=0; ij<7; ij++)
//			idat[npart+nextra+1][ij] = idat[npart+nextra][ij];
//
//		      for (int ij=0; ij<6; ij++)
//			rdat[npart+nextra+1][ij] = rdat[npart+nextra][ij];
//
//		      // what index should i put it at?
//		      int parentindex = idat[npart+nextra][3];
//		      int firstDaughterindex = idat[parentindex-1][2];
//		      int ndaughter = idat[parentindex-1][1];
//
//		      int insertPos = -1;
//		      int movePos = -1;
//
//		      cout << "daugheters of parent: " << parentindex << " " << firstDaughterindex << " " << ndaughter << endl;
//		      for (int ij=0; ij<ndaughter; ij++) {
//			cout << ij << " " << idat[firstDaughterindex-1+ij][1] << " " << idat[firstDaughterindex-1+ij][3] << endl;
//
//
//			if (idat[firstDaughterindex-1+ij][3]!=parentindex) {
//			  insertPos = firstDaughterindex-1+ij;
//			  movePos = firstDaughterindex-1+ndaughter;
//			  break;
//			}
//
//		      }
//
//		      cout << "insert, move pos = " << insertPos << " " << movePos << endl;
//
//		      for (int ij=npart+nextra; ij>movePos; ij--) {
//			for (int k=0; k<7; k++) idat[ij][k] = idat[ij-1][k];
//			for (int k=0; k<6; k++) rdat[ij][k] = rdat[ij-1][k];
//		      }
//		      for (int k=0; k<7; k++) idat[movePos][k] = idat[insertPos][k];
//		      for (int k=0; k<6; k++) rdat[movePos][k] = rdat[insertPos][k];
//
//		      for (int k=0; k<7; k++) idat[insertPos][k] = idat[npart+nextra+1][k];
//		      for (int k=0; k<6; k++) rdat[insertPos][k] = rdat[npart+nextra+1][k];
//
//		      for (int ij=0; ij<7; ij++) idat[npart+nextra+1][ij] = 0;
//		      for (int ij=0; ij<6; ij++) rdat[npart+nextra+1][ij] = 0;
//
//		      // shuffled 
//		      cout << "suffled:" << endl;
//		      for (int ii=0; ii<npart+nextra+1; ii++) {
//			cout << ii << " " << ii+1 << " , ";
//			for (int jj=0; jj<7; jj++) cout << idat[ii][jj] << " ";
//			cout << endl;
//		      }
//
//		      // update parent/daughter indices
//		      for (int ij=0; ij<npart+nextra+1; ij++) {
//
//			if (idat[ij][2]==npart+nextra+1) idat[ij][2]=insertPos+1;
//			else if (idat[ij][2]==insertPos+1) idat[ij][2]=movePos+1;
//			else if (idat[ij][2]>insertPos) idat[ij][2]++;
//
//			if (idat[ij][3]==npart+nextra+1) idat[ij][3]=insertPos+1;
//			else if (idat[ij][3]==insertPos+1) idat[ij][3]=movePos+1;
//			else if (idat[ij][3]>insertPos) idat[ij][3]++;
//		      }
//
//		      for (int iip=0; iip<2; iip++) {
//			if (par[iip]==npart+nextra+1) par[iip]=insertPos+1;
//			else if (par[iip]==insertPos+1) par[iip]=movePos+1;
//			else if (par[iip]>insertPos) par[iip]++;
//		      }
//			  
//		      if (i==npart+nextra+1)   i=insertPos+1;
//		      else if (i==insertPos+1) i=movePos+1;
//		      else if (i>insertPos)    i++;
//
//		      if (j==npart+nextra+1)   j=insertPos+1;
//		      else if (j==insertPos+1) j=movePos+1;
//		      else if (j>insertPos)    j++;
//
//
//		      // shuffled 
//		      cout << "corrected indices:" << endl;
//		      for (int i=0; i<npart+nextra+1; i++) {
//			cout << i << " " << i+1 << " , ";
//			for (int j=0; j<7; j++) cout << idat[i][j] << " ";
//			cout << endl;
//		      }
//
//
//		      nextra++;
//
//
//		    }
//		  }
//		  bottomlayer++;
//		}
//	      }
//	    }
//	  }
//	  npart+=nextra;
//

	  //cout << "final color/shower settings:" << endl;
	  //for (int i=0; i<npart+nextra; i++) {
	  //  cout << std::setw(4) << i << " " << std::setw(4) << i+1 << " , ";
	  //  for (int j=0; j<7; j++) cout << std::setw(4) << idat[i][j] << " ";
	  //  cout << endl;
	  //}

	  break;
	} else if (!inevent) {
	  continue;
	} else {
	  //std::cout << lline << std::endl;
	  TObjArray* hh = lline.Tokenize(" ");
	  //int status = ((TObjString*) hh->At(1))->GetString().Atoi();
	  for (int i=0; i<7; i++) idat[npart][i]=0;
	  for (int i=0; i<6; i++) rdat[npart][i]=0;
	  int pid = ((TObjString*) hh->At(0))->GetString().Atoi();
	  idat[npart][0] = pid;
	  idat[npart][1] = 0;
	  idat[npart][2] = 0;
	  int imo = ((TObjString*) hh->At(2))->GetString().Atoi();
	  imo-=1;
	  if (imo>=0) {
	    if (imo>=0) {
	      idat[npart][3] = imo+1;
	      idat[imo][1]++;
	      if (idat[imo][2]==0 || npart+1<idat[imo][2]) idat[imo][2] = npart+1;
	    }
	  }
	  idat[npart][4] = ((TObjString*) hh->At(12))->GetString().Atoi();
	  // save some color info for end of event
	  int icol1 = ((TObjString*) hh->At(4))->GetString().Atoi();
	  int icol2 = ((TObjString*) hh->At(5))->GetString().Atoi();
	  int jcol = icol1>0 ? icol1 : icol2;
	  //jcol-=500; // first color = 501->1
	  idat[npart][5] = jcol>0 ? jcol : 0;
	  rdat[npart][0] = ((TObjString*) hh->At(10))->GetString().Atof();
	  switch (TMath::Abs(pid)) {
	  case 12:
	  case 14:
	  case 16:
	  case 21:
	  case 22:
	  case 23:
	  case 25:
	    rdat[npart][1] = 0;
	    break;
	  case 11:
	  case 13:
	  case 15:
	    rdat[npart][1] = pid > 0 ? -1 : 1;
	    break;
	  case 24:
	    rdat[npart][1] = pid < 0 ? -1 : 1;
	    break;
	  case 1:
	  case 3:
	  case 5:
	    //rdat[npart][1] = pid < 0 ? 1/3. : -1/3;
	    rdat[npart][1] = pid < 0 ? 1 : -1;
	    break;
	  case 2:
	  case 4:
	  case 6:
	    //rdat[npart][1] = pid < 0 ? -2/3. : 2/3;
	    rdat[npart][1] = pid < 0 ? -1 : 1;
	    break;
	  default:
	    std::cout << "unknown chrge for particle " << pid << endl;
	    break;
	  }
	  //rdat[npart][1] = 
	  rdat[npart][2] = ((TObjString*) hh->At(6))->GetString().Atof();
	  rdat[npart][3] = ((TObjString*) hh->At(7))->GetString().Atof();
	  rdat[npart][4] = ((TObjString*) hh->At(8))->GetString().Atof();
	  rdat[npart][5] = ((TObjString*) hh->At(9))->GetString().Atof();
	  hh->Delete(); delete hh; hh=NULL;
	  npart++;
	}

      }
    }

    fNparton=npart;

  }
  else {
    JSFReadParton *rdp=(JSFReadParton*)Module();
    //ifstream fin(fDataFileName);
    //    while( !fin.eof() ) {
    Int_t npgen=rdp->GetNPGen();
    fNparton=npgen-2;
    Int_t io=0;
    ifstream *fin=rdp->GetInStream();
    if ( fin->good() && !fin->eof() ) {
      for(Int_t ip=0;ip<npgen;ip++) {
	*fin >> rdat[io][2] >> rdat[io][3] >> rdat[io][4] ;
	*fin >> rdat[io][5] >> rdat[io][0];
        rdat[io][1]=rdp->GetCharge(ip);
        idat[io][0]=rdp->GetPID(ip);
        idat[io][1]=0; // ndaughter
        idat[io][2]=0; // first daughter
        idat[io][3]=0; // mother
        idat[io][4]=0; // helicity

#if 1
        idat[io][5]=0; // color id
        idat[io][6]=0; // shower info
#else // yoshioka-san's approach
        Int_t iq = TMath::Abs(rdp->GetPID(ip));
        if ( iq > 0 && iq < 9 ){
          idat[io][5] = iq;   // color id
          idat[io][6] = 101;  // shower info
        }
        else {
          idat[io][5]=0; // color id
          idat[io][6]=0; // shower info
        }
#endif
	//cout << ip << " " << io << " ";
	//for (int kk=0; kk<7; kk++) cout << idat[io][kk] << " ";
	//cout << endl;

        if( ip > 1 ) io++;
      }
    }
    else {
      std::cout << "In JSFReadPartonBuf::SetParton .. " ;
      std::cout << "read end-of-file of the input file," ;
      std::cout << rdp->GetDataFileName() << std::endl;
      return kFALSE;
    }      
  }

  // ****************************
  TVector p(4);
  for(Int_t i=0;i<fNparton;i++){
    p(1)=rdat[i][2];
    p(2)=rdat[i][3];
    p(3)=rdat[i][4];
    if( rdat[i][5] < 0.0 ) {
      rdat[i][5]=TMath::Sqrt(rdat[i][0]*rdat[i][0] + rdat[i][2]*rdat[i][2]
			   + rdat[i][3]*rdat[i][3] + rdat[i][4]*rdat[i][4]);
    }
    p(0)=rdat[i][5];

    //std::cerr << "ReadParton summary: " ;
    //std::cerr << "i=" << i << "(px,py,pz)=" << rdat[i][2] ;
    //std::cerr << " " << rdat[i][3] << " " << rdat[i][4] ;
    //std::cerr << " mass=" << rdat[i][0] ;
    //std::cerr << " id= " << idat[i][0] ; 
    //std::cerr << " nchild= " << idat[i][1] ; 
    //std::cerr << " 1stchild= " << idat[i][2] ; 
    //std::cerr << " parent= " << idat[i][3] ; 
    //std::cerr << " helicity= " << idat[i][4] ; 
    //std::cerr << " colsinglet= " << idat[i][5] ; 
    //std::cerr << " showerinfo= " << idat[i][6] ; 
    //std::cerr << std::endl;

    new(partons[i]) JSFSpringParton(i+1, idat[i][0], rdat[i][0], rdat[i][1],
		    p, idat[i][1], idat[i][2], idat[i][3], 
		    idat[i][4], idat[i][5], idat[i][6] );
  }

  return kTRUE ;
}


//_____________________________________________________________________________
Bool_t JSFReadParton::BeginRun(Int_t nrun)
{
  if( !IsWritable() ) return kTRUE; // Quit when not output mode.
// Open a file 
  if( fType == 1 || fType == 2 ) {
    Int_t lenf=strlen(fDataFileName);
    readpartonopen_(&fUnit, &fType, fDataFileName, lenf);
  }
  else {
    fInStream.open(fDataFileName, ifstream::in);
    if( fInStream.bad() ) {
      std::cerr << "JSFReadParton::BeginRun failed to open input file " ;
      std::cerr << GetDataFileName() << std::endl;
      return kFALSE;
    }
  }
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFReadParton::EndRun()
{
  if( !IsWritable() ) return kTRUE;
  if( fType == 1 || fType == 2 ) {
    readpartonclose_(&fUnit);
  }
  else {
    fInStream.close();
  }

  return kTRUE;

}

// ---------------------------------------------------------------
Bool_t JSFReadParton::Process(Int_t nev)
{
//
  JSFReadPartonBuf *simb=(JSFReadPartonBuf*)EventBuf();
  return simb->SetPartons();
}


