#ifndef __JSFJ4TOutput__
#define __JSFJ4TOutput__

///////////////////////////////////////////////////////////////////////
//
//  Template class to output Jupiter data as root file.
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include "J4Output.hh"

class TObjArray;

template <class JSFHitClass, class J4HitClass>
class JSFJ4TOutput : public J4Output
{
 public:
  TObjArray   *fJSFJ4Buf;

 public:
  JSFJ4TOutput(TObjArray *buf=0){
    J4HitClass::SetOutput(this);
    fJSFJ4Buf=buf;
  }

  virtual ~JSFJ4TOutput(){}

  virtual void Output(J4VHit *ahit){
    fJSFJ4Buf->Add(new JSFHitClass((J4HitClass*)ahit) );
  }
  virtual void Clear(){
    if( fJSFJ4Buf ) fJSFJ4Buf->Clear();
  }

}; 

#endif
