C
C ======================================================================
C
      SUBROUTINE BOOST0 (P,Q , PBOOST)
C
C This subroutine computes the Lorentz boost of the real vector.
C The vector P is assumed to be given in the rest frame of Q, which must
C be a timelike vector.  PBOOST is the vector P boosted to the frame in
C which Q is given.
C
C INPUT:
C       real    P(0:3)         : real vector which will be boosted
C       real    Q(0:3)         : vector of new origin in the old
C                                coordinates
C
C OUTPUT:
C       real    PBOOST(0:3)    : real vector boosted
C
      IMPLICIT REAL (A-H,L,M,O-Z)
      REAL    P(0:3),Q(0:3),PBOOST(0:3)
C
      QQ=Q(1)**2+Q(2)**2+Q(3)**2
C
      IF( QQ.NE.0 ) THEN 
         PQ=P(1)*Q(1)+P(2)*Q(2)+P(3)*Q(3)
C
         M=SQRT(Q(0)**2-QQ)
         LF=((Q(0)-M)*PQ/QQ+P(0))/M
         PBOOST(0)=(P(0)*Q(0)+PQ)/M
         PBOOST(1)=P(1)+Q(1)*LF
         PBOOST(2)=P(2)+Q(2)*LF
         PBOOST(3)=P(3)+Q(3)*LF
      ELSE
         PBOOST(0) = P(0)
         PBOOST(1) = P(1)
         PBOOST(2) = P(2)
         PBOOST(3) = P(3)
      ENDIF

      RETURN
      END
