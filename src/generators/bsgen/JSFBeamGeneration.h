#ifndef __JSFBeamGeneration__
#define __JSFBeamGeneration__

///////////////////////////////////////////////////////////////////////
//
//  JSFBeamGeneration
//
//  Generate BeamStraulung spectrum of JLC
//
//(How to use)
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TNamed.h>
#include <TString.h>
#include <TRandom2.h>
#include <TLorentzVector.h>
#include <TF2.h>


// __________________________________________________________
class JSFBeamGeneration : public TNamed
{
 protected:
  Int_t  fNGenerated;     //! Number of generated event
  Double_t fEEInitial;    //! Initial Electron Energy (GeV)  
  Double_t fEPInitial;    //! Initial Positron Energy (GeV)

  Int_t fBeamDataFormat;  //  Beam Data Format
  Double_t fLuminosity;   //  Luminosity( x 10^33 /cm^2/s )
  Double_t fNominalE;     //  Nominal Beam Energy (GeV)
  TString fParameterName; //  Name of parameter
  TRandom2 fRandom;       //!  Uniform random number generator

  inline Double_t GetGauss(Double_t mean=0.0, Double_t sigma=1.0){ 
                             return fRandom.Gaus(mean,sigma); }


 public:
  JSFBeamGeneration();
  JSFBeamGeneration(Int_t format, Double_t lum, Double_t nominalE, Char_t *name);
  virtual ~JSFBeamGeneration(){}

  inline void     IncrementEventNumber(){ fNGenerated++; }
  inline Int_t    GetBeamDataFormat(){ return fBeamDataFormat; }
  inline TString &GetParameterName(){ return fParameterName; }
  inline virtual Double_t GetLuminosity(){ return fLuminosity; }
  inline virtual Double_t GetInitialElectronEnergy(){ return fEEInitial; }
  inline virtual Double_t GetInitialPositronEnergy(){ return fEPInitial; }
  inline virtual Double_t GetNominalEnergy(){ return fNominalE; }
  inline virtual Double_t GetIBWidth(){ return 0.0; }

  inline virtual Double_t GetRndm(){ return fRandom.Rndm(); }

  virtual void Print();

  virtual Double_t GetWeight(const Double_t random_initial_eminus, 
			const Double_t random_initial_eplus,
			const Double_t random_bs_eminus,
			const Double_t random_bs_eplus,
			Double_t &eminus, Double_t &eplus)=0;
  virtual void Generate(Double_t &eminus, Double_t &eplus);
  virtual void GenEnergySpread(Double_t &eminus, Double_t &eplus);
  virtual void GenBeamStrahlung(Double_t &eminus, Double_t &eplus);
  virtual void Generate(TLorentzVector &ecm, TLorentzVector &vtx);
  virtual void GenBeamStrahlung(TLorentzVector &ecm, TLorentzVector &vtx);

  ClassDef(JSFBeamGeneration,3)  // Virtual class of  Beam Spectrum generation

};


//_____________________________________________________________
class JSFBeamGenerationCain: public JSFBeamGeneration
{
// Luminosity spectrum created by CAIN generator
 public:
  enum EIBType  { kUniform=1, kGauss=2 } ;

 protected:
  Double_t fLumRatio[3]; // Luminosity of each part.
  Double_t fBndOrder;    // < 10^(fBndOrder) region is uniform 

  //--- Parameter for projected part.
  Double_t fPEdge1[3]; // parabola parameter of low X region.
  Double_t fPEdgeLimit; // Boundary of lowX and HighX.
  Int_t    fNEdge2;    // Number of parameters used to describe high X region
  Double_t *fPEdge2;   //[fNEdge2]  Parameters to describe high X region.

  //--- Parameter for central part.
  Int_t  fNfits;   // Number of fit parameter
  Int_t  fNpmax;   // Max number of parameter
  Int_t  fPCntSize; // Size of fPCnt Array
  Double_t *fPCnt; //[fPCntSize]  Parameters for central part.

  //--- Parameter for initial beam spread.
  EIBType  fIBType;      // Type of nominal energy spread
  Double_t fIBWidth;     // Relative width(sigma) of initial beam spread
  Double_t fIBWidthEMinus; // Initial beam energy spread of E-Minus beam
  Double_t fIBWidthEPlus;  // Initial beam energy spread of E-Plus beam
  // if fIBType=kUniform, fIBWidth is half width of beam spread.
  // if IBWidthEMinus(EPlus) is negative, fIBWidth is used.


  //--- Parameters for generation of 2D part.
  Int_t    fMdiv;     // No. of division in one axis 
  Int_t    fMapsize;  // size of array for random generation (fMdiv*fMdiv)
  Double_t *fMxmin;   //[fMapsize] xmin in each cell
  Double_t *fMxmax;   //[fMapsize] xmax in each cell
  Double_t *fMymin;   //[fMapsize] ymin in each cell
  Double_t *fMymax;   //[fMapsize] ymax in each cell
  Double_t *fMaint;   //[fMapsize] Acumulated integral of bin
  Double_t *fMvmax;   //[fMapsize] Maximum of each bin
  TF2      *fBS2DFunction; //!  BS2DFunction
  Double_t  fTotalInt;     //   Total integration

  static Double_t fLastEMinus0;  // E- energy before BS of last call 
  static Double_t fLastEPlus0;   // E+ energy before BS of last call 

 public:
  JSFBeamGenerationCain();
  virtual ~JSFBeamGenerationCain();

  inline Double_t GetLastEMinus0(){ return fLastEMinus0; }
  inline Double_t GetLastEPlus0(){ return fLastEPlus0; }

  virtual Double_t GetWeight(const Double_t random_initial_eminus, 
			const Double_t random_initial_eplus,
			const Double_t random_bs_eminus,
			const Double_t random_bs_eplus,
			Double_t &eminus, Double_t &eplus);
  inline void    GetLumRatio(Double_t *ratio){ ratio[0]=fLumRatio[0]; 
                     ratio[1]=fLumRatio[1]; ratio[2]=fLumRatio[2]; }
  inline Double_t GetLumRatio(Int_t i){ return fLumRatio[i]; }
  inline Double_t GetBndOrder(){ return fBndOrder; }
  inline virtual EIBType  GetIBType(){ return fIBType;  }
  inline virtual Double_t GetIBWidth(){ return fIBWidth; }
  inline virtual Double_t GetIBWidthEMinus(){ return ( fIBWidthEMinus > 0 ? fIBWidthEMinus : fIBWidth ) ; } 
  inline virtual Double_t GetIBWidthEPlus(){   return ( fIBWidthEPlus > 0 ? fIBWidthEPlus : fIBWidth ) ; } 

  inline Double_t *GetPCnt(){ return fPCnt; }
  inline Int_t GetPCntSize(){ return fPCntSize; }
  inline TF2   *GetBS2DFunction(){ return fBS2DFunction; }

  virtual void Print();
  
  void SetBSParameters(const Int_t beamdataformat, const TString parametername,
	   const Double_t luminosity, const Double_t nominalE, const Double_t lumratio[3],
	   const Double_t bndorder, const Double_t pedge1[3],  const Double_t edgelimit, 
           const Int_t fnedge2, const Double_t pedge2[], const Int_t nfits, 
	   const Int_t npmax, const Double_t pcnt[]);
  
  void SetIBParameters(const Double_t width, const EIBType type=kUniform);
  void SetIBParameters(const Double_t nominalE, const Double_t width, const EIBType type=kUniform);
  void SetIBParameters(const Double_t nominalE, const Double_t widthEMinus, 
		       const Double_t widthEPlus, const EIBType type=kUniform);
  void SetIBParametersByEnv(const Double_t nominalE=-1.0);

  //  virtual void Generate(Double_t &eminus, Double_t &eplus);
  virtual void GenEnergySpread(Double_t &eminus, Double_t &eplus);
  virtual void GenBeamStrahlung(Double_t &eminus, Double_t &eplus);
  //  virtual void Generate(TLorentzVector &ecm, TLorentzVector &vtx);
  virtual void GenBeamStrahlung(TLorentzVector &ecm, TLorentzVector &vtx);
  

  Double_t GenBSEnergy1(const Double_t eini, const Double_t xrand=-1.0);
  Bool_t   GenBSEnergy2(Double_t &eminus, Double_t &eplus);
  Double_t GetBSEnergy2Weight(const Double_t xbsm, const Double_t xbsp,
			      Double_t &eminus, Double_t &eplus) ;


  void MakeBSMap(); //
  Double_t BS1DFunction(Double_t *x, Double_t *p); //
  Double_t BS2DFunction(Double_t *x, Double_t *p); //
  Double_t PolFunc6(Double_t *x, Double_t *p);    

  ClassDef(JSFBeamGenerationCain,2)  // Generate CAIN Beam Spectrum
};

#endif


