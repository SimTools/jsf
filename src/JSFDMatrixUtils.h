
#ifndef ROOT_JSFDMatrixUtils
#define ROOT_JSFDMatrixUtils

//+SEQ,CopyRight,T=NOINCLUDE.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Matrix utility classes.                                              //
//                                                                      //
// This file defines utility classes for the Linear Algebra Package.    //
// The following classes are defined here:                              //
//   TElementAction                                                     //
//   TElementPosAction                                                  //
//   TLazyMatrix                                                        //
//   THaarMatrix                                                        //
//   TMatrixRow                                                         //
//   TMatrixColumn                                                      //
//   TMatrixDiag                                                        //
//   TMatrixPivoting                                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_JSFDMatrix
//*KEEP,TMatrix,T=C++.
#include "JSFDMatrix.h"
//*KEND.
#endif


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TElementAction                                                       //
//                                                                      //
// A class to do a specific operation on every vector or matrix element //
// (regardless of it position) as the object is being traversed.        //
// This is an abstract class. Derived classes need to implement the     //
// action function Operation().                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDElementAction {

friend class JSFDMatrix;
friend class JSFDVector;

private:
   virtual void Operation(Double_t &element) = 0;
   void operator=(const JSFDElementAction &) { }
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TElementPosAction                                                    //
//                                                                      //
// A class to do a specific operation on every vector or matrix element //
// as the object is being traversed. This is an abstract class.         //
// Derived classes need to implement the action function Operation().   //
// In the action function the location of the current element is        //
// known (fI=row, fJ=columns).                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDElementPosAction {

friend class JSFDMatrix;
friend class JSFDVector;

protected:
   Int_t fI;        // i position of element being passed to Operation()
   Int_t fJ;        // j position of element being passed to Operation()

private:
   virtual void Operation(Double_t &element) = 0;
   void operator=(const JSFDElementPosAction &) { }
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLazyMatrix                                                          //
//                                                                      //
// Class used to make a lazy copy of a matrix, i.e. only copy matrix    //
// when really needed (when accessed).                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDLazyMatrix : public TObject {

friend class JSFDMatrix;

protected:
   Int_t fRowUpb;
   Int_t fRowLwb;
   Int_t fColUpb;
   Int_t fColLwb;

private:
   virtual void FillIn(JSFDMatrix &m) const = 0;

   JSFDLazyMatrix(const JSFDLazyMatrix &) { }
   void operator=(const JSFDLazyMatrix &) { }

public:
   JSFDLazyMatrix() { fRowUpb = fRowLwb = fColUpb = fColLwb = 0; }
   JSFDLazyMatrix(Int_t nrows, Int_t ncols)
      : fRowUpb(nrows-1), fRowLwb(0), fColUpb(ncols-1), fColLwb(0) { }
   JSFDLazyMatrix(Int_t row_lwb, Int_t row_upb, Int_t col_lwb, Int_t col_upb)
      : fRowUpb(row_upb), fRowLwb(row_lwb), fColUpb(col_upb), fColLwb(col_lwb) { }

   ClassDef(JSFDLazyMatrix,1)  // Lazy matrix
};


class JSFDHaarMatrix : public JSFDLazyMatrix {

private:
   void FillIn(JSFDMatrix &m) const;

public:
   JSFDHaarMatrix(Int_t n, Int_t no_cols = 0);
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMatrixRow                                                           //
//                                                                      //
// Class represents a row of a TMatrix.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDMatrixRow : public TObject {

friend class JSFDMatrix;
friend class JSFDVector;

private:
   const JSFDMatrix  *fMatrix;  // the matrix I am a row of
   Int_t           fRowInd;  // effective row index
   Int_t           fInc;     // if ptr = @a[row,i], then ptr+inc = @a[row,i+1]
   Double_t         *fPtr;     // pointer to the a[row,0]

   JSFDMatrixRow() { fMatrix = 0; fInc = 0; fPtr = 0; }

public:
   JSFDMatrixRow(const JSFDMatrix &matrix, Int_t row);

   void operator=(Double_t val);
   void operator+=(Double_t val);
   void operator*=(Double_t val);

   void operator=(const JSFDVector &vec);

   const Double_t &operator()(Int_t i) const;
   Double_t &operator()(Int_t i);

   ClassDef(JSFDMatrixRow,1)  // One row of a matrix
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMatrixColumn                                                        //
//                                                                      //
// Class represents a column of a TMatrix.                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDMatrixColumn : public TObject {

friend class JSFDMatrix;
friend class JSFDVector;

private:
   const JSFDMatrix  *fMatrix;         // the matrix I am a column of
   Int_t           fColInd;         // effective column index
   Double_t         *fPtr;            // pointer to the a[0,i] column

   JSFDMatrixColumn() { fMatrix = 0; fPtr = 0; }

public:
   JSFDMatrixColumn(const JSFDMatrix &matrix, Int_t col);

   void operator=(Double_t val);
   void operator+=(Double_t val);
   void operator*=(Double_t val);

   void operator=(const JSFDVector &vec);

   const Double_t &operator()(Int_t i) const;
   Double_t &operator()(Int_t i);

   ClassDef(JSFDMatrixColumn,1)  // One column of a matrix
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMatrixDiag                                                          //
//                                                                      //
// Class represents the diagonal of a matrix (for easy manipulation).   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDMatrixDiag : public TObject {

friend class JSFDMatrix;
friend class JSFDVector;

private:
   const JSFDMatrix  *fMatrix;  // the matrix I am the diagonal of
   Int_t           fInc;     // if ptr=@a[i,i], then ptr+inc = @a[i+1,i+1]
   Int_t           fNdiag;   // number of diag elems, min(nrows,ncols)
   Double_t         *fPtr;     // pointer to the a[0,0]

   JSFDMatrixDiag() { fMatrix = 0; fInc = 0; fNdiag = 0; fPtr = 0; }

public:
   JSFDMatrixDiag(const JSFDMatrix &matrix);

   void operator=(Double_t val);
   void operator+=(Double_t val);
   void operator*=(Double_t val);

   void operator=(const JSFDVector &vec);

   const Double_t &operator()(Int_t i) const;
   Double_t &operator()(Int_t i);
   Int_t  GetNdiags() const { return fNdiag; }

   ClassDef(JSFDMatrixDiag,1)  // Diagonal of a matrix
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMatrixPivoting                                                      //
//                                                                      //
// This class inherits from TMatrix and it keeps additional information //
// about what is being/has been pivoted.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class JSFDMatrixPivoting : public JSFDMatrix {

private:
   typedef Double_t* Index_t;             // wanted to have typeof(index[0])
   Index_t *const fRowIndex;            // fRowIndex[i] = ptr to the i-th
                                        // matrix row, or 0 if the row
                                        // has been pivoted. Note,
                                        // pivoted columns are marked
                                        // by setting fIndex[j] to zero.

                                // Information about the pivot that was
                                // just picked up
   Double_t fPivotValue;                // Value of the pivoting element
   Index_t  fPivotRow;                  // pivot's location (ptrs)
   Index_t  fPivotCol;
   Int_t    fPivotOdd;                  // parity of the pivot
                                        // (0 for even, 1 for odd)

   void PickUpPivot();                  // Pick up a pivot from
                                        // not-pivoted rows and cols

public:
   JSFDMatrixPivoting(const JSFDMatrix &m);
   ~JSFDMatrixPivoting();

   Double_t PivotingAndElimination();   // Perform the pivoting, return
                                        // the pivot value times (-1)^(pi+pj)
                                        // (pi,pj - pivot el row & col)
};

//----- inlines ----------------------------------------------------------------

#ifndef R__HPUX

#ifndef __CINT__

inline JSFDMatrixRow::JSFDMatrixRow(const JSFDMatrix &matrix, Int_t row)
       : fMatrix(&matrix), fInc(matrix.fNrows)
{
   if (!matrix.IsValid()) {
      Error("JSFDMatrixRow", "matrix is not initialized");
      return;
   }

   fRowInd = row - matrix.fRowLwb;

   if (fRowInd >= matrix.fNrows || fRowInd < 0) {
      Error("JSFDMatrixRow", "row #%d is not within the matrix", row);
      return;
   }

   fPtr = &(matrix.fIndex[0][fRowInd]);
}

inline const Double_t &JSFDMatrixRow::operator()(Int_t i) const
{
   // Get hold of the i-th row's element.

   static Double_t err;
   err = 0.0;

   if (!fMatrix->IsValid()) {
      Error("operator()", "matrix is not initialized");
      return err;
   }

   Int_t acoln = i-fMatrix->fColLwb;           // Effective index

   if (acoln >= fMatrix->fNcols || acoln < 0) {
      Error("operator()", "JSFDMatrixRow index %d is out of row boundaries [%d,%d]",
            i, fMatrix->fColLwb, fMatrix->fNcols+fMatrix->fColLwb-1);
      return err;
   }

   return fMatrix->fIndex[acoln][fPtr-fMatrix->fElements];
}

inline Double_t &JSFDMatrixRow::operator()(Int_t i)
{
   return (Double_t&)((*(const JSFDMatrixRow *)this)(i));
}

inline JSFDMatrixColumn::JSFDMatrixColumn(const JSFDMatrix &matrix, Int_t col)
       : fMatrix(&matrix)
{
   if (!matrix.IsValid()) {
      Error("JSFDMatrixColumn", "matrix is not initialized");
      return;
   }

   fColInd = col - matrix.fColLwb;

   if (fColInd >= matrix.fNcols || fColInd < 0) {
      Error("JSFDMatrixColumn", "column #%d is not within the matrix", col);
      return;
   }

   fPtr = &(matrix.fIndex[fColInd][0]);
}

inline const Double_t &JSFDMatrixColumn::operator()(Int_t i) const
{
   // Access the i-th element of the column

   static Double_t err;
   err = 0.0;

   if (!fMatrix->IsValid()) {
      Error("operator()", "matrix is not initialized");
      return err;
   }

   Int_t arown = i-fMatrix->fRowLwb;           // Effective indices

   if (arown >= fMatrix->fNrows || arown < 0) {
      Error("operator()", "JSFDMatrixColumn index %d is out of column boundaries [%d,%d]",
            i, fMatrix->fRowLwb, fMatrix->fNrows+fMatrix->fRowLwb-1);
      return err;
   }

   return fPtr[arown];
}

inline Double_t &JSFDMatrixColumn::operator()(Int_t i)
{
   return (Double_t&)((*(const JSFDMatrixColumn *)this)(i));
}

inline JSFDMatrixDiag::JSFDMatrixDiag(const JSFDMatrix &matrix)
       : fMatrix(&matrix), fInc(matrix.fNrows+1),
         fNdiag(TMath::Min(matrix.fNrows, matrix.fNcols))
{
   if (!matrix.IsValid()) {
      Error("JSFDMatrixDiag", "matrix is not initialized");
      return;
   }
   fPtr = &(matrix.fElements[0]);
}

inline const Double_t &JSFDMatrixDiag::operator()(Int_t i) const
{
   // Get hold of the i-th diag element (indexing always starts at 0,
   // regardless of matrix' col_lwb and row_lwb)

   static Double_t err;
   err = 0.0;

   if (!fMatrix->IsValid()) {
      Error("operator()", "matrix is not initialized");
      return err;
   }

   if (i > fNdiag || i < 1) {
      Error("JSFDMatrixDiag", "JSFDMatrixDiag index %d is out of diag boundaries [1,%d]",
            i, fNdiag);
      return err;
   }

   return fMatrix->fIndex[i-1][i-1];
}

inline Double_t &JSFDMatrixDiag::operator()(Int_t i)
{
   return (Double_t&)((*(const JSFDMatrixDiag *)this)(i));
}

#endif

#endif

#endif
