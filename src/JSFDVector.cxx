//*CMZ :  2.23/11 12/01/2000  11.38.25by  Fons Rademakers
//*CMZ :  2.23/10 30/11/99  14.12.52  by  Fons Rademakers
//*CMZ :  2.23/04 05/10/99  15.17.15  by  Fons Rademakers
//*CMZ :  2.20/00 13/11/98  18.44.30  by  Fons Rademakers
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

//*KEEP,TMatrix,T=C++.
#include "JSFDMatrix.h"
//*KEND.


ClassImp(JSFDVector)

//______________________________________________________________________________
void JSFDVector::Allocate(Int_t nrows, Int_t row_lwb)
{
   // Allocate new vector. Arguments are number of rows and row
   // lowerbound (0 default).

   Invalidate();

   if (nrows <= 0) {
      Error("Allocate", "no of rows has to be positive");
      return;
   }

   fNrows  = nrows;
   fNmem   = nrows;
   fRowLwb = row_lwb;

   fElements = new Double_t[fNrows];
   if (fElements)
      memset(fElements, 0, fNrows*sizeof(Double_t));
}

//______________________________________________________________________________
JSFDVector::JSFDVector(Int_t lwb, Int_t upb, Double_t va_(iv1), ...)
{
   // Make a vector and assign initial values. Argument list should contain
   // Double_t values to assign to vector elements. The list must be
   // terminated by the string "END". Example:
   // JSFDVector foo(1,3,0.0,1.0,1.5,"END");

   va_list args;
   va_start(args,va_(iv1));             // Init 'args' to the beginning of
                                        // the variable length list of args

   Allocate(upb-lwb+1, lwb);

   Int_t i;
   (*this)(lwb) = iv1;
   for (i = lwb+1; i <= upb; i++)
      (*this)(i) = (Double_t)va_arg(args,Double_t);

   if (strcmp((char *)va_arg(args,char *),"END"))
      Error("JSFDVector(Int_t, Int_t, ...)", "argument list must be terminated by \"END\"");

   va_end(args);
}

//______________________________________________________________________________
JSFDVector::~JSFDVector()
{
   // JSFDVector destructor.

   if (IsValid())
      delete [] fElements;

   Invalidate();
}

//______________________________________________________________________________
void JSFDVector::ResizeTo(Int_t lwb, Int_t upb)
{
   // Resize the vector for a specified number of elements, trying to keep
   // intact as many elements of the old vector as possible. If the vector is
   // expanded, the new elements will be zeroes.

   if (upb-lwb+1 <= 0) {
      Error("ResizeTo", "can't resize vector to a non-positive number of elems");
      return;
   }

   if (!IsValid()) {
      Allocate(upb-lwb+1, lwb);
      return;
   }

   const Int_t old_nrows = fNrows;
   fNrows  = upb-lwb+1;
   fRowLwb = lwb;

   if (old_nrows == fNrows)
      return;                       // The same number of elems

   // If the vector is to grow, reallocate and clear the newly added elements
   if (fNrows > old_nrows) {
      fElements = (Double_t *)TStorage::ReAlloc(fElements, fNrows*sizeof(Double_t),
                                              fNmem*sizeof(Double_t));
      memset(fElements+old_nrows, 0, (fNrows-old_nrows)*sizeof(Double_t));
      fNmem = fNrows;
   } else if (old_nrows - fNrows < (old_nrows>>3)) {
      // Vector is to shrink a lot (more than 7/8 of the original size), reallocate
      fElements = (Double_t *)TStorage::ReAlloc(fElements, fNrows*sizeof(Double_t));
      fNmem = fNrows;
   }

   // If the vector shrinks only a little, don't bother to reallocate

   Assert(fElements != 0);
}

//______________________________________________________________________________
Double_t JSFDVector::Norm1() const
{
   // Compute the 1-norm of the vector SUM{ |v[i]| }.

   if (!IsValid()) {
      Error("Norm1", "vector is not initialized");
      return 0.0;
   }

   Double_t norm = 0;
   Double_t *vp;

   for (vp = fElements; vp < fElements + fNrows; )
      norm += TMath::Abs(*vp++);

   return norm;
}

//______________________________________________________________________________
Double_t JSFDVector::Norm2Sqr() const
{
   // Compute the square of the 2-norm SUM{ v[i]^2 }.

   if (!IsValid()) {
      Error("Norm2Sqr", "vector is not initialized");
      return 0.0;
   }

   Double_t norm = 0;
   Double_t *vp;

   for (vp = fElements; vp < fElements + fNrows; vp++)
      norm += (*vp) * (*vp);

   return norm;
}

//______________________________________________________________________________
Double_t JSFDVector::NormInf() const
{
   // Compute the infinity-norm of the vector MAX{ |v[i]| }.

   if (!IsValid()) {
      Error("NormInf", "vector is not initialized");
      return 0.0;
   }

   Double_t norm = 0;
   Double_t *vp;

   for (vp = fElements; vp < fElements + fNrows; )
      norm = TMath::Max(norm, (Double_t)TMath::Abs(*vp++));

   return norm;
}

//______________________________________________________________________________
Double_t operator*(const JSFDVector &v1, const JSFDVector &v2)
{
   // Compute the scalar product.

   if (!AreCompatible(v1,v2))
      return 0.0;

   Double_t *v1p = v1.fElements;
   Double_t *v2p = v2.fElements;
   Double_t sum = 0.0;

   while (v1p < v1.fElements + v1.fNrows)
      sum += *v1p++ * *v2p++;

   return sum;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator*=(Double_t val)
{
   // Multiply every element of the vector with val.

   if (!IsValid()) {
      Error("operator*=", "vector not initialized");
      return *this;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      *ep++ *= val;

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator*=(const JSFDMatrix &a)
{
   // "Inplace" multiplication target = A*target. A needn't be a square one
   // (the target will be resized to fit).

   if (!a.IsValid()) {
      Error("operator*=(const JSFDMatrix&)", "matrix a is not initialized");
      return *this;
   }
   if (!IsValid()) {
      Error("operator*=(const JSFDMatrix&)", "vector is not initialized");
      return *this;
   }

   if (a.fNcols != fNrows || a.fColLwb != fRowLwb) {
      Error("operator*=(const JSFDMatrix&)", "matrix and vector cannot be multiplied");
      return *this;
   }

   const Int_t old_nrows = fNrows;
   Double_t *old_vector = fElements;        // Save the old vector elem
   fRowLwb = a.fRowLwb;
   Assert((fNrows = a.fNrows) > 0);

   Assert((fElements = new Double_t[fNrows]) != 0);
   fNmem = fNrows;

   Double_t *tp = fElements;                     // Target vector ptr
   Double_t *mp = a.fElements;                   // Matrix row ptr
   while (tp < fElements + fNrows) {
      Double_t sum = 0;
      for (const Double_t *sp = old_vector; sp < old_vector + old_nrows; )
         sum += *sp++ * *mp, mp += a.fNrows;
      *tp++ = sum;
      mp -= a.fNelems - 1;       // mp points to the beginning of the next row
   }
   Assert(mp == a.fElements + a.fNrows);

   delete [] old_vector;
   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator=(Double_t val)
{
   // Assign val to every element of the vector.

   if (!IsValid()) {
      Error("operator=", "vector not initialized");
      return *this;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      *ep++ = val;

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator=(const JSFDMatrixRow &mr)
{
   // Assign a matrix row to a vector. The matrix row is implicitly transposed
   // to allow the assignment in the strict sense.

   if (!IsValid()) {
      Error("operator=(const JSFDMatrixRow&)", "vector is not initialized");
      return *this;
   }
   if (!mr.fMatrix->IsValid()) {
      Error("operator=(const JSFDMatrixRow&)", "matrix is not initialized");
      return *this;
   }

   if (mr.fMatrix->fColLwb != fRowLwb || mr.fMatrix->fNcols != fNrows) {
      Error("operator=(const JSFDMatrixRow&)", "can't assign the transposed row of the matrix to the vector");
      return *this;
   }

   Double_t *rp = mr.fPtr;                       // Row ptr
   Double_t *vp = fElements;                     // Vector ptr
   for ( ; vp < fElements + fNrows; rp += mr.fInc)
      *vp++ = *rp;

   Assert(rp == mr.fPtr + mr.fMatrix->fNelems);

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator=(const JSFDMatrixColumn &mc)
{
   // Assign a matrix column to a vector.

   if (!IsValid()) {
      Error("operator=(const JSFDMatrixColumn&)", "vector is not initialized");
      return *this;
   }
   if (!mc.fMatrix->IsValid()) {
      Error("operator=(const JSFDMatrixColumn&)", "matrix is not initialized");
      return *this;
   }

   if (mc.fMatrix->fRowLwb != fRowLwb || mc.fMatrix->fNrows != fNrows) {
      Error("operator=(const JSFDMatrixColumn&)", "can't assign a column of the matrix to the vector");
      return *this;
   }

   Double_t *cp = mc.fPtr;                   // Column ptr
   Double_t *vp = fElements;                 // Vector ptr
   while (vp < fElements + fNrows)
      *vp++ = *cp++;

   Assert(cp == mc.fPtr + mc.fMatrix->fNrows);

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator=(const JSFDMatrixDiag &md)
{
   // Assign the matrix diagonal to a vector.

   if (!IsValid()) {
      Error("operator=(const JSFDMatrixDiag&)", "vector is not initialized");
      return *this;
   }
   if (!md.fMatrix->IsValid()) {
      Error("operator=(const JSFDMatrixDiag&)", "matrix is not initialized");
      return *this;
   }

   if (md.fNdiag != fNrows) {
      Error("operator=(const JSFDMatrixDiag&)", "can't assign the diagonal of the matrix to the vector");
      return *this;
   }

   Double_t *dp = md.fPtr;                  // Diag ptr
   Double_t *vp = fElements;                // Vector ptr
   for ( ; vp < fElements + fNrows; dp += md.fInc)
      *vp++ = *dp;

   Assert(dp < md.fPtr + md.fMatrix->fNelems + md.fInc);

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator+=(Double_t val)
{
   // Add val to every element of the vector.

   if (!IsValid()) {
      Error("operator+=", "vector not initialized");
      return *this;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      *ep++ += val;

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::operator-=(Double_t val)
{
   // Subtract val from every element of the vector.

   if (!IsValid()) {
      Error("operator-=", "vector not initialized");
      return *this;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      *ep++ -= val;

   return *this;
}

//______________________________________________________________________________
Bool_t JSFDVector::operator==(Double_t val) const
{
   // Are all vector elements equal to val?

   if (!IsValid()) {
      Error("operator==", "vector not initialized");
      return kFALSE;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      if (!(*ep++ == val))
         return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t JSFDVector::operator!=(Double_t val) const
{
   // Are all vector elements not equal to val?

   if (!IsValid()) {
      Error("operator!=", "vector not initialized");
      return kFALSE;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      if (!(*ep++ != val))
         return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t JSFDVector::operator<(Double_t val) const
{
   // Are all vector elements < val?

   if (!IsValid()) {
      Error("operator<", "vector not initialized");
      return kFALSE;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      if (!(*ep++ < val))
         return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t JSFDVector::operator<=(Double_t val) const
{
   // Are all vector elements <= val?

   if (!IsValid()) {
      Error("operator<=", "vector not initialized");
      return kFALSE;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      if (!(*ep++ <= val))
         return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t JSFDVector::operator>(Double_t val) const
{
   // Are all vector elements > val?

   if (!IsValid()) {
      Error("operator>", "vector not initialized");
      return kFALSE;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      if (!(*ep++ > val))
         return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t JSFDVector::operator>=(Double_t val) const
{
   // Are all vector elements >= val?

   if (!IsValid()) {
      Error("operator>=", "vector not initialized");
      return kFALSE;
   }

   Double_t *ep = fElements;
   while (ep < fElements+fNrows)
      if (!(*ep++ >= val))
         return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::Abs()
{
   // Take an absolute value of a vector, i.e. apply Abs() to each element.

   if (!IsValid()) {
      Error("Abs", "vector not initialized");
      return *this;
   }

   Double_t *ep;
   for (ep = fElements; ep < fElements+fNrows; ep++)
      *ep = TMath::Abs(*ep);

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::Sqr()
{
   // Square each element of the vector.

   if (!IsValid()) {
      Error("Sqr", "vector not initialized");
      return *this;
   }

   Double_t *ep;
   for (ep = fElements; ep < fElements+fNrows; ep++)
      *ep = (*ep) * (*ep);

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::Sqrt()
{
   // Take square root of all elements.

   if (!IsValid()) {
      Error("Sqrt", "vector not initialized");
      return *this;
   }

   Double_t *ep;
   for (ep = fElements; ep < fElements+fNrows; ep++)
      if (*ep >= 0)
         *ep = TMath::Sqrt(*ep);
      else
         Error("Sqrt", "(%d)-th element, %g, is negative, can't take the square root",
               (ep-fElements) + fRowLwb, *ep);

   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::Apply(JSFDElementAction &action)
{
   // Apply action to each element of the vector.

   if (!IsValid())
      Error("Apply(JSFDElementAction&)", "vector not initialized");
   else
      for (Double_t *ep = fElements; ep < fElements+fNrows; ep++)
         action.Operation(*ep);
   return *this;
}

//______________________________________________________________________________
JSFDVector &JSFDVector::Apply(JSFDElementPosAction &action)
{
   // Apply action to each element of the vector. In action the location
   // of the current element is known.

   if (!IsValid()) {
      Error("Apply(JSFDElementPosAction&)", "vector not initialized");
      return *this;
   }

   Double_t *ep = fElements;
   for (action.fI = fRowLwb; action.fI < fRowLwb+fNrows; action.fI++)
      action.Operation(*ep++);

   Assert(ep == fElements+fNrows);

   return *this;
}

//______________________________________________________________________________
Bool_t operator==(const JSFDVector &v1, const JSFDVector &v2)
{
   // Check to see if two vectors are identical.

   if (!AreCompatible(v1, v2)) return kFALSE;
   return (memcmp(v1.fElements, v2.fElements, v1.fNrows*sizeof(Double_t)) == 0);
}

//______________________________________________________________________________
JSFDVector &operator+=(JSFDVector &target, const JSFDVector &source)
{
   // Add the source vector to the target vector.

   if (!AreCompatible(target, source)) {
      Error("operator+=", "vectors are not compatible");
      return target;
   }

   Double_t *sp = source.fElements;
   Double_t *tp = target.fElements;
   for ( ; tp < target.fElements+target.fNrows; )
      *tp++ += *sp++;

   return target;
}

//______________________________________________________________________________
JSFDVector &operator-=(JSFDVector &target, const JSFDVector &source)
{
   // Subtract the source vector from the target vector.

   if (!AreCompatible(target, source)) {
      Error("operator-=", "vectors are not compatible");
      return target;
   }

   Double_t *sp = source.fElements;
   Double_t *tp = target.fElements;
   for ( ; tp < target.fElements+target.fNrows; )
      *tp++ -= *sp++;

   return target;
}

//______________________________________________________________________________
JSFDVector &Add(JSFDVector &target, Double_t scalar, const JSFDVector &source)
{
   // Modify addition: target += scalar * source.

   if (!AreCompatible(target, source)) {
      Error("Add", "vectors are not compatible");
      return target;
   }

   Double_t *sp = source.fElements;
   Double_t *tp = target.fElements;
   for ( ; tp < target.fElements+target.fNrows; )
      *tp++ += scalar * (*sp++);

   return target;
}

//______________________________________________________________________________
JSFDVector &ElementMult(JSFDVector &target, const JSFDVector &source)
{
   // Multiply target by the source, element-by-element.

   if (!AreCompatible(target, source)) {
      Error("ElementMult", "vectors are not compatible");
      return target;
   }

   Double_t *sp = source.fElements;
   Double_t *tp = target.fElements;
   for ( ; tp < target.fElements+target.fNrows; )
      *tp++ *= *sp++;

   return target;
}

//______________________________________________________________________________
JSFDVector &ElementDiv(JSFDVector &target, const JSFDVector &source)
{
   // Divide target by the source, element-by-element.

   if (!AreCompatible(target, source)) {
      Error("ElementDiv", "vectors are not compatible");
      return target;
   }

   Double_t *sp = source.fElements;
   Double_t *tp = target.fElements;
   for ( ; tp < target.fElements+target.fNrows; )
      *tp++ /= *sp++;

   return target;
}

//______________________________________________________________________________
void JSFDVector::Print(Option_t *)
{
   // Print the vector as a list of elements.

   if (!IsValid()) {
      Error("Print", "vector not initialized");
      return;
   }

   printf("\nVector %d is as follows", fNrows);

   printf("\n\n     |   %6d  |", 1);
   printf("\n%s\n", "------------------");
   for (Int_t i = 0; i < fNrows; i++) {
      printf("%4d |", i+fRowLwb);
      printf("%11.4g \n", (*this)(i+fRowLwb));
   }
   printf("\n");
}

//______________________________________________________________________________
void JSFDVector::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFDVector.

   if (R__b.IsReading()) {
      R__b.ReadVersion();  //Version_t R__v = R__b.ReadVersion();
      TObject::Streamer(R__b);
      R__b >> fRowLwb;
      fNrows = R__b.ReadArray(fElements);
   } else {
      R__b.WriteVersion(JSFDVector::IsA());
      TObject::Streamer(R__b);
      R__b << fRowLwb;
      R__b.WriteArray(fElements, fNrows);
   }
}

//______________________________________________________________________________
void Compare(const JSFDVector &v1, const JSFDVector &v2)
{
   // Compare two vectors and print out the result of the comparison.

   Int_t i;

   if (!AreCompatible(v1, v2)) {
      Error("Compare", "vectors are not compatible");
      return;
   }

   printf("\n\nComparison of two JSFDVectors:\n");

   Double_t norm1 = 0, norm2 = 0;       // Norm of the Matrices
   Double_t ndiff = 0;                  // Norm of the difference
   Int_t    imax = 0;                   // For the elements that differ most
   Double_t   difmax = -1;
   Double_t  *mp1 = v1.fElements;         // Vector element pointers
   Double_t  *mp2 = v2.fElements;

   for (i = 0; i < v1.fNrows; i++) {
      Double_t mv1 = *mp1++;
      Double_t mv2 = *mp2++;
      Double_t diff = TMath::Abs(mv1-mv2);

      if (diff > difmax) {
         difmax = diff;
         imax = i;
      }
      norm1 += TMath::Abs(mv1);
      norm2 += TMath::Abs(mv2);
      ndiff += TMath::Abs(diff);
   }

   imax += v1.fRowLwb;
   printf("\nMaximal discrepancy    \t\t%g", difmax);
   printf("\n   occured at the point\t\t(%d)", imax);
   const Double_t mv1 = v1(imax);
   const Double_t mv2 = v2(imax);
   printf("\n Vector 1 element is    \t\t%g", mv1);
   printf("\n Vector 2 element is    \t\t%g", mv2);
   printf("\n Absolute error v2[i]-v1[i]\t\t%g", mv2-mv1);
   printf("\n Relative error\t\t\t\t%g\n",
          (mv2-mv1)/TMath::Max(TMath::Abs(mv2+mv1)/2,(Double_t)1e-7));

   printf("\n||Vector 1||   \t\t\t%g", norm1);
   printf("\n||Vector 2||   \t\t\t%g", norm2);
   printf("\n||Vector1-Vector2||\t\t\t\t%g", ndiff);
   printf("\n||Vector1-Vector2||/sqrt(||Vector1|| ||Vector2||)\t%g\n\n",
          ndiff/TMath::Max(TMath::Sqrt(norm1*norm2), 1e-7));
}

//______________________________________________________________________________
void VerifyElementValue(const JSFDVector &v, Double_t val)
{
   // Validate that all elements of vector have value val (within 1.e-5).

   Int_t    imax = 0;
   Double_t max_dev = 0;
   Int_t    i;

   for (i = v.GetLwb(); i <= v.GetUpb(); i++) {
      Double_t dev = TMath::Abs(v(i)-val);
      if (dev > max_dev)
         imax = i, max_dev = dev;
   }

   if (max_dev == 0)
      return;
   else if(max_dev < 1e-5)
      printf("Element (%d) with value %g differs the most from what\n"
             "was expected, %g, though the deviation %g is small\n",
             imax, v(imax), val, max_dev);
   else
      Error("VerifyElementValue", "a significant difference from the expected value %g\n"
            "encountered for element (%d) with value %g",
            val, imax, v(imax));
}

//______________________________________________________________________________
void VerifyVectorIdentity(const JSFDVector &v1, const JSFDVector &v2)
{
   // Verify that elements of the two vectors are equal (within 1.e-5).

   Int_t    imax = 0;
   Double_t max_dev = 0;
   Int_t    i;

   if (!AreCompatible(v1, v2)) {
      Error("VerifyVectorIdentity", "vectors are not compatible");
      return;
   }

   for (i = v1.GetLwb(); i <= v1.GetUpb(); i++) {
      Double_t dev = TMath::Abs(v1(i)-v2(i));
      if (dev > max_dev)
         imax = i, max_dev = dev;
   }

   if (max_dev == 0)
      return;
   if (max_dev < 1e-5)
      printf("Two (%d) elements of vectors with values %g and %g\n"
             "differ the most, though the deviation %g is small\n",
             imax, v1(imax), v2(imax), max_dev);
   else
      Error("VerifyVectorIdentity", "a significant difference between the vectors encountered\n"
            "at (%d) element, with values %g and %g",
            imax, v1(imax), v2(imax));
}



#ifdef R__HPUX

//______________________________________________________________________________
//  These functions should be inline
//______________________________________________________________________________

JSFDVector::JSFDVector(Int_t n)
{
   Allocate(n);
}

JSFDVector::JSFDVector(Int_t lwb, Int_t upb)
{
   Allocate(upb-lwb+1, lwb);
}

Bool_t JSFDVector::IsValid() const
{
   if (fNrows == -1)
      return kFALSE;
   return kTRUE;
}

Bool_t AreCompatible(const JSFDVector &v1, const JSFDVector &v2)
{
   if (!v1.IsValid()) {
      Error("AreCompatible", "vector 1 not initialized");
      return kFALSE;
   }
   if (!v2.IsValid()) {
      Error("AreCompatible", "vector 2 not initialized");
      return kFALSE;
   }

   if (v1.fNrows != v2.fNrows || v1.fRowLwb != v2.fRowLwb)
      return kFALSE;

   return kTRUE;
}

JSFDVector &JSFDVector::operator=(const JSFDVector &source)
{
   if (this != &source && AreCompatible(*this, source)) {
      TObject::operator=(source);
      memcpy(fElements, source.fElements, fNrows*sizeof(Double_t));
   }
   return *this;
}

JSFDVector::JSFDVector(const JSFDVector &another)
{
   if (another.IsValid()) {
      Allocate(another.GetUpb()-another.GetLwb()+1, another.GetLwb());
      *this = another;
   } else
      Error("JSFDVector(const JSFDVector&)", "other vector is not valid");
}

void JSFDVector::ResizeTo(Int_t n)
{
   JSFDVector::ResizeTo(0,n-1);
}

void JSFDVector::ResizeTo(const JSFDVector &v)
{
   JSFDVector::ResizeTo(v.GetLwb(), v.GetUpb());
}

Double_t &JSFDVector::operator()(Int_t ind) const
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

Double_t &JSFDVector::operator()(Int_t index)
{
   return (Double_t&)((*(const JSFDVector *)this)(index));
}

JSFDVector &JSFDVector::Zero()
{
   if (!IsValid())
      Error("Zero", "vector not initialized");
   else
      memset(fElements, 0, fNrows*sizeof(Double_t));
   return *this;
}

#endif
