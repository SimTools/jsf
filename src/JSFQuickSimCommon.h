#ifndef __JSFQuickSimCommon__
#define __JSFQuickSimCommon__

//////////////////////////////////////////////////////////////////////////
//
// JSFQuickSimCommon
//
// Common definitions for Quick Simulator.
//
//////////////////////////////////////////////////////////////////////////

// ******* Commons for Smearing ****************
typedef struct {
  Float_t bfield, ptor, ropt;
} COMMON_PRFELD;

typedef struct { 
  Int_t iseed;
} COMMON_SWMRND;

typedef struct {
  Int_t imodbf[2];
  Float_t bfdata[2];
} COMMON_SMRFLD;

extern COMMON_SWMRND swmrnd_;
extern COMMON_PRFELD prfeld_;
extern COMMON_SMRFLD smrfld_;


// ******* Commons for smearng *****************
static const Int_t kMXxVXL=11;
typedef struct {
  Int_t   nsmpvx;
  Float_t dphivx, dzeevx;
  Float_t rcyvx[kMXxVXL], zcyvx[kMXxVXL][2], rdlvx[kMXxVXL];
  Int_t   nerrvx;
  Float_t errvx[5];
} COMMON_SMRVGO ;
extern COMMON_SMRVGO smrvgo_;

typedef struct {
  Int_t nsmptk, nsmpct;
  Float_t dsmptk, rcytk[2], zcytk[2];
} COMMON_SMRTGO;
extern COMMON_SMRTGO smrtgo_;

typedef struct {
  Int_t nphiem, nthtem, nradem;
  Float_t  dphiem, dthtem, drdmem;
  Float_t  drdpem, thmnem, thmxem, rmmnem, rmmxem;
  Float_t  rpmnem, rpmxem, rcyem[2], zcyem[2];
  Int_t    npadem;
} COMMON_SMREGO;
extern COMMON_SMREGO smrego_;


typedef struct {
  Int_t   nphihd, nththd, nradhd;
  Float_t dphihd, dththd, drdmhd;
  Float_t drdphd, thmnhd, thmxhd, rmmnhd, rmmxhd;
  Float_t rpmnhd, rpmxhd, rcyhd[2], zcyhd[2];
} COMMON_SMRHGO;
extern COMMON_SMRHGO smrhgo_;

typedef struct {
  Float_t sigrf , sigze;
  Float_t sgemb0, sgemb, sgeme0, sgeme, sghdb0, sghdb;
  Float_t sghde0, sghde;
} COMMON_SMRRES;
extern COMMON_SMRRES smrres_;

typedef struct {
  Float_t clspar[4][5];
} COMMON_SMRCLP;
extern COMMON_SMRCLP smrclp_;

typedef struct {
  Float_t adxect, ensgem, ensghd;
} COMMON_CMBCUT;
extern COMMON_CMBCUT cmbcut_;

#endif
