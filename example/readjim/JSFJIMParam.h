#ifndef __JSFJIMParam__
#define __JSFJIMParam__

//////////////////////////////////////////////////////////////////////////
//
// JSFJIMParam
//
// A JIM Parameter class
//
//$Id:
//
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFQuickSimParam__
#include "JSFQuickSimParam.h"
#endif

// ******* JSFQuickSimParam *************************************
class JSFJIMParam : public JSFQuickSimParam {
protected:
public:
   JSFJIMParam();
   virtual ~JSFJIMParam();

   void ReadParameter() ;// Read detector parameter
					      
   ClassDef(JSFJIMParam,1) // JSFJIMParam
};
 
#endif

