#ifndef __JSFReadGenerator__
#define __JSFReadGenerator__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFReadGenerator                                                     //
//                                                                      //
// A class to read Generator data and put them into class               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFGenerator__
#include "JSFGenerator.h"
#endif

class JSFReadGenerator;
class JSFReadGeneratorBuf;

// ********************************************************
class JSFReadGeneratorBuf : public JSFGeneratorBuf {
public:
   
   JSFReadGeneratorBuf(const char *name="JSFReadGeneratorBuf",
     const char *title="JSF Read Generator", JSFReadGenerator *module=0);

   virtual ~JSFReadGeneratorBuf(){}

   virtual Bool_t ReadOneRecord();
   virtual Bool_t ReadHepEvent(const Int_t maxhep, Int_t &nevhep, Int_t &nhep, 
   	 Int_t isthep[], Int_t idhep[], Int_t jmohep[][2], Int_t jdahep[][2],
            Double_t phep[][5], Double_t vhep[][4]);

   ClassDef(JSFReadGeneratorBuf, 1) // JSF ReadGenerator EventBuffer class.

};

// ********************************************************
class JSFReadGenerator : public JSFGenerator {
protected:
  Int_t  fUnit;       // Fortran logical unit number to output SIMDST data.
  Char_t fDataFileName[256]; //! File name of output file.
  Char_t fFormat[32];  // Input binary data format.	   
public:
   JSFReadGenerator(const char *name="JSFReadGenerator", 
		 const char *title="JSFReadGenerator",
		 const Bool_t makebuf=kTRUE);

   virtual Bool_t BeginRun(Int_t run);
   virtual Bool_t EndRun();
   virtual Bool_t Process(Int_t event);

   Int_t  GetUnit(){ return fUnit;}
   Char_t *GetDataFileName(){ return fDataFileName; }
   void SetUnit(Int_t nunit){ fUnit=nunit; }
   void SetDataFileName(Char_t *name){ strcpy(fDataFileName, name);}

   ClassDef(JSFReadGenerator,2)  // To read Generator data
};


#endif


