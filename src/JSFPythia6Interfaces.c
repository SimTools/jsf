extern void pycomp_(int *kf);
extern void pylist_(int *mlist);
extern void pyname_(int *kf, char *chau);
extern int  pychge_(int *kf);
extern void pyrget_(int *lfn, int *move); 
extern void pyrset_(int *lfn, int *move); 
extern void pyhepc_(int *mconv); 
extern void pyjoin_(int *njoin, int *ijoin);
extern void pyprep_(int *ip);
extern void pyshow_(int *ip1, int *ip2, double *qmax);
extern void pyexec_();

/* ----------------------------------------------------- */
void lucomp_(int *kf){ pycomp_(kf); }
void lulist_(int *mlist){ pylist_(mlist); }
void luname_(int *kf, char *chau){ pyname_(kf, chau); }
int  luchge_(int *kf){ return pychge_(kf); }
void rluget_(int *lfn, int *move){ pyrget_(lfn, move); }
void rluset_(int *lfn, int *move){ pyrset_(lfn, move); }

void ludata_(){}

void luhepc_(int *mconv){ pyhepc_(mconv); }
void lujoin_(int *njoin, int *ijoin){ pyjoin_(njoin, ijoin); }
void luprep_(int *ip){ pyprep_(ip); }
void lushow_(int *ip1, int *ip2, double *qmax){ pyshow_(ip1, ip2, qmax); }
void luexec_(){ pyexec_(); }

