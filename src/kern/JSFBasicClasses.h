#ifndef __JSFBasicClasses__
#define __JSFBasicClasses__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFBasicClasses                                                      //
//                                                                      //
//$Id$ 
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TMath.h"



//***************************************************************************
class JSF2DV {
  public:
    Double_t x;
    Double_t y;
  public:
    JSF2DV(Double_t xi=0, Double_t yi=0){x=xi; y=yi;}

    inline Double_t Abs(){ return TMath::Sqrt(x*x+y*y); }
    inline JSF2DV UnitV(){ Double_t l=Abs(); return JSF2DV(x/l, y/l) ; }
    inline JSF2DV NormalV(){ return JSF2DV(-y, x); }
    
    inline JSF2DV operator + (JSF2DV v) { return JSF2DV( x+v.x, y+v.y); }
    inline JSF2DV operator - (JSF2DV v) { return JSF2DV( x-v.x, y-v.y); }
    inline Double_t operator * (JSF2DV v) { return ( x*v.x+y*v.y); }
    inline JSF2DV operator * (Double_t f){ return JSF2DV( f*x, f*y) ; }
    inline Bool_t operator == (const JSF2DV &v) const{
      return (v.x==x && v.y==y) ? kTRUE : kFALSE;
    }

    void Print(){ printf("%g,%g",x,y);}
    inline Double_t X ()   const { return x; };
    inline Double_t Y ()   const { return y; };
    inline void Set(Double_t X, Double_t Y){x=X; y=Y;}
};

//***************************************************************************

class JSFRThPhi_f;

class JSFRThPhi;
class JSFRPhiZ;

class JSF3DV_f {
public:
  Float_t x;
  Float_t y;
  Float_t z;
public:
  JSF3DV_f(Float_t X=0, Float_t Y=0, Float_t Z=0){ x=X; y=Y; z=Z; }

  inline Float_t Abs(){ return TMath::Sqrt(x*x+y*y+z*z); }
  inline JSF3DV_f UnitV(){ Float_t l=Abs(); return JSF3DV_f(x/l, y/l, z/l) ; }
    
  inline JSF3DV_f operator + (JSF3DV_f v) { return JSF3DV_f( x+v.x, y+v.y, z+v.z); }
  inline JSF3DV_f operator - (JSF3DV_f v) { return JSF3DV_f( x-v.x, y-v.y, z-v.z); }
  inline Float_t operator * (JSF3DV_f v) { return ( x*v.x+y*v.y+z*v.z); }
  inline JSF3DV_f operator * (Float_t f){ return JSF3DV_f( f*x, f*y, f*z) ; }
  void Print(){ printf("%g,%g,%g",x,y,z);}


};

class JSF3DV {
public:
  Double_t x;
  Double_t y;
  Double_t z;
public:
  JSF3DV(Double_t X=0, Double_t Y=0, Double_t Z=0){ x=X; y=Y; z=Z; }

  inline Double_t Abs(){ return TMath::Sqrt(x*x+y*y+z*z); }
  inline JSF3DV UnitV(){ Double_t l=Abs(); return JSF3DV(x/l, y/l, z/l) ; }
     
  inline JSF3DV operator + (JSF3DV v) { return JSF3DV( x+v.x, y+v.y, z+v.z); }
  inline JSF3DV operator - (JSF3DV v) { return JSF3DV( x-v.x, y-v.y, z-v.z); }
  inline Double_t operator * (JSF3DV v) { return ( x*v.x+y*v.y+z*v.z); }
  inline JSF3DV operator * (Double_t f){ return JSF3DV( f*x, f*y, f*z) ; }
  void Print(){ printf("%g,%g,%g",x,y,z);}
  inline Double_t X ()   const { return x; };
  inline Double_t Y ()   const { return y; };
  inline Double_t Z ()   const { return z; };
  inline void Set(Double_t X, Double_t Y, Double_t Z){x=X;y=Y;z=Z;}
  inline Bool_t operator == (const JSF3DV &v) const{
      return (v.x==x && v.y==y && v.z==z) ? kTRUE : kFALSE;
    }


} ;

class JSFRPhiZ_f {
public:
  Float_t r;
  Float_t phi;
  Float_t z;
};

class JSFRPhiZ {
public:
  Double_t r;
  Double_t phi;
  Double_t z;
public:
  JSFRPhiZ(Double_t ri=0, Double_t phii=0, Double_t zi=0){ r=ri; phi=phii ; z=zi; }
  JSFRPhiZ(JSF3DV xyz){
     r=TMath::Sqrt(xyz.x*xyz.x+xyz.y*xyz.y);
     phi=TMath::ATan2(xyz.y, xyz.x);
     z=xyz.z; 
  }
  inline JSF3DV XYZ(){ JSF3DV xyz; xyz.x=r*TMath::Cos(phi);
                xyz.y=r*TMath::Sin(phi);
                xyz.z=z;
		return xyz; 
  }
};

class JSFRThPhi_f {
public:
  Float_t r;
  Float_t th;
  Float_t phi;

public:
  JSFRThPhi_f(Float_t ri=0, Float_t thi=0, Float_t phii=0){ r=ri;th=thi;phi=phii;}
  inline JSF3DV_f XYZ(){ JSF3DV_f xyz; xyz.x=r*TMath::Sin(th)*TMath::Cos(phi);
                xyz.y=r*TMath::Sin(th)*TMath::Sin(phi);
                xyz.z=r*TMath::Cos(th);
		return xyz; 
  }
};

class JSFRThPhi {
public:
  Double_t r;
  Double_t th;
  Double_t phi;

  inline JSF3DV XYZ(){ JSF3DV xyz; xyz.x=r*TMath::Sin(th)*TMath::Cos(phi);
                xyz.y=r*TMath::Sin(th)*TMath::Sin(phi);
                xyz.z=r*TMath::Cos(th);
		return xyz; 
  }

};

#endif





