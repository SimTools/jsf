//*LastUpdate : 10-July-2000     Akiya Miyamoto 
//*-- Author  : Akiya Miyamoto  10-July-2000

///////////////////////////////////////////////////////////////////
//
//  Vertex classes
//
//$Id:
//  
//////////////////////////////////////////////////////////////////


#include "JSFVirtualVertex.h"
#include "JSFGeneratorParticle.h"

ClassImp(JSFVirtualVertex)
ClassImp(JSFVirtualVertexing)


//____________________________________________
JSFVirtualVertex::JSFVirtualVertex()
{
  fMass=0.0;
  fCharge=0.0;
  fNcharged=0;
  fTracks=NULL;
  fPos[0]=0.0;  fPos[1]=0.0;  fPos[3]=0.0;
}

//___________________________________________________
JSFVirtualVertex::~JSFVirtualVertex()
{
  if( fTracks ) {
    fTracks->Delete();
    delete fTracks;
    fTracks=0;
  }
}

//___________________________________________________
void JSFVirtualVertex::MakeVertex()
{

  fCharge=0.0;
  fNcharged=0;

  return ;
}


