//*************************************************************************
//* ================
//*  Lockable Class
//* ================
//*
//* (Description)
//*    Lockable class supplies basic functions for lockable objects.
//* (Requires)
//* 	none
//* (Provides)
//* 	class Lockable
//* (Update Recored)
//*    1999/06/05  K.Fujii	Original very primitive version.
//*
//*************************************************************************
//
#include "Lockable.h"
#include <iostream>

using namespace std;
//_____________________________________________________________________
//  ------------------------------
//  Base Class for Lockale Objects
//  ------------------------------
//
//*--
//*  Constructors
//*--
Lockable::Lockable() { fStatus = kFALSE; }

//*--
//*  Destructors
//*--
Lockable::~Lockable() {}

//*--
//*  Getters
//*--
Bool_t Lockable::IsLocked() const { return fStatus; }

//*--
//*  Basic services
//*--
void Lockable::Lock() { fStatus = kTRUE; }

void Lockable::Unlock() { fStatus = kFALSE; }

ClassImp(Lockable)
