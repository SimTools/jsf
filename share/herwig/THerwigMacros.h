//$Id$ 
// Macros to access Herwig Commons, Get and Set functions
// 

#ifndef __THERWIG_MACROS__
#define __THERWIG_MACROS__


#ifndef HERWIG_SetGet_Imp
#define HERWIG_SetGet_Imp(common,type,var)  \
void THerwig::Set##var(type var)     \
  {  common->var = var ;  }  \
type THerwig::Get##var() { return common->var; }
#endif

#ifndef HERWIG_SetGet_PImp
#define HERWIG_SetGet_PImp(common,type,var)  \
void THerwig::Set##var(type var)     \
  {  common->var = var ;  }  \
type THerwig::Get##var() { return common->var; }
#endif


#ifndef HERWIG_SetGet_Def
#define HERWIG_SetGet_Def(type,var)  \
void THerwig::Set##var(type var);     \
type THerwig::Get##var();
#endif


#ifndef HERWIG_SetGetN_Imp
#define HERWIG_SetGetN_Imp(common,type,var)  \
void THerwig::Set##var(type var, int i)     \
  {  common->var[i-1] = var ;  }  \
type THerwig::Get##var(int i) { return common->var[i-1]; }

#endif


#ifndef HERWIG_SetGetN_PImp
#define HERWIG_SetGetN_PImp(common,type,var)  \
void THerwig::Set##var(type var, int i)     \
  {  common->var[i-1] = var ;  }  \
type THerwig::Get##var(int i) { return common->var[i-1]; }

#endif


#ifndef HERWIG_SetGetN_Def
#define HERWIG_SetGetN_Def(type,var) \
  void Set##var(type var, int i);   \
  type Get##var(int i); 

#endif

#ifndef HERWIG_SetGetNN_Imp
#define HERWIG_SetGetNN_Imp(common,type,var)  \
void THerwig::Set##var(type var, int i, int j)     \
  {  common->var[j-1][i-1] = var ;  }  \
type THerwig::Get##var(int i, int j) { return common->var[j-1][i-1]; }

#endif

#ifndef HERWIG_SetGetNN_PImp
#define HERWIG_SetGetNN_PImp(common,type,var)  \
void THerwig::Set##var(type var, int i, int j)     \
  {  common->var[j-1][i-1] = var ;  }  \
type THerwig::Get##var(int i, int j) { return common->var[j-1][i-1]; }

#endif


#ifndef HERWIG_SetGetNN_Def
#define HERWIG_SetGetNN_Def(type,var) \
  void Set##var(type var, int i, int j);   \
  type Get##var(int i, int j); 

#endif

#ifndef HERWIG_SetGetNNN_Imp
#define HERWIG_SetGetNNN_Imp(common,type,var)  \
void THerwig::Set##var(type var, int i, int j, int k)     \
  {  common->var[k-1][j-1][i-1] = var ;  }  \
type THerwig::Get##var(int i, int j, int k) { return common->var[k-1][j-1][i-1]; }

#endif

#ifndef HERWIG_SetGetNNN_PImp
#define HERWIG_SetGetNNN_PImp(common,type,var)  \
void THerwig::Set##var(type var, int i, int j, int k)     \
  {  common->var[k-1][j-1][i-1] = var ;  }  \
type THerwig::Get##var(int i, int j, int k) { return common->var[k-1][j-1][i-1]; }

#endif


#ifndef HERWIG_SetGetNNN_Def
#define HERWIG_SetGetNNN_Def(type,var) \
  void Set##var(type var, int i, int j, int k);   \
  type Get##var(int i, int j, int k); 

#endif

#endif
