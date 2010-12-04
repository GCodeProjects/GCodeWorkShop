% 
:0100( FLANGE OP.2 ) 
( DRG. 12345 )
( 19.04.2010 ) 
(TOOLTIPS FROM cnc_tips.txt)
#501=350.(FLANGE DIA.) 
#502=-23(FLANGE CLEARENCE DEPTH) 
#503=-5(FLANGE FACE)
#504=250(SPIGOT DIA)
#505=0(DATUM/SPIGOT FACE)
#506=214(SPIGOT ID CLEARENCE) 
#507=120(SHAFT OD CLEARENCE) 
#508=-8.0(SHAFT FACE) 
#509=45.15(SHAFT DIA)
(****************************************) 
N1G0X380.Z300.T0101M16 
G0G96X[#501]Z[#505+50.]S250M3M8(ROUGH SPIGOT) 
M7 
Z[#505]
G1Z[#502]F0.35 
G0X[#501+2.]Z[#503+2.] 
G1X[#504+6.]F.3
G0X[#504+8.]Z[#505+2.] 
G1X[#506]
G0X[#501+2.]Z[#505+3.] 
Z[#503-2.] 
G1U-4.6Z[#503+.3]F0.2
X[#504+6.]F0.3 
G0X[#504+8.]Z[#505+0.3]
G1X[#506]
G0X[#504+3.]Z[#505+1.] 
G1Z[#503+0.3]
X[#504+6.]F0.35
G0Z[#505+1.] 
X[#504+0.5]
G1Z[#503+0.3]F0.3
X[#504+3.]F0.35
G0Z200.
M01
N5G0Z200.T0505M16
G96X[#509-1]Z[#505+50]S250M3M8(SHAFT FACE)
Z[#508+2.] 
G1X[#507]F0.25 
G0X[#509+2.6]Z[#508+3.]
G1Z[#508+0.5]F.2 
G1X[#509]W-1.3F.15 
G0X[#509-1.]Z[#508]
G1X[#507]F.2 
G0Z[#505+5.] 
G0Z200.M9
M1 
N3G0Z200.T0303M16
G96X[#501+2.]Z[#505+50.]S280M3M8(FINISH SPIGOT)
Z[#503-2.] 
G1U-4.Z[#503]F0.18 
X[#504+0.5]
G0X[#504+.7]Z[#505-2.5]
G1U1.4Z[#505]F.15
X[#506]F.2 
G0X[#504]Z[#505+1.]
G1Z[#503-.1]F.15 
G4P200 
G0X400.Z300.M9 
G97S1200 
G04P2000 
S150 
M26
G4P6000
M27
M5 
M30
%