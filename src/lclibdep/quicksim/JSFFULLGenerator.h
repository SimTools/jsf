#ifndef __JSFFULLGenerator__
#define __JSFFULLGenerator__

//////////////////////////////////////////////////////////////////////////
//
// JSFFULLGenerator
//
// Convert TBS data to JSF classes                 
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif
#ifndef __JSFGenerator__
#include "JSFGenerator.h"
#endif


class JSFFULLGenerator : public JSFGenerator {
public:
   JSFSpring *fSpring;	//!
public:
   JSFFULLGenerator(const char *name="JSFFULLGenerator", 
                    const char *title="JSFFULLGenerator");
   virtual      ~JSFFULLGenerator();

   void TBPUT(JSFSpring *spring); // Copy JSFSpringParton class to TBS bank
   void TBGET(); // Copy TBS bank data to JSFGeneratorParticle class

   void SetSpring(JSFSpring *spring){ fSpring=spring; return;} 

   ClassDef(JSFFULLGenerator,1)  // FULL to JSF converter
};


#endif



