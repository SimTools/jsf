
#ifndef ROOT_JSFDMatrix
#define ROOT_JSFDMatrix

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

#ifndef ROOT_JSFDVector
//*KEEP,TVector,T=C++.
#include "JSFDVector.h"
//*KEND.
#endif

class JSFDLazyMatrix;



class JSFDMatrix : public TObject {

friend class JSFDVector;
friend class JSFDMatrixRow;
friend class JSFDMatrixColumn;
friend class JSFDMatrixDiag;
friend class JSFDMatrixPivoting;

protected:
   Int_t     fNrows;            // number of rows
   Int_t     fNcols;            // number of columns
   Int_t     fNelems;           // number of elements in matrix
   Int_t     fRowLwb;           // lower bound of the row index
   Int_t     fColLwb;           // lower bound of the col index
   Double_t   *fElements;	        // elements themselves
   Double_t  **fIndex;            // index[i] = &matrix(0,i) (col index)

   void Allocate(Int_t nrows, Int_t ncols, Int_t row_lwb = 0, Int_t col_lwb = 0);
   void Invalidate() { fNrows = fNcols = fNelems = -1; fElements = 0; fIndex = 0; }

   // Elementary constructors
   void Transpose(const JSFDMatrix &m);
   void Invert(const JSFDMatrix &m);
   void AMultB(const JSFDMatrix &a, const JSFDMatrix &b);
   void AtMultB(const JSFDMatrix &a, const JSFDMatrix &b);

   friend void MakeHaarMatrix(JSFDMatrix &m);

public:
   enum EMatrixCreatorsOp1 { kZero, kUnit, kTransposed, kInverted };
   enum EMatrixCreatorsOp2 { kMult, kTransposeMult, kInvMult, kAtBA };

   JSFDMatrix() { Invalidate(); }
   JSFDMatrix(Int_t nrows, Int_t ncols);
   JSFDMatrix(Int_t row_lwb, Int_t row_upb, Int_t col_lwb, Int_t col_upb);
   JSFDMatrix(const JSFDMatrix &another);
   JSFDMatrix(EMatrixCreatorsOp1 op, const JSFDMatrix &prototype);
   JSFDMatrix(const JSFDMatrix &a, EMatrixCreatorsOp2 op, const JSFDMatrix &b);
   JSFDMatrix(const JSFDLazyMatrix &lazy_constructor);

   virtual ~JSFDMatrix();

   void ResizeTo(Int_t nrows, Int_t ncols);
   void ResizeTo(Int_t row_lwb, Int_t row_upb, Int_t col_lwb, Int_t col_upb);
   void ResizeTo(const JSFDMatrix &m);

   Bool_t IsValid() const;

   Int_t GetRowLwb() const     { return fRowLwb; }
   Int_t GetRowUpb() const     { return fNrows+fRowLwb-1; }
   Int_t GetNrows() const      { return fNrows; }
   Int_t GetColLwb() const     { return fColLwb; }
   Int_t GetColUpb() const     { return fNcols+fColLwb-1; }
   Int_t GetNcols() const      { return fNcols; }
   Int_t GetNoElements() const { return fNelems; }

   const Double_t &operator()(Int_t rown, Int_t coln) const;
   Double_t &operator()(Int_t rown, Int_t coln);

   JSFDMatrix &operator=(const JSFDMatrix &source);
   JSFDMatrix &operator=(const JSFDLazyMatrix &source);
   JSFDMatrix &operator=(Double_t val);
   JSFDMatrix &operator-=(Double_t val);
   JSFDMatrix &operator+=(Double_t val);
   JSFDMatrix &operator*=(Double_t val);

   Bool_t operator==(Double_t val) const;
   Bool_t operator!=(Double_t val) const;
   Bool_t operator<(Double_t val) const;
   Bool_t operator<=(Double_t val) const;
   Bool_t operator>(Double_t val) const;
   Bool_t operator>=(Double_t val) const;

   JSFDMatrix &Zero();
   JSFDMatrix &Abs();
   JSFDMatrix &Sqr();
   JSFDMatrix &Sqrt();

   JSFDMatrix &Apply(JSFDElementAction &action);
   JSFDMatrix &Apply(JSFDElementPosAction &action);

   JSFDMatrix &Invert(Double_t *determ_ptr = 0);

   JSFDMatrix &UnitMatrix();
   JSFDMatrix &HilbertMatrix();

   JSFDMatrix &operator*=(const JSFDMatrix &source);
   JSFDMatrix &operator*=(const JSFDMatrixDiag &diag);

   void Mult(const JSFDMatrix &a, const JSFDMatrix &b);

   Double_t RowNorm() const;
   Double_t NormInf() const { return RowNorm(); }
   Double_t ColNorm() const;
   Double_t Norm1() const { return ColNorm(); }
   Double_t E2Norm() const;

   Double_t Determinant() const;

   void Print(Option_t *option="");

   friend JSFDMatrix &operator+=(JSFDMatrix &target, const JSFDMatrix &source);
   friend JSFDMatrix &operator-=(JSFDMatrix &target, const JSFDMatrix &source);
   friend JSFDMatrix &Add(JSFDMatrix &target, Double_t scalar, const JSFDMatrix &source);
   friend JSFDMatrix &ElementMult(JSFDMatrix &target, const JSFDMatrix &source);
   friend JSFDMatrix &ElementDiv(JSFDMatrix &target, const JSFDMatrix &source);

   friend Bool_t operator==(const JSFDMatrix &im1, const JSFDMatrix &im2);
   friend void Compare(const JSFDMatrix &im1, const JSFDMatrix &im2);
   friend Bool_t AreCompatible(const JSFDMatrix &im1, const JSFDMatrix &im2);
   friend Double_t E2Norm(const JSFDMatrix &m1, const JSFDMatrix &m2);

   ClassDef(JSFDMatrix,1)  // Matrix class
};


// Service functions (useful in the verification code).
// They print some detail info if the validation condition fails
void VerifyElementValue(const JSFDMatrix &m, Double_t val);
void VerifyMatrixIdentity(const JSFDMatrix &m1, const JSFDMatrix &m2);


#ifndef R__HPUX
inline Bool_t JSFDMatrix::IsValid() const
   { if (fNrows == -1) return kFALSE; return kTRUE; }
#endif

#ifndef ROOT_JSFDMatrixUtils
//*KEEP,TMatrixUtils,T=C++.
#include "JSFDMatrixUtils.h"
//*KEND.
#endif


//----- inlines ----------------------------------------------------------------

#ifndef R__HPUX

#ifndef __CINT__

inline JSFDMatrix::JSFDMatrix(Int_t no_rows, Int_t no_cols)
{
   Allocate(no_rows, no_cols);
}

inline JSFDMatrix::JSFDMatrix(Int_t row_lwb, Int_t row_upb, Int_t col_lwb, Int_t col_upb)
{
   Allocate(row_upb-row_lwb+1, col_upb-col_lwb+1, row_lwb, col_lwb);
}

inline JSFDMatrix::JSFDMatrix(const JSFDLazyMatrix &lazy_constructor)
{
   Allocate(lazy_constructor.fRowUpb-lazy_constructor.fRowLwb+1,
            lazy_constructor.fColUpb-lazy_constructor.fColLwb+1,
            lazy_constructor.fRowLwb, lazy_constructor.fColLwb);
  lazy_constructor.FillIn(*this);
}

inline JSFDMatrix &JSFDMatrix::operator=(const JSFDLazyMatrix &lazy_constructor)
{
   if (!IsValid()) {
      Error("operator=(const JSFDLazyMatrix&)", "matrix is not initialized");
      return *this;
   }
   if (lazy_constructor.fRowUpb != GetRowUpb() ||
       lazy_constructor.fColUpb != GetColUpb() ||
       lazy_constructor.fRowLwb != GetRowLwb() ||
       lazy_constructor.fColLwb != GetColLwb()) {
      Error("operator=(const JSFDLazyMatrix&)", "matrix is incompatible with "
            "the assigned Lazy matrix");
      return *this;
   }

   lazy_constructor.FillIn(*this);
   return *this;
}

inline Bool_t AreCompatible(const JSFDMatrix &im1, const JSFDMatrix &im2)
{
   if (!im1.IsValid()) {
      ::Error("AreCompatible", "matrix 1 not initialized");
      return kFALSE;
   }
   if (!im2.IsValid()) {
      ::Error("AreCompatible", "matrix 2 not initialized");
      return kFALSE;
   }

   if (im1.fNrows  != im2.fNrows  || im1.fNcols  != im2.fNcols ||
       im1.fRowLwb != im2.fRowLwb || im1.fColLwb != im2.fColLwb)
      return kFALSE;

   return kTRUE;
}

inline JSFDMatrix &JSFDMatrix::operator=(const JSFDMatrix &source)
{
   if (this != &source && AreCompatible(*this, source)) {
      TObject::operator=(source);
      memcpy(fElements, source.fElements, fNelems*sizeof(Double_t));
   }
   return *this;
}

inline JSFDMatrix::JSFDMatrix(const JSFDMatrix &another)
{
   if (another.IsValid()) {
      Allocate(another.fNrows, another.fNcols, another.fRowLwb, another.fColLwb);
      *this = another;
   } else
      Error("JSFDMatrix(const JSFDMatrix&)", "other matrix is not valid");
}

inline void JSFDMatrix::ResizeTo(const JSFDMatrix &m)
{
   ResizeTo(m.GetRowLwb(), m.GetRowUpb(), m.GetColLwb(), m.GetColUpb());
}

inline const Double_t &JSFDMatrix::operator()(int rown, int coln) const
{
   static Double_t err;
   err = 0.0;

   if (!IsValid()) {
      Error("operator()", "matrix is not initialized");
      return err;
   }

   Int_t arown = rown - fRowLwb;          // Effective indices
   Int_t acoln = coln - fColLwb;

   if (arown >= fNrows || arown < 0) {
      Error("operator()", "row index %d is out of matrix boundaries [%d,%d]",
            rown, fRowLwb, fNrows+fRowLwb-1);
      return err;
   }
   if (acoln >= fNcols || acoln < 0) {
      Error("operator()", "col index %d is out of matrix boundaries [%d,%d]",
            coln, fColLwb, fNcols+fColLwb-1);
      return err;
   }

   return (fIndex[acoln])[arown];
}

inline Double_t &JSFDMatrix::operator()(Int_t rown, Int_t coln)
{
   return (Double_t&)((*(const JSFDMatrix *)this)(rown,coln));
}

inline JSFDMatrix &JSFDMatrix::Zero()
{
   if (!IsValid())
      Error("Zero", "matrix not initialized");
   else
      memset(fElements, 0, fNelems*sizeof(Double_t));
   return *this;
}

inline JSFDMatrix &JSFDMatrix::Apply(JSFDElementAction &action)
{
   if (!IsValid())
      Error("Apply(JSFDElementAction&)", "matrix not initialized");
   else
      for (Double_t *ep = fElements; ep < fElements+fNelems; ep++)
         action.Operation(*ep);
   return *this;
}

#endif

#endif

#endif
