#ifndef __JSFDefines__
#define __JSFDefines__

#define JSF_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define JSF_VERSION_CODE JSF_VERSION(__JSF_VERSION__,__JSF_MINORVERSION__,__JSF_PATCHLEVEL__)
#define GNUC_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define GNUC_VERSION_CODE GNUC_VERSION(__JSF_GNUC_VERSION__,__JSF_GNUC_MINOR__,__JSF_GNUC_PATCHLEVEL__)

#endif

