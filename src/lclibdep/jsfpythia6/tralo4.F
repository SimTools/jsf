      SUBROUTINE TRALO4(KTO,P,Q,AMS)

      REAL   *4  P(4), Q(4), AMS
      INTEGER*4  KTO
C--
C #include "parmas.inc"
      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     .                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     .                 ,AMK,AMKZ,AMKST,GAMKST
C--
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     .                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     .                 ,AMK,AMKZ,AMKST,GAMKST

C #include "p4tau.inc"
      COMMON /P4TAU/ P4TAU(4)
      REAL*4         P4TAU
C
C====================< Entry Point >====================================
C
C--
C  KTO is useless in this single tau mode.
C--
      KTO = KTO
C--
C  Boost P to Q. 
C--
      AMS  = AMAS4(P)
C--      
      QA4  = P(4)*P4TAU(4) + P(1)*P4TAU(1)
     .                     + P(2)*P4TAU(2) 
     .                     + P(3)*P4TAU(3)
      QA4  = QA4/AMTAU
      A    = ( P(4) + QA4 )/( P4TAU(4) + AMTAU )
C--
      Q(1) = P(1) + A*P4TAU(1)
      Q(2) = P(2) + A*P4TAU(2)
      Q(3) = P(3) + A*P4TAU(3)
      Q(4) = SQRT( Q(1)**2 + Q(2)**2 + Q(3)**2 + AMS**2 )
C--
C  That's it.
C--
      RETURN
      END
