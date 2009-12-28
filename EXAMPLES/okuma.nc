$OKUMA.SSB%
(OKUMA OSP5020M)
OSTRT
CALL OCHCK
(M31 CLOSE DOOR)
G0 G90 G17 G21
IF [VC1 EQ 20] NDTM8
IF [VC1 EQ 22] NDTM9
GOTO NDTM
NDTM8
IF [FIX[VC2/7000] EQ 1] NNF80
IF [FIX[VC2/6000] EQ 1] NWF80
GOTO NDTM
NWF80 VC32=1
GOTO NDTM
NNF80 VC32=2
GOTO NDTM
NDTM9
IF [FIX[VC2/9000] EQ 1] NNF90
IF [FIX[VC2/8000] EQ 1] NWF90
GOTO NDTM
NWF90 VC32=3
GOTO NDTM
NNF90 VC32=4
NDTM G15 H=VC32
IF [VNTOL GT 0] NM64
GOTO NHME
NM64 M64
NHME G30 P8
CALL OSPD
M1
RTS

OTBOX
IF [VC1 EQ 20] NG80
IF [VC1 EQ 22] NG90
GOTO NCTL
NG80 CALL OT800 
GOTO NCTL
NG90 CALL OT900  
NCTL IF [VTLCN EQ 1] NXT2
T1
M6
NXT2 T2
NT1 (100MM FACE MILL)
(-------------------------------------)
(**         MILL                    **)
(-------------------------------------)
G15 H=VC32
XSRT=[XPOS-45-[VTOFD[1]]]
G0 X=XSRT Y=YPOS A=ANGL S=VC81 F=VC101 M3
G56 Z=ZPOS+1 H1 M8
(M51)
MODIN OINT1
VC41=[VDIN[1001]/60]
G1 G91 X95
G0 G90 X=XSRT Y=YPOS Z=ZPOS+2
Z=ZPOS
G1 G91 X95
G0 G90 Z999
MODOUT
M1
IF [VTLCN EQ 2] NXT3
T2
M6
NXT3 T3
NT2 (8MM SPOTDRILL)
(-------------------------------------)
(**       SPOTDRILL ALL HOLES       **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC82 F=VC102 M3
G56 Z=ZPOS+50 H2 M8
(M51)
G71 Z140
NCYL G81 Z=ZPOS-2.6 R=ZPOS+3 M54
BHC X=XPOS Y=YPOS I35 J45 K4
X=XPOS+8 Y=YPOS Z=ZPOS-2.1
X=XPOS-8
X=XPOS+13.00 Y=YPOS-23.0 Z=ZPOS-5.1 M53
X=XPOS+6.5 Y=ZPOS-7 Z=[YPOS+[35+10-1.5]] R=[YPOS+49] A=ANGL-90 M16
G80
G0 Z999
M1
IF [VTLCN EQ 3] NXT4
T3
M6
NXT4 T4
NT3 (4.2MM DRILL)
(-------------------------------------)
(**         DRILL M5 HOLES          **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC83 F=VC103 M3
G56 Z=ZPOS+50 H3 M8
(M51)
G81 X=XPOS+6.5 Y=ZPOS-7 Z=[YPOS+[35+10-16]] R=[YPOS+49] A=ANGL-90 M53
NCYL G81 Z=ZPOS-18 R=ZPOS+3 A=ANGL M54 M15
BHC X=XPOS Y=YPOS I35 J45 K4
X=XPOS+13.00 Y=YPOS-23.0 Z=ZPOS-22 M52
G80
G0 Z999
M1
IF [VTLCN EQ 4] NXT5
T4
M6
NXT5 T5
NT4 (M5 TAP)
(-------------------------------------)
(**          TAP M5 HOLES           **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC84 F=VC104 M3
G56 Z=ZPOS+50 H4 M8
(M51)
VTMNO=1
M327
NCYL G84 Z=ZPOS-15 R=ZPOS+3 M54
BHC X=XPOS Y=YPOS I35 J45 K4
X=XPOS+13.00 Y=YPOS-23.0 Z=ZPOS-19.5 M53
X=XPOS+6.5 Y=ZPOS-7 Z=[YPOS+[35+10-13]] R=[YPOS+49] A=ANGL-90 M16
M326
G80
G0 Z999
M1
IF [VTLCN EQ 5] NXT6
T5
M6
NXT6 T6
NT5 (3.2MM DRILL)
(-------------------------------------)
(**         DRILL M4 HOLES          **)
(-------------------------------------)
G15 H=VC32 M15
G0 X=XPOS Y=YPOS A=ANGL S=VC85 F=VC105 M3
G56 Z=ZPOS+50 H5 M8
(M51)
NCYL G81 Z=ZPOS-18 R=ZPOS+3 M54
X=XPOS+8 Y=YPOS
X=XPOS-8
G80
G0 Z999
M1
IF [VTLCN EQ 6] NT6
T6
M6
NT6 (M4 TAP)
(-------------------------------------)
(**          TAP M4 HOLES           **)
(-------------------------------------)
G15 H=VC32 M15
G0 X=XPOS Y=YPOS A=ANGL S=VC86 F=VC106 M3
G56 Z=ZPOS+50 H6 M8
(M51)
VTMNO=1
M327
NCYL G84 Z=ZPOS-15 R=ZPOS+3 M54
X=XPOS+8 Y=YPOS
X=XPOS-8
M326
G80
G0 Z999
M1
RTS

OFT80
CALL OF800  
IF [VTLCN EQ 1] NXT8
T1
M6
NXT8 T8
NT1 (100MM FACE MILL)
(-------------------------------------)
(**            MILL FEET            **)
(-------------------------------------)
G15 H=VC32 M16
XSRT=[XPOS-74-[VTOFD[1]]]
G0 X=XSRT Y=YPOS+63 A=ANGL S=VC81 F=VC101 M3
G56 Z=ZPOS+1 H1 M8
(M51)
MODIN OINT1
VC41=[VDIN[1001]/60]
G1 G91 X147
G0 G90 X=XSRT Z=ZPOS+2
Z=ZPOS
G1 G91 X147
G0 G90 X=XSRT Y=YPOS-63 Z=ZPOS+10
Z=ZPOS+1
G1 G91 X147
G0 G90 X=XSRT Z=ZPOS+2
Z=ZPOS
G1 G91 X147
G0 G90 Z999
MODOUT
M1
IF [VTLCN EQ 8] NXT7
T8
M6
NXT7 T7
NT8 (10.5MM DRILL)
(-------------------------------------)
(**           DRILL HOLES           **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC88 F=VC108 M3
G56 Z=ZPOS+50 H8 M8
(M51)
G81 X=XPOS-50 Y=YPOS-62.5 Z=ZPOS-20 R=ZPOS+3 M54
SQRX X=XPOS-50 Y=YPOS-62.5 I100 J125 K1 P1
G80
G0 Z999
M1
IF [VTLCN EQ 7] NT7
T7
M6
NT7 (12MM SPOTDRILL)
(-------------------------------------)
(**          CHAMFER HOLES          **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC87 F=VC107 M3
G56 Z=ZPOS+50 H7 M8
(M51)
G81 X=XPOS-50 Y=YPOS-62.5 Z=ZPOS-5.4 R=ZPOS+3 M54
SQRX X=XPOS-50 Y=YPOS-62.5 I100 J125 K1 P1
G80
G0 Z999
M1
RTS
(-------------------------------------)
(-------------------------------------)
(**   90 FRAME FOOT M/C COMPLETE    **)
(-------------------------------------)
(-------------------------------------)
OFT90
CALL OF900  
IF [VTLCN EQ 1] NXT8
T1
M6
NXT8 T8
NT1 (100MM FACE MILL)
(-------------------------------------)
(**            MILL FEET            **)
(-------------------------------------)
G15 H=VC32 M16
XSRT=[XPOS-88-[VTOFD[1]]]
G0 X=XSRT Y=YPOS+70 A=ANGL S=VC81 F=VC101 M3
IF [ZPOS LT 90] NSZOF
ZOFS=1
GOTO NG56
NSZOF ZOFS=2.1
NG56 G56 Z=ZPOS+ZOFS H1 M8
(M51)
MODIN OINT1
VC41=[VDIN[1001]/60]
G1 G91 X175
G0 G90 X=XSRT Z=ZPOS+ZOFS+1
IF [ZPOS GE 90] NLPS1
Z=ZPOS+1
G1 G91 X175
G0 G90 X=XSRT Z=ZPOS+ZOFS+1
NLPS1 Z=ZPOS
G1 G91 X175
G0 G90 X=XSRT Y=YPOS-70 Z=ZPOS+10
Z=ZPOS+ZOFS
G1 G91 X175
G0 G90 X=XSRT Z=ZPOS+ZOFS+1
IF [ZPOS GE 90] NLPS2
Z=ZPOS+1
G1 G91 X175
G0 G90 X=XSRT Z=ZPOS+ZOFS+1
NLPS2 Z=ZPOS
G1 G91 X175
G0 G90 Z999
MODOUT
M1
IF [VTLCN EQ 8] NXT7
T8
M6
NXT7 T7
NT8 (10.5MM DRILL)
(-------------------------------------)
(**           DRILL HOLES           **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC88 F=VC108 M3
G56 Z=ZPOS+50 H8 M8
(M51)
G81 X=XPOS-63 Y=YPOS-70 Z=ZPOS-20 R=ZPOS+3 M54
LAA X=XPOS-63 Y=YPOS-70 I25 I76 I25 J0
X=XPOS-63 Y=YPOS+70
LAA X=XPOS-63 Y=YPOS+70 I25 I76 I25 J0
G80
G0 Z999
M1
IF [VTLCN EQ 7] NT7
T7
M6
NT7 (12MM SPOTDRILL)
(-------------------------------------)
(**          CHAMFER HOLES          **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS S=VC87 F=VC107 M3
G56 Z=ZPOS+50 H7 M8
(M51)
G81 X=XPOS-63 Y=YPOS-70 Z=ZPOS-5.4 R=ZPOS+3 M54
LAA X=XPOS-63 Y=YPOS-70 I25 I76 I25 J0
X=XPOS-63 Y=YPOS+70
LAA X=XPOS-63 Y=YPOS+70 I25 I76 I25 J0
G80
G0 Z999
M1
RTS
(-------------------------------------)
(-------------------------------------)
(**   NAMEPLATE WITH PLAIN HOLES    **)
(-------------------------------------)
(-------------------------------------)
OPLTP
IF [VC1 EQ 20] NPG80
IF [VC1 EQ 22] NPG90
GOTO NCTL
NPG80 CALL ON800  
GOTO NCTL
NPG90 CALL ON900  
NCTL IF [VTLCN EQ 5] NT5
T5
M6
NT5 (3.2MM DRILL)
(-------------------------------------)
(**           DRILL HOLES           **)
(-------------------------------------)
G15 H=VC32 M16
G0 X=XPOS Y=YPOS A=ANGL S=VC85 F=VC105 M3
G56 Z=ZPOS+50 H5 M8
(M51)
XOFS=CENT/2
G81 X=XPOS-XOFS Y=YPOS Z=ZPOS-6 R=ZPOS+8 M54
    X=XPOS+XOFS
G80
G0 Z999 M15
M1
RTS
(-------------------------------------)
(-------------------------------------)
OTAGT
IF [VTLCN EQ 9] NXT10
T9
M6
NXT10 T10
NT9 (2.5MM DRILL)
(-------------------------------------)
(**          DRILL M3 HOLES         **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS A=ANGL S=VC89 F=VC109 M3
G56 Z=ZPOS+50 H9 M8
(M51)
XOFS=CENT/2
G81 X=XPOS-XOFS Y=YPOS Z=ZPOS-9 R=ZPOS+8 M54
    X=XPOS+XOFS
G80
G0 Z999
M1
IF [VTLCN EQ 10] NT10
T10
M6
NT10 (M3 TAP)
(-------------------------------------)
(**          TAP M3 HOLES           **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS A=ANGL S=VC90 F=VC110 M3
G56 Z=ZPOS+50 H10 M8
(M51)
VTMNO=1
M327
G84 X=XPOS-XOFS Y=YPOS Z=ZPOS-7 R=ZPOS+8 M54
    X=XPOS+XOFS
M326
G80
G0 Z999
M1
RTS
(-------------------------------------)
(-------------------------------------)
OTAGP
IF [VTLCN EQ 5] NT5
T5
M6
NT5 (3.2MM DRILL)
(-------------------------------------)
(**       DRILL 3.2MM HOLES         **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS A=ANGL S=VC85 F=VC105 M3
G56 Z=ZPOS+50 H5 M8
(M51)
XOFS=CENT/2
G81 X=XPOS-XOFS Y=YPOS Z=ZPOS-9 R=ZPOS+8 M54
    X=XPOS+XOFS
G80
G0 Z999
M1
RTS

(-------------------------------------)
(-------------------------------------)
OMLIS
IF [VC1 EQ 20] NG80
IF [VC1 EQ 22] NG90
GOTO NCTL
NG80 CALL OT800 
GOTO NCTL
NG90 CALL OT900  
NCTL IF [VTLCN EQ 11] NT11
T11
M6
NT11 (12MM MILL)
(-------------------------------------)
(**      MILL T BOX ISLAND OFF      **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS A=ANGL S=VC91 F=VC111 M3
G56 Z=ZPOS+50 H11 M8
(M51)
G0 X=XPOS-15
Z=ZPOS+1
G1 Z=ZPOS-2 F=VC111*0.5
X=XPOS+15 F=VC111
G0 X=XPOS-15 Z=ZPOS-1
G1 Z=ZPOS-4 F=VC111*0.5
X=XPOS+15 F=VC111
G0 X=XPOS-15 Z=ZPOS-3
G1 Z=ZPOS-6 F=VC111*0.5
X=XPOS+15 F=VC111
G0 X=XPOS-15 Z=ZPOS-5
G1 Z=ZPOS-8 F=VC111*0.5
X=XPOS+15 F=VC111
G0 X=XPOS-15 Z=ZPOS-7
G1 Z=ZPOS-10 F=VC111*0.5
X=XPOS+15 F=VC111
G0 X=XPOS-15 Z=ZPOS-9
G1 Z=ZPOS-12 F=VC111*0.5
X=XPOS+15 F=VC111
Y=YPOS+3
X=XPOS-15
Y=YPOS-3
X=XPOS+15
G0 Z=ZPOS+50
G0 G90 Z999
M1
RTS
(-------------------------------------)
(-------------------------------------)

OBLDA
IF [VC1 EQ 20] NG80
IF [VC1 EQ 22] NG90
GOTO NCTL
NG80 CALL OT800 
GOTO NCTL
NG90 CALL OT900  
NCTL IF [VTLCN EQ 13] NT13
T13
M6
NT13 (10MM BALL NOSE MILL)
(-------------------------------------)
(**            MILL HOLE            **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS A=ANGL S=VC93 F=VC113 M3
G56 Z=ZPOS+50 H13 M8
(M51)
G0 X=XPOS-12 Y=YPOS+31
Z=ZPOS+1
G1 Z=ZPOS-10 F=VC113*0.5
Y=YPOS+26 F=VC113
Y=YPOS+36
G0 Z=ZPOS+50
G0 G90 Z999
M1
RTS

(-------------------------------------)
(-------------------------------------)
OBLDB
IF [VC1 EQ 20] NG80
IF [VC1 EQ 22] NG90
GOTO NCTL
NG80 CALL OT800
YOFS=11
GOTO NCTL
NG90 CALL OT900  
YOFS=12
NCTL IF [VTLCN EQ 13] NT13
T13
M6
NT13 (10MM BALL NOSE MILL)
(-------------------------------------)
(**            MILL HOLE            **)
(-------------------------------------)
G15 H=VC32
G0 X=XPOS Y=YPOS A=ANGL S=VC93 F=VC113 M3
G56 Z=ZPOS+50 H13 M8
(M51)
G0 X=XPOS+31 Y=YPOS+YOFS
Z=ZPOS+1
G1 Z=ZPOS-10 F=VC113*0.5
X=XPOS+26 F=VC113
X=XPOS+36
G0 Z=ZPOS+50
G0 G90 Z999
M1
RTS

