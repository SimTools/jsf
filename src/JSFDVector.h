
#ifndef ROOT_JSFDVector
#define ROOT_JSFDVector

//+SEQ,CopyRight,T=NOINCLUDE.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Linear Algebra Package                                               //
//                                                                      //
// The present package implements all the basic algorithms dealing      //
// with vectors, matrices, matrix columns, rows, diagonals, etc.        //
//                                                                      //
// Matrix elements are arranged in memory in a COLUMN-wise              //
// fashion (in FORTRAN's spirit). In fact, it makes it very easy to     //
// feed the matrices to FORTRAN procedures, which implement more        //
// elaborate algorithms.                                                //
//                                                                      //
// Unless otherwise specified, matrix and vector indices always start   //
// with 0, spanning up to the specified limit-1.                        //
//                                                                      //
// The present package provides all facilities to completely AVOID      //
// returning matrices. Use "TMatrix A(TMatrix::kTransposed,B);" and     //
// other fancy constructors as much as possible. If one really needs    //
// to return a matrix, return a TLazyMatrix object instead. The         //
// conversion is completely transparent to the end user, e.g.           //
// "TMatrix m = THaarMatrix(5);" and _is_ efficient.                    //
//                                                                      //
// For usage examples see $ROOTSYS/test/vmatrix.cxx and vvector.cxx     //
// and also:                                                            //
// http://root.cern.ch/root/html/TMatrix.html#TMatrix:description       //
//                                                                      //
// The implementation is based on original code by                      //
// Oleg E. Kiselyov (oleg@pobox.com).                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
//*KEEP,TObject.
#include "TObject.h"
//*KEND.
#endif
#ifndef ROOT_TMath
//*KEEP,TMath.
#include "TMath.h"
//*KEND.
#endif
#ifndef ROOT_TError
//*KEEP,TError.
#include "TError.h"
//*KEND.
#endif


class JSFDMatrix;
class JSFDElementAction;
class JSFDElementPosAction;


class JSFDVector : public TObject {

friend class JSFDMatrixRow;
friend class JSFDMatrixColumn;
friend class JSFDMatrixDiag;

protected:
   Int_t     fNrows;            // number of rows
   Int_t     fNmem;             // number of rows in allocated memory (>=fNrows)
   Int_t     fRowLwb;           // lower bound of the row index
   Double_t   *fElements;	        // elements themselves

   void Allocate(Int_t nrows, Int_t row_lwb = 0);
   void Invalidate() { fNrows = -1; fElements = 0; }

public:
   JSFDVector() { Invalidate(); }
   JSFDVector(Int_t n);
   JSFDVector(Int_t lwb, Int_t upb);
   JSFDVector(const JSFDVector &another);
#ifndef __CINT__
   JSFDVector(Int_t lwb, Int_t upb, Double_t iv1, ...);
#endif

   virtual ~JSFDVector();

   void ResizeTo(Int_t n);
   void ResizeTo(Int_t lwb, Int_t upb);
   void ResizeTo(const JSFDVector &v);

   Bool_t IsValid() const;

   Double_t &operator()(Int_t index) const;
   Double_t &operator()(Int_t index);

   Int_t GetLwb() const            { return fRowLwb; }
   Int_t GetUpb() const            { return fNrows + fRowLwb - 1; }
   Int_t GetNrows() const          { return fNrows; }
   Int_t GetNoElements() const     { return fNrows; }

   JSFDVector &operator=(const JSFDVector &source);
   JSFDVector &operator=(Double_t val);
   JSFDVector &operator=(const JSFDMatrixRow &mr);
   JSFDVector &operator=(const JSFDMatrixColumn &mc);
   JSFDVector &operator=(const JSFDMatrixDiag &md);
   JSFDVector &operator-=(Double_t val);
   JSFDVector &operator+=(Double_t val);
   JSFDVector &operator*=(Double_t val);
   JSFDVector &operator*=(const JSFDMatrix &a);

   Bool_t operator==(Double_t val) const;
   Bool_t operator!=(Double_t val) const;
   Bool_t operator<(Double_t val) const;
   Bool_t operator<=(Double_t val) const;
   Bool_t operator>(Double_t val) const;
   Bool_t operator>=(Double_t val) const;

   JSFDVector &Zero();
   JSFDVector &Abs();
   JSFDVector &Sqr();
   JSFDVector &Sqrt();

   JSFDVector &Apply(JSFDElementAction &action);
   JSFDVector &Apply(JSFDElementPosAction &action);

   Double_t Norm1() const;
   Double_t Norm2Sqr() const;
   Double_t NormInf() const;

   void Print(Option_t *option="");

   friend JSFDVector &operator+=(JSFDVector &target, const JSFDVector &source);
   friend JSFDVector &operator-=(JSFDVector &target, const JSFDVector &source);
   friend Double_t operator*(const JSFDVector &v1, const JSFDVector &v2);
   friend JSFDVector &Add(JSFDVector &target, Double_t scalar, const JSFDVector &source);
   friend JSFDVector &ElementMult(JSFDVector &target, const JSFDVector &source);
   friend JSFDVector &ElementDiv(JSFDVector &target, const JSFDVector &source);

   friend Bool_t operator==(const JSFDVector &v1, const JSFDVector &v2);
   friend void Compare(const JSFDVector &im1, const JSFDVector &im2);
   friend Bool_t AreCompatible(const JSFDVector &v1, const JSFDVector &v2);

   ClassDef(JSFDVector,1)  // Vector class
};


// Service functions (useful in the verification code).
// They print some detail info if the validation condition fails
void VerifyElementValue(const JSFDVector &v, Double_t val);
void VerifyVectorIdentity(const JSFDVector &v1, const JSFDVector &v2);


//----- inlines ----------------------------------------------------------------

#ifndef R__HPUX

#ifndef __CINT__

inline JSFDVector::JSFDVector(Int_t n)
{
   Allocate(n);
}

inline JSFDVector::JSFDVector(Int_t lwb, Int_t upb)
{
   Allocate(upb-lwb+1, lwb);
}

inline Bool_t JSFDVector::IsValid() const
{
   if (fNrows == -1)
      return kFALSE;
   return kTRUE;
}

inline Bool_t AreCompatible(const JSFDVector &v1, const JSFDVector &v2)
{
   if (!v1.IsValid()) {
      ::Error("AreCompatible", "vector 1 not initialized");
      return kFALSE;
   }
   if (!v2.IsValid()) {
      ::Error("AreCompatible", "vector 2 not initialized");
      return kFALSE;
   }

   if (v1.fNrows != v2.fNrows || v1.fRowLwb != v2.fRowLwb)
      return kFALSE;

   return kTRUE;
}

inline JSFDVector &JSFDVector::operator=(const JSFDVector &source)
{
   if (this != &source && AreCompatible(*this, source)) {
      TObject::operator=(source);
      memcpy(fElements, source.fElements, fNrows*sizeof(Double_t));
   }
   return *this;
}

inline JSFDVector::JSFDVector(const JSFDVector &another)
{
   if (another.IsValid()) {
      Allocate(another.GetUpb()-another.GetLwb()+1, another.GetLwb());
      *this = another;
   } else
      Error("JSFDVector(const JSFDVector&)", "other vector is not valid");
}

inline void JSFDVector::ResizeTo(Int_t n)
{
   JSFDVector::ResizeTo(0,n-1);
}

inline void JSFDVector::ResizeTo(const JSFDVector &v)
{
   JSFDVector::ResizeTo(v.GetLwb(), v.GetUpb());
}

inline Double_t &JSFDVector::operator()(Int_t ind) const
{
   static Double_t err;
   err = 0.0;

   if (!IsValid()) {
      Error("operator()", "vector is not initialized");
      return err;
   }

   Int_t aind = ind - fRowLwb;
   if (aind >= fNrows || aind < 0) {
      Error("operator()", "requested element %d is out of vector boundaries [%d,%d]",
            ind, fRowLwb, fNrows+fRowLwb-1);
      return err;
   }

   return fElements[aind];
}

inline Double_t &JSFDVector::operator()(Int_t index)
{
   return (Double_t&)((*(const JSFDVector *)this)(index));
}

inline JSFDVector &JSFDVector::Zero()
{
   if (!IsValid())
      Error("Zero", "vector not initialized");
   else
      memset(fElements, 0, fNrows*sizeof(Double_t));
   return *this;
}

#endif

#endif

#endif
