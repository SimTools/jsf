C zxtrks algo
      GWIDMAX = -100.0
      NITR = NITR + 1
      TRDISUM = 0.0
      VAX = SIN(THETAV)*COS(PHIV)
      VAY = SIN(THETAV)*SIN(PHIV)
      VAZ = COS(THETAV)
      CT(NGTRK) = COS(PHIV-POT)
      ST(NGTRK) = SIN(PHIV-POT)
      TL(NGTRK) = VAZ/SQRT(VAX**2+VAY**2)
      S1S(NGTRK)=GWID**2
      S2S(NGTRK)=GWID**2/(COS(ATAN(TL(NGTRK))))**2
      J = NGTRK
      DO L231 I=1,NTRK,1
C    try my own chi**2 calculation
         C1 = (CT(I)*CT(I)/S1S(I) + TL(I)*TL(I)*ST(I)*ST(I)/S2S(I) + CT(
     *   J)*CT(J)/S1S(J) + TL(J)*TL(J)*ST(J)*ST(J)/S2S(J))
         C2 = (ST(I)*ST(I)/S1S(I) + TL(I)*TL(I)*CT(I)*CT(I)/S2S(I) + ST(
     *   J)*ST(J)/S1S(J) + TL(J)*TL(J)*CT(J)*CT(J)/S2S(J))
         C3 = (1/S2S(I) + 1/S2S(J))
         C4 = CT(I)*ST(I)/S1S(I) - TL(I)*TL(I)*ST(I)*CT(I)/S2S(I) + CT(
     *   J)*ST(J)/S1S(J) - TL(J)*TL(J)*ST(J)*CT(J)/S2S(J)
         C5 = TL(I)*ST(I)/S2S(I) + TL(J)*ST(J)/S2S(J)
         C6 = -(TL(I)*CT(I)/S2S(I) + TL(J)*CT(J)/S2S(J))
         C7 = -(CT(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*ST(I)/S2S(I) + CT(J)*
     *   X0P(J)/S1S(J) + Z0(J)*TL(J)*ST(J)/S2S(J))
         C8 = (-ST(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*CT(I)/S2S(I) - ST(J)*
     *   X0P(J)/S1S(J) + Z0(J)*TL(J)*CT(J)/S2S(J))
         C9 = -(Z0(I)/S2S(I) + Z0(J)/S2S(J))
         CM(1,1) = C1
         CM(1,2) = C4
         CM(1,3) = C5
         CM(2,1) = C4
         CM(2,2) = C2
         CM(2,3) = C6
         CM(3,1) = C5
         CM(3,2) = C6
         CM(3,3) = C3
C    *****  copied this code from bmxinv, and densified *****
         SING=1.0
         N=3
         J1=1
            GO TO L243
L241         J1=J1+1
L243         IF(J1-(N).GT.0)GO TO L242
            INDEX(J1)=1
         GO TO L241
L242      CONTINUE
         NTODO=N
L250      PIVOT=0.0
         DO L261 J1=1,N
            IF((INDEX(J1).EQ.0))GOTO   L270
            ELM = ABS(CM(J1,J1))
            IF((ELM.LE.PIVOT))GOTO L270
            PIVOT=ELM
            I1=J1
L270         CONTINUE
L261      CONTINUE
L262      CONTINUE
         IF((PIVOT.EQ.0.0))GOTO   L280
         INDEX(I1)=0
L290      PIVOT=-CM(I1,I1)
         DO L301 J1=1,N
            IF((J1-I1))  L310,  L320,  L330
L320         CM(I1,J1)=1.0/PIVOT
            RI(J1)=0.0
            GOTO   L340
L310         ELM=-CM(I1,J1)
            GOTO   L350
L330         ELM=-CM(J1,I1)
L350         RI(J1)=ELM/PIVOT
            IF((ELM.EQ.0.0))GOTO   L360
            DO   L370 K1=1,J1
L370         CM(J1,K1)=CM(J1,K1)+ELM*RI(K1)
L360         IF (J1.GT.I1) GOTO   L380
L390         CM(I1,J1)=RI(J1)
            GOTO L340
L380         CM(J1,I1)=RI(J1)
L340         CONTINUE
L301      CONTINUE
L302      CONTINUE
         NTODO = NTODO - 1
         IF((NTODO.NE.0))GOTO L250
L400      DO   L410 I1=1,N
         DO L410 J1=1,I1
         CM(I1,J1)=-CM(I1,J1)
L410      CM(J1,I1)=CM(I1,J1)
         GOTO   L420
C    *****  end of matrix inversion code *****
L280      SING=0.0
L420      IF (SING.GT.0.5) THEN
            XD = -(CM(1,1)*C7 + CM(2,1)*C8 + CM(3,1)*C9)
            YD = -(CM(1,2)*C7 + CM(2,2)*C8 + CM(3,2)*C9)
            ZD = -(CM(1,3)*C7 + CM(2,3)*C8 + CM(3,3)*C9)
         END IF
         XPR = CT(I)*XD + ST(I)*YD
         YPR = -ST(I)*XD + CT(I)*YD
C    distance along track form 2d doca to poca xd vertex
         DTRK(I) = (XD+DX-TKX(I))*PX(I)+(YD+DY-TKY(I))*PY(I) +(ZD-TKZ(I)
     *   )*PZ(I)
         DTRK(I) = DTRK(I)/PTOT(I)
         PRFI = ((XPR-X0P(I))**2)/S1S(I) + ((ZD-(Z0(I)+YPR*TL(I)))**2)/
     *   S2S(I)
         XPR = CT(J)*XD + ST(J)*YD
         YPR = -ST(J)*XD + CT(J)*YD
         PRFJ = ((XPR-X0P(J))**2)/S1S(J) + ((ZD-(Z0(J)+YPR*TL(J)))**2)/
     *   S2S(J)
         CHISQ = PRFI + PRFJ
C    behind IP
         DE = GWID*SQRT(PRFJ)
C    latter should not happen
C      my calc at POCA
         IF (((VAX*XD+VAY*YD+VAZ*(ZD-AXI(3))).LT.0.0 .OR.  CHISQ.GT.
     *   CHI0(I))) THEN
            IF (NTRY.EQ.2) THEN
               CHISQ = CHI0(I)
            ELSE
               CHISQ = 2.0*CHI0(I) - CHISQ
            END IF
C        try this
         ELSE IF(DE.GT.0.00001) THEN
            GWIDN = CHISQ*S1S(J)**2*(CHISQ/CMAX-1.0)/DE**2+S1S(J)
            IF (GWIDN.GT.GWIDMAX) THEN
               GWIDMAX = GWIDN
            END IF
C      *    max gwid for all chi**2 < 2                              *
C      *      de = sqrt((xpr-x0p(j))**2                              *
C      *            +(zd-(z0(j)+ypr*tl(j)))**2*cos(atan(tl(j)))**2); *
C      *      Dee = chisq*s1s(j)/de;                                 *
C      *      s2 = Dee**2/chisq-s1s(j);                              *
C      *      gwidn = (Dee**2/cmax-s2);                              *
         END IF
         CHIS(I) = CHISQ
         TRDISUM = TRDISUM + CHISQ
L231   CONTINUE
L232   CONTINUE
C end loop over tracks
C  write(*,*) 'end 1 t ';
      AJET = VAX*JETD(1)+VAY*JETD(2)+VAZ*JETD(3)
      IF (AJET.GE.1.0) THEN
         AJET = 1.0
      END IF
      IF (AJET.LE.-1.0) THEN
         AJET = -1.0
      END IF
      AJET = ACOS(AJET)
      AJET = ABS(AJET-0.02)**0.8
C try soft jet core weight
      TRDISUM = TRDISUM + (AJET/0.3)**2
#if 0
      IF (RETN.EQ.0) GOTO 90
      IF (RETN.EQ.1) GOTO 120
      IF (RETN.EQ.2) GOTO 140
      IF (RETN.EQ.3) GOTO 160
      IF (RETN.EQ.4) GOTO 180
      IF (RETN.EQ.5) GOTO 190
      IF (RETN.EQ.10) GOTO 210
#endif
