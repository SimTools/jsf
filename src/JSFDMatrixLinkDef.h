#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ function operator+=(JSFDVector&,const JSFDVector&);
#pragma link C++ function operator-=(JSFDVector&,const JSFDVector&);
#pragma link C++ function Add(JSFDVector&,Double_t,const JSFDVector&);
#pragma link C++ function ElementMult(JSFDVector&,const JSFDVector&);
#pragma link C++ function ElementDiv(JSFDVector&,const JSFDVector&);
#pragma link C++ function operator==(const JSFDVector&, const JSFDVector&);
#pragma link C++ function operator*(const JSFDVector&,const JSFDVector&);
#pragma link C++ function Compare(const JSFDVector&,const JSFDVector&);
#pragma link C++ function AreCompatible(const JSFDVector&,const JSFDVector&);
#pragma link C++ function operator+=(JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function operator-=(JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function Add(JSFDMatrix&,Double_t,const JSFDMatrix&);
#pragma link C++ function ElementMult(JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function ElementDiv(JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function operator==(const JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function Compare(const JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function AreCompatible(const JSFDMatrix&,const JSFDMatrix&);
#pragma link C++ function E2Norm(const JSFDMatrix&,const JSFDMatrix&);

#pragma link C++ class JSFDVector-;
#pragma link C++ class JSFDMatrix-;
#pragma link C++ class JSFDLazyMatrix;
#pragma link C++ class JSFDMatrixRow-;
#pragma link C++ class JSFDMatrixColumn-;
#pragma link C++ class JSFDMatrixDiag-;

#endif
