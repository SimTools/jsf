//*CMZ :  1.03/09 16/12/97  16.17.48  by  Rene Brun
//*-- Author :    Fons Rademakers   05/11/97

//*KEEP,CopyRight,T=C.
/*************************************************************************
 * Copyright(c) 1995-1999, The ROOT System, All rights reserved.         *
 * Authors: Rene Brun and Fons Rademakers.                               *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/AA_LICENSE.                      *
 * For the list of contributors see $ROOTSYS/AA_CREDITS.                 *
 *************************************************************************/
//*KEND.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Matrix utility classes.                                              //
//                                                                      //
// This file defines utility classes for the Linear Algebra Package.    //
// The following classes are defined here:                              //
//   TMatrixAction                                                      //
//   TMatrixPosAction                                                   //
//   TLazyMatrix                                                        //
//   THaarMatrix                                                        //
//   TMatrixRow                                                         //
//   TMatrixColumn                                                      //
//   TMatrixDiag                                                        //
//   TMatrixPivoting                                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*KEEP,TVector,T=C++.
#include "JSFDVector.h"
//*KEEP,TMatrix,T=C++.
#include "JSFDMatrix.h"
//*KEND.


ClassImp(JSFDLazyMatrix)
ClassImp(JSFDMatrixRow)
ClassImp(JSFDMatrixColumn)
ClassImp(JSFDMatrixDiag)


//______________________________________________________________________________
void JSFDMatrixRow::operator=(Double_t val)
{
   // Assign val to every element of the matrix row.

   if (!fMatrix->IsValid()) {
      Error("operator=", "matrix not initialized");
      return;
   }

   Double_t *rp = fPtr;                    // Row ptr
   for ( ; rp < fPtr + fMatrix->fNelems; rp += fInc)
      *rp = val;
}

//______________________________________________________________________________
void JSFDMatrixRow::operator+=(Double_t val)
{
   // Add val to every element of the matrix row.

   if (!fMatrix->IsValid()) {
      Error("operator+=", "matrix not initialized");
      return;
   }

   Double_t *rp = fPtr;                    // Row ptr
   for ( ; rp < fPtr + fMatrix->fNelems; rp += fInc)
      *rp += val;
}

//______________________________________________________________________________
void JSFDMatrixRow::operator*=(Double_t val)
{
   // Multiply every element of the matrix row with val.

   if (!fMatrix->IsValid()) {
      Error("operator*=", "matrix not initialized");
      return;
   }

   Double_t *rp = fPtr;                    // Row ptr
   for ( ; rp < fPtr + fMatrix->fNelems; rp += fInc)
      *rp *= val;
}

//______________________________________________________________________________
void JSFDMatrixRow::operator=(const JSFDVector &vec)
{
   // Assign a vector to a matrix row. The vector is considered row-vector
   // to allow the assignment in the strict sense.

   if (!fMatrix->IsValid()) {
      Error("operator=", "matrix not initialized");
      return;
   }
   if (!vec.IsValid()) {
      Error("operator=", "vector not initialized");
      return;
   }

   if (fMatrix->fColLwb != vec.fRowLwb || fMatrix->fNcols != vec.fNrows) {
      Error("operator=", "transposed vector cannot be assigned to a row of the matrix");
      return;
   }

   Double_t *rp = fPtr;                          // Row ptr
   Double_t *vp = vec.fElements;                 // Vector ptr
   for ( ; rp < fPtr + fMatrix->fNelems; rp += fInc)
      *rp = *vp++;

   Assert(vp == vec.fElements + vec.fNrows);
}

//______________________________________________________________________________
void JSFDMatrixRow::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFDMatrixRow.

   if (R__b.IsReading()) {
      R__b.ReadVersion();  //Version_t R__v = R__b.ReadVersion();
      TObject::Streamer(R__b);
      R__b >> (JSFDMatrix*&)fMatrix;
      R__b >> fRowInd;
      R__b >> fInc;
      fPtr = &(fMatrix->fIndex[0][fRowInd]);
   } else {
      R__b.WriteVersion(JSFDMatrixRow::IsA());
      TObject::Streamer(R__b);
      R__b << fMatrix;
      R__b << fRowInd;
      R__b << fInc;
   }
}

//______________________________________________________________________________
void JSFDMatrixColumn::operator=(Double_t val)
{
   // Assign val to every element of the matrix column.

   if (!fMatrix->IsValid()) {
      Error("operator=", "matrix not initialized");
      return;
   }

   Double_t *cp = fPtr;                    // Column ptr
   while (cp < fPtr + fMatrix->fNrows)
      *cp++ = val;
}

//______________________________________________________________________________
void JSFDMatrixColumn::operator+=(Double_t val)
{
   // Add val to every element of the matrix column.

   if (!fMatrix->IsValid()) {
      Error("operator+=", "matrix not initialized");
      return;
   }

   Double_t *cp = fPtr;                    // Column ptr
   while (cp < fPtr + fMatrix->fNrows)
      *cp++ += val;
}

//______________________________________________________________________________
void JSFDMatrixColumn::operator*=(Double_t val)
{
   // Multiply every element of the matrix column with val.

   if (!fMatrix->IsValid()) {
      Error("operator*=", "matrix not initialized");
      return;
   }

   Double_t *cp = fPtr;                    // Column ptr
   while (cp < fPtr + fMatrix->fNrows)
      *cp++ *= val;
}

//______________________________________________________________________________
void JSFDMatrixColumn::operator=(const JSFDVector &vec)
{
   // Assign a vector to a matrix column.

   if (!fMatrix->IsValid()) {
      Error("operator=", "matrix not initialized");
      return;
   }
   if (!vec.IsValid()) {
      Error("operator=", "vector not initialized");
      return;
   }

   if (fMatrix->fRowLwb != vec.fRowLwb || fMatrix->fNrows != vec.fNrows) {
      Error("operator=", "vector cannot be assigned to a column of the matrix");
      return;
   }

   Double_t *cp = fPtr;                       // Column ptr
   Double_t *vp = vec.fElements;              // Vector ptr
   while (cp < fPtr + fMatrix->fNrows)
      *cp++ = *vp++;

   Assert(vp == vec.fElements + vec.fNrows);
}

//______________________________________________________________________________
void JSFDMatrixColumn::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFDMatrixColumn.

   if (R__b.IsReading()) {
      R__b.ReadVersion();  //Version_t R__v = R__b.ReadVersion();
      TObject::Streamer(R__b);
      R__b >> (JSFDMatrix*&)fMatrix;
      R__b >> fColInd;
      fPtr = &(fMatrix->fIndex[fColInd][0]);
   } else {
      R__b.WriteVersion(JSFDMatrixColumn::IsA());
      TObject::Streamer(R__b);
      R__b << fMatrix;
      R__b << fColInd;
   }
}

//______________________________________________________________________________
void JSFDMatrixDiag::operator=(Double_t val)
{
   // Assign val to every element of the matrix diagonal.

   if (!fMatrix->IsValid()) {
      Error("operator=", "matrix not initialized");
      return;
   }

   Double_t *dp = fPtr;                // Diag ptr
   Int_t i;
   for (i = 0; i < fNdiag; i++, dp += fInc)
      *dp = val;
}

//______________________________________________________________________________
void JSFDMatrixDiag::operator+=(Double_t val)
{
   // Assign val to every element of the matrix diagonal.

   if (!fMatrix->IsValid()) {
      Error("operator+=", "matrix not initialized");
      return;
   }

   Double_t *dp = fPtr;                // Diag ptr
   Int_t i;
   for (i = 0; i < fNdiag; i++, dp += fInc)
      *dp += val;
}

//______________________________________________________________________________
void JSFDMatrixDiag::operator*=(Double_t val)
{
   // Multiply every element of the matrix diagonal with val.

   if (!fMatrix->IsValid()) {
      Error("operator*=", "matrix not initialized");
      return;
   }

   Double_t *dp = fPtr;                // Diag ptr
   Int_t i;
   for (i = 0; i < fNdiag; i++, dp += fInc)
      *dp *= val;
}

//______________________________________________________________________________
void JSFDMatrixDiag::operator=(const JSFDVector &vec)
{
   // Assign a vector to the matrix diagonal.

   if (!fMatrix->IsValid()) {
      Error("operator=", "matrix not initialized");
      return;
   }
   if (!vec.IsValid()) {
      Error("operator=", "vector not initialized");
      return;
   }

   if (fNdiag != vec.fNrows) {
      Error("operator=", "vector cannot be assigned to the matrix diagonal");
      return;
   }

   Double_t *dp = fPtr;                       // Diag ptr
   Double_t *vp = vec.fElements;              // Vector ptr
   for ( ; vp < vec.fElements + vec.fNrows; dp += fInc)
      *dp = *vp++;

   Assert(dp < fPtr + fMatrix->fNelems + fInc);
}

//______________________________________________________________________________
void JSFDMatrixDiag::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFDMatrixDiag.

   if (R__b.IsReading()) {
      R__b.ReadVersion();  //Version_t R__v = R__b.ReadVersion();
      TObject::Streamer(R__b);
      R__b >> (JSFDMatrix*&)fMatrix;
      R__b >> fInc;
      R__b >> fNdiag;
      fPtr = &(fMatrix->fElements[0]);
   } else {
      R__b.WriteVersion(JSFDMatrixDiag::IsA());
      TObject::Streamer(R__b);
      R__b << fMatrix;
      R__b << fInc;
      R__b << fNdiag;
   }
}

//______________________________________________________________________________
JSFDMatrixPivoting::JSFDMatrixPivoting(const JSFDMatrix &m)
    : JSFDMatrix(m), fRowIndex(new Index_t[fNrows]), fPivotRow(0), fPivotCol(0)
{
   Assert(fRowIndex != 0);

   Index_t rp = fElements;               // Fill in the row_index
   for (Index_t *rip = fRowIndex; rip < fRowIndex+fNrows; )
      *rip++ = rp++;
}

//______________________________________________________________________________
JSFDMatrixPivoting::~JSFDMatrixPivoting()
{
   delete [] fRowIndex;
}

//______________________________________________________________________________
void JSFDMatrixPivoting::PickUpPivot()
{
   // Pick up a pivot, an element with the largest abs value from yet
   // not-pivoted rows and cols

   Double_t max_elem = -1;                // Abs value of the largest element
   Index_t *prpp = 0;                   // Position of the pivot in fRowIndex
   Index_t *pcpp = 0;                   // Position of the pivot in fIndex

   Int_t col_odd = 0;                   // Parity of the current column

   for (Index_t *cpp = fIndex; cpp < fIndex + fNcols; cpp++) {
      const Double_t *cp = *cpp;          // Column pointer for the curr col
      if (cp == 0)                      // skip over already pivoted col
         continue;
      Int_t row_odd = 0;                // Parity of the current row
      for (Index_t *rip = fRowIndex; rip < fRowIndex + fNrows; rip++, cp++)
         if (*rip) {                    // only if the row hasn't been pivoted
            const Double_t v = *cp;
            if (TMath::Abs(v) > max_elem) {
               max_elem = TMath::Abs(v); // Note the local max of col elements
               fPivotValue = v;
               prpp = rip;
               pcpp = cpp;
               fPivotOdd = row_odd ^ col_odd;
            }
            row_odd ^= 1;                // Toggle parity for the next row
         }
      col_odd ^= 1;
   }

   if (max_elem < 0 || prpp == 0 || pcpp == 0)
      Error("PickUpPivot", "all the rows and columns have been already pivoted and eliminated");

   fPivotRow = *prpp, *prpp = 0;
   fPivotCol = *pcpp, *pcpp = 0;
}

//______________________________________________________________________________
Double_t JSFDMatrixPivoting::PivotingAndElimination()
{
   // Perform pivoting and gaussian elemination, return the pivot value
   // times pivot parity. The procedure places zeros to the fPivotRow of
   // all not yet pivoted columns. A[i,j] -= A[i,pivot_col]/pivot*A[pivot_row,j]

   PickUpPivot();

   if (fPivotValue == 0)
      return 0;

   Assert(fPivotRow != 0 && fPivotCol != 0);

   Double_t *pcp;                         // Pivot column pointer
   const Index_t *rip;                  // Current ptr in row_index
                                        // Divide the pivoted column by pivot
   for (pcp = fPivotCol, rip = fRowIndex; rip < fRowIndex+fNrows; pcp++, rip++)
     if (*rip)                          // Skip already pivoted rows
        *pcp /= fPivotValue;

   // Eliminate all the elements from the pivot_row in all not pivoted columns
   const Double_t *prp = fPivotRow;
   for (const Index_t *cpp = fIndex; cpp < fIndex + fNcols; cpp++, prp += fNrows) {
      Double_t *cp = *cpp;                // A[*,j]
      if (cp == 0)                      // skip over already pivoted col
         continue;
      const Double_t fac = *prp;        // fac = A[pivot_row,j]
                                        // Do elimination stepping over pivoted rows
      for (pcp = fPivotCol, rip = fRowIndex; rip < fRowIndex+fNrows; pcp++, cp++, rip++)
         if (*rip)
            *cp -= *pcp * fac;
   }

   return fPivotOdd ? -fPivotValue : fPivotValue;
}

//______________________________________________________________________________
void MakeHaarMatrix(JSFDMatrix &m)
{
   // Create an orthonormal (2^n)*(no_cols) Haar (sub)matrix, whose columns
   // are Haar functions. If no_cols is 0, create the complete matrix with
   // 2^n columns. Example, the complete Haar matrix of the second order is:
   // column 1: [ 1  1  1  1]/2
   // column 2: [ 1  1 -1 -1]/2
   // column 3: [ 1 -1  0  0]/sqrt(2)
   // column 4: [ 0  0  1 -1]/sqrt(2)
   // Matrix m is assumed to be zero originally.

   if (!m.IsValid()) {
      Error("MakeHaarMatrix", "matrix not initialized");
      return;
   }

   if (m.fNcols > m.fNrows || m.fNcols <= 0) {
      Error("MakeHaarMatrix", "matrix not right shape");
      return;
   }

   Double_t *cp = m.fElements;
   Double_t *m_end = m.fElements + (m.fNrows*m.fNcols);
   Int_t i;

   Double_t norm_factor = 1/TMath::Sqrt((Double_t)m.fNrows);

   // First column is always 1 (up to normalization)
   for ( i = 0; i < m.fNrows; i++)
      *cp++ = norm_factor;

   // The other functions are kind of steps: stretch of 1 followed by the
   // equally long stretch of -1. The functions can be grouped in families
   // according to their order (step size), differing only in the location
   // of the step
   Int_t step_length = m.fNrows/2;
   while (cp < m_end && step_length > 0) {
      for (Int_t step_position = 0; cp < m_end && step_position < m.fNrows;
           step_position += 2*step_length, cp += m.fNrows) {
         Double_t *ccp = cp + step_position;
         for (i = 0; i < step_length; i++)
            *ccp++ = norm_factor;
         for (i = 0; i < step_length; i++)
            *ccp++ = -norm_factor;
      }
      step_length /= 2;
      norm_factor *= TMath::Sqrt(2.0);
   }

   Assert(step_length != 0 || cp == m_end);
   Assert(m.fNrows != m.fNcols || step_length == 0);
}

//______________________________________________________________________________
JSFDHaarMatrix::JSFDHaarMatrix(Int_t order, Int_t no_cols)
    : JSFDLazyMatrix(1<<order, no_cols == 0 ? 1<<order : no_cols)
{
   Assert(order > 0 && no_cols >= 0);
}

//______________________________________________________________________________
void JSFDHaarMatrix::FillIn(JSFDMatrix &m) const
{
   MakeHaarMatrix(m);
}


#ifdef R__HPUX

//______________________________________________________________________________
//  These functions should be inline
//______________________________________________________________________________

JSFDMatrixRow::JSFDMatrixRow(const JSFDMatrix &matrix, Int_t row)
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

const Double_t &JSFDMatrixRow::operator()(Int_t i) const
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

Double_t &JSFDMatrixRow::operator()(Int_t i)
{
   return (Double_t&)((*(const JSFDMatrixRow *)this)(i));
}

JSFDMatrixColumn::JSFDMatrixColumn(const JSFDMatrix &matrix, Int_t col)
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

const Double_t &JSFDMatrixColumn::operator()(Int_t i) const
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

Double_t &JSFDMatrixColumn::operator()(Int_t i)
{
   return (Double_t&)((*(const JSFDMatrixColumn *)this)(i));
}

JSFDMatrixDiag::JSFDMatrixDiag(const JSFDMatrix &matrix)
       : fMatrix(&matrix), fInc(matrix.fNrows+1),
         fNdiag(TMath::Min(matrix.fNrows, matrix.fNcols))
{
   if (!matrix.IsValid()) {
      Error("JSFDMatrixDiag", "matrix is not initialized");
      return;
   }
   fPtr = &(matrix.fElements[0]);
}

const Double_t &JSFDMatrixDiag::operator()(Int_t i) const
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

Double_t &JSFDMatrixDiag::operator()(Int_t i)
{
   return (Double_t&)((*(const JSFDMatrixDiag *)this)(i));
}

#endif