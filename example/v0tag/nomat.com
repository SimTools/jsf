@!
@! jlc3T.com prepared in 25-January-2000
@!   JLC detector parameter for 3 Tesla Solenoid field.
@!   5 layers of intremideate tracker is also included.
@!
@!(Description)
@!  This is a sample input stream for the JLC quick detector simulator.
@!  Major detector parameters of this file are as follows.
@!    VTX  : 4 layer from r=2.4 to 6.0cm
@!           0.448 < Theta < Pi - 0.448
@!    CDC  : 45 cm < r < 155 cm,  50 sampling    
@!           -155.0cm < z < 155.0 cm
@!           sigma-rphi= 85 micron m
@!           sigma-z   = 0.3 cm
@!    ECL  : dE/E      = 15%/Sqrt(E) + 1% 
@!           R=160cm, -190 cm < Z < 190cm   (Barrel)
@!           45cm<R<160cm at Z=+-300cm      (EndCap)
@!    HDC  : dE/E      = 40%/Sqrt(E) + 1%
@!           R=190cm, -190 cm < Z < 190 cm  (Barrel)
@!           50cm<R<190cm at Z=190cm        (EndCap)
@!    Magnet : 3 tesla
@!
@!(Update Record)
@!  91/02/18  K.Fujii          This version contains parameters for
@!                             vertex detector. The calorimeter
@!                             parameters are consistent with the
@!                             default clustering parameters.
@!                             Those who wish to modify calorimeter
@!                             parameters should tune the clustering
@!                             parameters for yourself. Ask TAYM for
@!                             details of the tunig.
@!  91/03/06  K.Fujii          New detector parameters.
@!
@!  92/07/01  A.Miyamoto       Parameter set for Proposed Detector.
@!  98/12/25  A.Miyamoto       Parameter set for 4 layer VTX,
@!                             by Sugimoto-san's recomendation
@!  99/03/30  A.Miyamoto       If NERRVX=3, create smeared VTX;Track_parameter
@!                             If NUMVTX>NSMPVX+1, Hits of Inner tracker is 
@!                             created, with a spacial resolution different 
@!                             from VTX.  See comments below about how to set
@!                             parameters for Inner Tracker.
@!  99/04/06  A.Miyamoto       detect6.com is revised to have consistency 
@!                             with JIM and Background study
@!  99/05/07  A.Miyamoto       Thick ness of 1st VTX layer is changes from 0.0015 to 0.003
@!2000/01/25  A.Miyamoto       Parameter set for 3T solenoid, with Intermideate tracker
@!
@! $Id$
@!
@! Tracking device parameters.
@!
  1    314159 ! seed for smearing
  2    30.    ! B field (kG) ( dummy if Swimmer specifies B )
@!
@!
@! Tracking device parameters.
@!
 11    45.    ! inner radius(cm)
 12   155.    ! outer radius(cm)
 13  -155.    ! Z-(cm)
 14   155.    ! Z+(cm)
 15    50     ! # sampling points
 16    10     ! cut on # sampling points
 17   0.85E-2 ! sigma_(r-phi)
 18     3.E-1 ! sigma_Z
@!
@! EM_calorimeter parameters.
@!
 31   252    ! # phi segments
 32    90    ! # theta segments of barrel
 33    29    ! # radial segments of endcap
 34    45.   ! inner radius(cm)
 35   160.   ! outer radius(cm)
 36  -190.   ! Z-(cm)
 37   190.   ! Z+(cm)
 38   1.E-2  ! sigma_E/E pedestal for barrel
 39  1.5E-1  ! sigma_E/E at 1 GeV
 40   1.E-2  ! sigma_E/E pedestal for endcap
 41  1.5E-1  ! sigma_E/E at 1 GeV
@!
@! HD_calorimeter parameters.
@!
 51    84    ! # phi segments
 52    30    ! # theta segments of barrel
 53    28    ! # radial segments of endcap
 54    50.   ! inner radius(cm)
 55   190.   ! outer radius(cm)
 56  -190.   ! Z-(cm)
 57   190.   ! Z+(CM)
 58    1.E-2 ! sigma_E/E pedestal for barrel
 59    4.E-1 ! sigma_E/E at 1 GeV
 60    1.E-2 ! sigma_E/E pedestal for endcap
 61    4.E-1 ! sigma_E/E at 1 GeV
@!
@! Vertex detector parameters.
@!    NSMPVX specifies # of measuring layers. Layer #0 is beampipe
@!    and layer #NSMPVX is CDC inner cylinder. A negative NSMPVX
@!    skips VTX simulation.
@!
@! ***** Caution *******************************************************
@! *  Up to parameter #71 has a fixed format, while parameters #72     *
@! *  and after depend on NSMPVX. In the following, for instance,      *
@! *  parameters #86-89 are for the CDC inner cylinder since NSMPVX    *
@! *  is 3 here. If NSMPVX is 4, parameters #86-89 are for 3-rd VTX    *
@! *  layer and the CDC inner cylinder parameters becom #90-93.        *
@! *********************************************************************
@! *  24-Dec-1998 : Data format is changed to allow gaussian error.
@! *  When word #70 (NERRVX) is 1, VTX space point is digitized by the
@! *  size of pixel ( phi and z pitch ).  If NERRVX is 2, VTX Space
@! *  point is smeared by gaussian, whose standard deviation is given by 
@! *  the word following CDC inner cylynder section. 
@! *********************************************************************
@! *  29-March-1999 : Add NERRVTX=3 format.
@! *  # of sampling layer is a sum of sampling layers of VTX and that of 
@! *  inner tracker.  Number of VTX layer must be greater than 2, since
@! *  they are used to create smeared VTX:Track_Parameters.
@! *  If NSMPVX+1 > NUMVTX, hit points for Inner Tracker are created and
@! *  stored in the bank, Production:IT;Hit_Points.
@! *  Spacing between VTX sampling layer must be equal since it is assumed
@! *  in the routine to create VTX Smeared track.
@! *********************************************************************
@!
 70     3    ! NERRVX Type of parameter format
 71    10    ! # sampling layers + 1 = NSMPVX
 72     4    ! NUMVTX Number of VTX layers ( those used for tracking)
 73    4.E-4 ! r-phi resolution (cm) of VTX
 74    4.E-4 ! Z   resolution (cm) of VTX
 75   40.E-4 ! r-phi resolution (cm) of Inner Tracker
 76   40.E-4 ! Z   resolution (cm) of Inner Tracker
@! layer #0 (beampipe) 
 77     2.   ! radius (cm)
 78  -100.   ! Z-(cm)
 79   100.   ! Z+(cm)
 80   0.000000015  ! thickness in radiation length
@! layer #1
 81     2.4  ! radius (cm)
 82    -5.0  ! Z-(cm)
 83     5.0  ! Z+(cm)
 84   0.00000003  ! thickness in radiation length
@! layer #2 
 85     3.6  ! radius (cm)
 86    -7.5  ! Z-(cm)
 87     7.5  ! Z+(cm)
 88   0.00000003  ! thickness in radiation leng5h
@! layer #3
 89     4.8  ! radius (cm)
 90   -10.0  ! Z-(cm)
 91    10.0  ! Z+(cm)
 92   0.00000003  ! thickness in radiation length
@! layer #4
 93     6.0  ! radius (cm)
 94   -12.5  ! Z-(cm)
 95    12.5  ! Z+(cm)
 96   0.00000003  ! thickness in radiation length
@! 
@! Following is a parmeter set for Intermidiate tracker.
@! This parameter sets are determined so as to
@!    (1) 5 layers to allow self tracking.
@!    (2) Uniform sampling between VTX to IT
@!    (3) Covers same costh range as vtx
@!    (4) Twice more thickness than VTX 
@! layer #5 (IT 1)
 97     9.0  ! radius (cm)
 98   -18.5  ! Z-(cm)
 99    18.5  ! Z+(cm)
100   0.00000006  ! thickness in radiation length
@! layer #6 (IT 2)
101    16.0  ! radius (cm)
102   -33.0  ! Z-(cm)
103    33.0  ! Z+(cm)
104   0.00000006  ! thickness in radiation length
@! layer #7 (IT 3)
105    23.0  ! radius (cm)
106   -47.5  ! Z-(cm)
107    47.5  ! Z+(cm)
108   0.00000006  ! thickness in radiation length
@! layer #8 (IT 4)
109    30.0  ! radius (cm)
110   -62.0  ! Z-(cm)
111    62.0  ! Z+(cm)
112   0.00000006  ! thickness in radiation length
@! layer #9 (IT 5)
113    37.0  ! radius (cm)
114   -76.5  ! Z-(cm)
115    76.5  ! Z+(cm)
116   0.00000006  ! thickness in radiation length
@!          .........
@!          .........
@! layer #NSMPVX (CDC inner cylinder)
117    40.   ! radius (cm)
118  -230.   ! Z-(cm)
119   230.   ! Z+(cm)
120   0.0000001   ! thickness in radiation length
@!
@! For example to include a layer of Inner Tracker, uncomment following
@! lines of parameter number 97 to 104, and set NSMPVX ( parameter number 71)
@! to 6.  Spacial resolution are specified by parameter number 75 and 76.
@!
@! layer #5 for IT
@! 97    20.0  ! radius (cm)
@! 98   -12.5  ! Z-(cm)
@! 99    12.5  ! Z+(cm)
@!100   0.003  ! thickness in radiation length
@! layer #NSMPVX (CDC inner cylinder)
@!101    25.   ! radius (cm)
@!102  -230.   ! Z-(cm)
@!103   230.   ! Z+(cm)
@!104   0.01   ! thickness in radiation length
@!
@!
@!          .........                                                          
@!  Parameters for Clustering.                                                 
@!          .........
1001  100.   ! Eth(MeV) for barrel EMC
1002   0.4   ! C1 for barrel EMC
1003   0.2   ! C2 for barrel EMC
1004   1.0   ! C3 for barrel EMC
1005   0.3   ! Alpha (to determine poistion) of EMC
1006  100.   ! Eth(MeV) for Endcal EMC
1007   0.4   ! C1 for Endcap EMC
1008   0.2   ! C2 for endCap EMC
1009   1.0   ! C3 for Endcal EMC
1010   0.3   ! Alpha (to determine poistion) of HDC
1011  100.   ! Eth(MeV) for barrel HDC
1012   0.4   ! C1 for barrel HDC
1013   0.2   ! C2 for barrel HDC
1014   1.0   ! C3 for barrel HDC
1015   0.25  ! Alpha (to determine poistion) of HDC
1016  100.   ! Eth(MeV) for Endcap HDC
1017   0.4   ! C1 for Endcap HDC
1018   0.2   ! C2 for Endcap HDC
1019   1.0   ! C3 for Endcap HDC
1020   0.25  ! Alpha (to determine poistion) of HDC
