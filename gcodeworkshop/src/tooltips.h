/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of EdytorNC.                                        *
 *                                                                         *
 *   EdytorNC is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TOOLTIPS_H
#define TOOLTIPS_H

#include <QSettings>
#include <QObject>

#include <utils/medium.h>

// _TR(s) QT_TRANSLATE_NOOP(s)
#define _TR(s) s

QString writeTooltipFile()
{
	QString fileName = Medium::instance().settingsDir() + "/" + "cnc_tips_" + QLocale::system().name() + ".txt";

	QSettings settings(fileName, QSettings::IniFormat);

	//*******************************************************************************************************************//
	settings.beginGroup("OKUMA");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M02", _TR("<b>M02</b> - end of program"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M12", _TR("<b>M12</b> - M-tool spindle STOP"));
	settings.setValue("M13", _TR("<b>M13</b> - M-tool spindle CW"));
	settings.setValue("M14", _TR("<b>M14</b> - M-tool spindle CCW"));
	settings.setValue("M15", _TR("<b>M15</b> - B or C-axis positioning, plus direction CW"));
	settings.setValue("M16",
	                  _TR("<b>M16</b> - B or C-axis positioning, minus direction CCW"));
	settings.setValue("M19", _TR("<b>M19</b> - oriented spindle stop"));
	settings.setValue("M20", _TR("<b>M20</b> - tailstock barrier OFF"));
	settings.setValue("M21", _TR("<b>M21</b> - tailstock barrier ON"));
	settings.setValue("M22", _TR("<b>M22</b> - chamfering OFF (for thread cutting cycle)"));
	settings.setValue("M23", _TR("<b>M23</b> - chamfering ON (for thread cutting cycle)"));
	settings.setValue("M24", _TR("<b>M24</b> - chuck barrier OFF"));
	settings.setValue("M25", _TR("<b>M25</b> - chuck barrier ON"));
	settings.setValue("M26", _TR("<b>M26</b> - thread lead along Z-axis"));
	settings.setValue("M27", _TR("<b>M27</b> - thread lead along X-axis"));
	settings.setValue("M28", _TR("<b>M28</b> - tool interference check function OFF"));
	settings.setValue("M29", _TR("<b>M28</b> - tool interference check function ON"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));

	settings.setValue("M40", _TR("<b>M40</b> - spindle gear range neutral"));
	settings.setValue("M41", _TR("<b>M41</b> - spindle gear range 1"));
	settings.setValue("M42", _TR("<b>M42</b> - spindle gear range 2"));
	settings.setValue("M43", _TR("<b>M43</b> - spindle gear range 3"));
	settings.setValue("M44", _TR("<b>M44</b> - spindle gear range 4"));
	settings.setValue("M48", _TR("<b>M48</b> - spindle speed override ignore cancel"));
	settings.setValue("M49", _TR("<b>M49</b> - spindle speed override ignore"));

	settings.setValue("M52",
	                  _TR("<i>v.M</i> <b>M52</b> - mode of return to upper limit level" \
	                      "<br /><i>v.L</i> <b>M52</b> - "));
	settings.setValue("M53",
	                  _TR("<i>v.M</i> <b>M53</b> - mode of return to a specified point level set by G71" \
	                      "<br /><i>v.L</i> <b>M53</b> - "));
	settings.setValue("M54",
	                  _TR("<i>v.M</i> <b>M54</b> - mode of return to the point R level" \
	                      "<br /><i>v.L</i> <b>M54</b> - "));

	settings.setValue("M55", _TR("<b>M55</b> - tailstock spindle retract"));
	settings.setValue("M56", _TR("<b>M56</b> - tailstock spindle advanced"));
	settings.setValue("M58", _TR("<b>M58</b> - chucking pressure low"));
	settings.setValue("M59", _TR("<b>M59</b> - chucking pressure high"));

	settings.setValue("M60", _TR("<b>M60</b> - cancel of M61"));
	settings.setValue("M61",
	                  _TR("<b>M61</b> - Ignoring fixed rpm arrival in constant speed cutting"));
	settings.setValue("M62", _TR("<b>M62</b> - cancel of M64"));
	settings.setValue("M63", _TR("<b>M63</b> - ignoring spindle rotation M code answer"));
	settings.setValue("M64", _TR("<b>M64</b> - ignoring general M code answer"));
	settings.setValue("M65", _TR("<b>M65</b> - ignoring T code answer"));
	settings.setValue("M66", _TR("<b>M66</b> - turret indexing position free"));

	settings.setValue("M78", _TR("<b>M78</b> - steady rest unclamp"));
	settings.setValue("M79", _TR("<b>M79</b> - steady rest clamp"));
	settings.setValue("M83", _TR("<b>M83</b> - chuck clamp"));
	settings.setValue("M84", _TR("<b>M84</b> - chuck unclamp"));
	settings.setValue("M85",
	                  _TR("<b>M85</b> - no return to the cutting starting point after the completion of rough turning cycle (LAP)"));
	settings.setValue("M88", _TR("<b>M88</b> - air blower OFF"));
	settings.setValue("M89", _TR("<b>M89</b> - air blower ON"));
	settings.setValue("M90", _TR("<b>M90</b> - door/cover close"));
	settings.setValue("M91", _TR("<b>M91</b> - door/cover open"));

	settings.setValue("M98", _TR("<b>M98</b> - tailstock spindle thrust low"));
	settings.setValue("M99", _TR("<b>M99</b> - tailstock spindle thrust high"));
	settings.setValue("M109", _TR("<b>M109</b> - cancel of M110"));
	settings.setValue("M110", _TR("<b>M110</b> - C-axis joint"));

	settings.setValue("M122", _TR("<b>M122</b> - work rest retraction"));
	settings.setValue("M123", _TR("<b>M123</b> - Cwork rest advance"));

	settings.setValue("M130",
	                  _TR("<b>M130</b> - cutting feed; spindle rotating condition OFF"));
	settings.setValue("M131",
	                  _TR("<b>M131</b> - cutting feed; spindle rotating condition ON"));
	settings.setValue("M132", _TR("<b>M132</b> - single block ineffective"));
	settings.setValue("M133", _TR("<b>M133</b> - single block effective"));
	settings.setValue("M136", _TR("<b>M136</b> - feedrate override ineffective"));
	settings.setValue("M137", _TR("<i>v.M</i> <b>M137</b> - feedrate override effective" \
	                              "<br /><i>v.L</i> <b>M137</b> - touch setter interlock release ON"));
	settings.setValue("M138", _TR("<i>v.M</i> <b>M138</b> - dry run ineffective" \
	                              "<br /><i>v.L</i> <b>M138</b> - touch setter interlock release OFF"));
	settings.setValue("M139", _TR("<b>M139</b> - dry run effective"));
	settings.setValue("M140", _TR("<i>v.M</i> <b>M140</b> - slide hold ineffective" \
	                              "<br /><i>v.L</i> <b>M140</b> - main motor overload monitoring OFF"));
	settings.setValue("M141", _TR("<i>v.M</i> <b>M141</b> - slide hold effective" \
	                              "<br /><i>v.L</i> <b>M141</b> - main motor overload monitoring ON"));
	settings.setValue("M142", _TR("<b>M142</b> - coolant pressure low"));
	settings.setValue("M143", _TR("<b>M143</b> - coolant pressure high"));
	settings.setValue("M144", _TR("<b>M144</b> - additional coolant 1 OFF"));
	settings.setValue("M145", _TR("<b>M145</b> - additional coolant 1 ON"));
	settings.setValue("M146", _TR("<b>M146</b> - C-axis unclamp"));
	settings.setValue("M147", _TR("<b>M147</b> - C-axis clamp"));
	settings.setValue("M152", _TR("<b>M152</b> - M-tools spindle interlock ON"));
	settings.setValue("M153", _TR("<b>M153</b> - M-tools spindle interlock OFF"));
	settings.setValue("M156", _TR("<b>M156</b> - center work interlock OFF"));
	settings.setValue("M157", _TR("<b>M157</b> - center work interlock ON"));
	settings.setValue("M161", _TR("<b>M161</b> - feedrate override fix (100%)"));
	settings.setValue("M162", _TR("<b>M162</b> - cancel of M163"));
	settings.setValue("M163",
	                  _TR("<b>M163</b> - M-tools spindle speed override fix (100%)"));

	settings.setValue("M184", _TR("<b>M184</b> - chuck internal interlock release OFF"));
	settings.setValue("M185", _TR("<b>M185</b> - chuck internal interlock release ON"));
	settings.setValue("M186", _TR("<b>M186</b> - work rest base unclamp"));
	settings.setValue("M187", _TR("<b>M187</b> - work rest base clamp"));
	settings.setValue("M215", _TR("<b>M215</b> - load monitor G00 ignore OFF"));
	settings.setValue("M216", _TR("<b>M216</b> - load monitor G00 ignore ON"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02",
	                  _TR("<b>G02 X Y Z [I J K</b> | <b>L]</b> - circular interpolation CW"));
	settings.setValue("G03",
	                  _TR("<b>G03 X Y Z [I J K</b> | <b>L]</b> - circular interpolation CCW"));
	settings.setValue("G04", _TR("<b>G04 F</b>xxxx - dwell xxxx seconds"));
	settings.setValue("G09", _TR("<b>G09</b> - exact stop"));
	settings.setValue("G10", _TR("<b>G10</b> - cancel of G11"));
	settings.setValue("G11",
	                  _TR("<b>G11</b> - parallel and rotation shift of coordinate system"));

	settings.setValue("G13", _TR("<b>G13</b> - turret selection: Turret A"));
	settings.setValue("G14", _TR("<b>G14</b> - turret selection: Turret B"));
	settings.setValue("G15",
	                  _TR("<b>G15 H</b>xx - selection of work coordinate system no. xx, modal"));
	settings.setValue("G16",
	                  _TR("<b>G15 H</b>xx - selection of work coordinate system no. xx, one-shot"));

	settings.setValue("G17", _TR("<b>G17</b> - XY plane"));
	settings.setValue("G18", _TR("<b>G18</b> - ZX plane"));
	settings.setValue("G19", _TR("<b>G19</b> - YZ plane"));
	settings.setValue("G20", _TR("<b>G20</b> - inch input confirmation"));
	settings.setValue("G21", _TR("<b>G21</b> - metric input confirmation"));

	settings.setValue("G40",
	                  _TR("<b>G40</b> - tool nose/cutter radius compensation cancel"));
	settings.setValue("G41", _TR("<b>G41</b> - tool nose/cutter radius compensation left"));
	settings.setValue("G42", _TR("<b>G42</b> - tool nose/cutter radius compensation right"));
	settings.setValue("G50", _TR("<i>v.L</i> <b>G50 S</b>xxxx - maximum spindle speed" \
	                             "<br /><i>v.L</i> <b>G50 X Z</b> - zero point shift"));

	settings.setValue("G53", _TR("<i>v.M</i> <b>G53</b> - cancel tool length offset" \
	                             "<br /><i>v.L</i> <b>G53</b> - "));
	settings.setValue("G54",
	                  _TR("<i>v.M</i> <b>G54 H</b>xx - tool length offset X-axis, xx - offset no." \
	                      "<br /><i>v.L</i> <b>G54</b> - "));
	settings.setValue("G55",
	                  _TR("<i>v.M</i> <b>G55 H</b>xx - tool length offset Y-axis, xx - offset no." \
	                      "<br /><i>v.L</i> <b>G55</b> - "));
	settings.setValue("G56",
	                  _TR("<i>v.M</i> <b>G56 H</b>xx - tool length offset Z-axis, xx - offset no." \
	                      "<br /><i>v.L</i> <b>G56</b> - "));
	settings.setValue("G57",
	                  _TR("<i>v.M</i> <b>G57 H</b>xx - tool length offset 4-axis, xx - offset no." \
	                      "<br /><i>v.L</i> <b>G57</b> - "));
	settings.setValue("G58",
	                  _TR("<i>v.M</i> <b>G58 H</b>xx - tool length offset 5-axis, xx - offset no." \
	                      "<br /><i>v.L</i> <b>G58</b> - "));
	settings.setValue("G59",
	                  _TR("<i>v.M</i> <b>G59 H</b>xx - tool length offset 6-axis, xx - offset no." \
	                      "<br /><i>v.L</i> <b>G59</b> - "));

	settings.setValue("G61", _TR("<b>G61</b> - exact stop mode"));
	settings.setValue("G62", _TR("<b>G62</b> - programmable mirror image function"));
	settings.setValue("G17", _TR("<b>G17</b> - cutting mode"));


	settings.setValue("G71", _TR("<i>v.M</i> <b>G71 Z</b>xx - return level xx command" \
	                             "<br /><i>v.L</i> <b>G71</b> - "));
	settings.setValue("G73", _TR("<i>v.M</i> <b>G73</b> - high speed deep hole drilling" \
	                             "<br /><i>v.L</i> <b>G73</b> - "));
	settings.setValue("G74", _TR("<i>v.M</i> <b>G74</b> - reverse tapping" \
	                             "<br /><i>v.L</i> <b>G74</b> - "));
	settings.setValue("G76", _TR("<i>v.M</i> <b>G76</b> - fine boring" \
	                             "<br /><i>v.L</i> <b>G76</b> - "));
	settings.setValue("G75",
	                  _TR("<i>v.L</i> <b>G75 G01 [X</b> | <b>Z] L</b>xxxx - chamfering 45deg. xxxx - direction and size"));
	settings.setValue("G76",
	                  _TR("<i>v.L</i> <b>G76 G01 [X</b> | <b>Z] L</b>xxxx - rounding. xxxx - direction and size"));

	settings.setValue("G80", _TR("<i>v.M</i> <b>G80</b> - fixed cycle mode cancel" \
	                             "<br /><i>v.L</i> <b>G80</b> - end of shape designation (LAP)"));

	settings.setValue("G81", _TR("<i>v.M</i> <b>G81 R X Y Z</b> - drilling cycle" \
	                             "<br /><i>v.L</i> <b>G81</b> - start of longitudinal shape designation (LAP)"));

	settings.setValue("G82", _TR("<i>v.M</i> <b>G82 R X Y Z</b> - counter bore cycle" \
	                             "<br /><i>v.L</i> <b>G82</b> - start of transverse shape designation (LAP)"));

	settings.setValue("G83", _TR("<i>v.M</i> <b>G83 R X Y Z</b> - deep hole drilling cycle" \
	                             "<br /><i>v.L</i> <b>G83</b> - start of blank material shape definition (LAP)"));

	settings.setValue("G84", _TR("<i>v.M</i> <b>G84 R X Y Z</b> - tapping cycle" \
	                             "<br /><i>v.L</i> <b>G84</b> -   change of cutting conditions in bar turning cycle (LAP)"));

	settings.setValue("G85", _TR("<i>v.M</i> <b>G85 R X Y Z</b> - boring cycle" \
	                             "<br /><i>v.L</i> <b>G85</b> - call of rough bar turning cycle (LAP)"));

	settings.setValue("G86", _TR("<i>v.M</i> <b>G86 R X Y Z</b> - boring cycle" \
	                             "<br /><i>v.L</i> <b>G86</b> - call of rough copy turning cycle (LAP)"));

	settings.setValue("G87", _TR("<i>v.M</i> <b>G87 R X Y Z</b> - back boring cycle" \
	                             "<br /><i>v.L</i> <b>G87</b> - call finish turning cycle (LAP)"));

	settings.setValue("G88", _TR("<i>v.M</i> <b>G88 R X Y Z</b> - drilling cycle" \
	                             "<br /><i>v.L</i> <b>G88</b> - call of continuous thread cutting cycle (LAP)"));

	settings.setValue("G89", _TR("<i>v.M</i> <b>G89 R X Y Z</b> - boring cycle" \
	                             "<br /><i>v.L</i> <b>G89</b> - "));


	settings.setValue("G90", _TR("<b>G90</b> - absolute programming"));
	settings.setValue("G91", _TR("<b>G91</b> - incremental programming"));
	settings.setValue("G92", _TR("<b>G92</b> - setting of work coordinate system"));
	settings.setValue("G94", _TR("<b>G94</b> - feed per minute"));
	settings.setValue("G95", _TR("<b>G95</b> - feed per revolution"));
	settings.setValue("G96", _TR("<b>G96 S</b>xx - constant cutting speed xx"));
	settings.setValue("G97", _TR("<b>G97 S</b>xx - constant spindle speed xx"));

	settings.setValue("NCYL",
	                  _TR("<b>NCYL</b> - if specified in fixed cycle, positioning to the definied hole position is performed, but the cycle axis does not operate"));
	settings.setValue("NOEX",
	                  _TR("<b>NOEX</b> - if specified in fixed cycle, no axis movements may be performed"));

	settings.setValue("SIN", _TR("<b>SIN[</b>angle<b>]</b> - sine"));
	settings.setValue("COS", _TR("<b>COS[</b>angle<b>]</b> - cosine"));
	settings.setValue("TAN", _TR("<b>TAN[</b>angle<b>]</b> - tangent"));
	settings.setValue("ATAN", _TR("<b>ATAN[</b>angle<b>]</b> - arctangent 1 or 2"));
	settings.setValue("SQRT", _TR("<b>SQRT[</b>val<b>]</b> - square root"));
	settings.setValue("ABS", _TR("<b>ABS[</b>val<b>]</b> - absolute value"));
	settings.setValue("BIN", _TR("<b>BIN[</b>val<b>]</b> - decimal to binary conversion"));
	settings.setValue("BCD", _TR("<b>BCD[</b>val<b>]</b> - binary to decimal conversion"));
	settings.setValue("ROUND",
	                  _TR("<b>ROUND[</b>val<b>]</b> - integer implementation (rounding)"));
	settings.setValue("FIX",
	                  _TR("<b>FIX[</b>val<b>]</b> - integer implementation (truncation)"));
	settings.setValue("FUP",
	                  _TR("<b>FUP[</b>val<b>]</b> - integer implementation (raising)"));
	settings.setValue("DROUND",
	                  _TR("<b>DROUND[</b>val<b>]</b> - unit integer implementation (rounding)"));
	settings.setValue("DFIX",
	                  _TR("<b>DFIX[</b>val<b>]</b> - unit integer implementation (truncation)"));
	settings.setValue("DFUP",
	                  _TR("<b>DFUP[</b>val<b>]</b> - unit integer implementation (raising)"));
	settings.setValue("MOD", _TR("<b>MOD[</b>val<b>,</b>yy<b>]</b> - remainder of val/yy"));

	settings.setValue("VDIN", _TR("<b>VDIN[</b>xx<b>]</b> - imput variable no. xx"));
	settings.setValue("VDOUT", _TR("<b>VDOUT[</b>xx<b>]</b> - output variable no. xx"));
	settings.setValue("VUACM",
	                  _TR("<b>VUACM[</b>n<b>]='</b>text<b>'</b> - sub message for user definied alarms, n - subscript expression, text - max. 16 chracters"));

	settings.setValue("MODIN",
	                  _TR("<b>MODIN O</b>nnnn [<b>Q</b>]xx - subprogram call after axis movement, nnnn - prog. name, xx - number of repetitions"));
	settings.setValue("MODOUT", _TR("<b>MODOUT</b> - cancels last MODIN command"));

	settings.setValue("OMIT", _TR("<b>OMIT</b> - coordinate calculation function, omit"));
	settings.setValue("RSTRT",
	                  _TR("<b>RSTRT</b> - coordinate calculation function, restart"));

	settings.setValue("LAA", _TR("<b>LAA</b> - line at angle"));
	settings.setValue("ARC", _TR("<b>ARC</b> - arc"));
	settings.setValue("GRDX", _TR("<b>GRDX</b> - grid X"));
	settings.setValue("GRDY", _TR("<b>GRDY</b> - grid Y"));
	settings.setValue("DGRDX", _TR("<b>DGRDX</b> - double grid X"));
	settings.setValue("DGRDY", _TR("<b>DGRDY</b> - double grid Y"));
	settings.setValue("SQRX", _TR("<b>SQRX</b> - square X"));
	settings.setValue("SQRY", _TR("<b>SQRY</b> - square Y"));
	settings.setValue("BHC",
	                  _TR("<b>BHC X Y I J K </b> - bolt hole circle, X Y - circle center, I - radius, J - angle of first hole, K - no. of holes"));

	settings.setValue("EQ", _TR("<b>EQ</b> - equal to"));
	settings.setValue("NE", _TR("<b>NE</b> - not equal to"));
	settings.setValue("GT", _TR("<b>GT</b> - greather than"));
	settings.setValue("LE", _TR("<b>LE</b> - less than or equal to"));
	settings.setValue("LT", _TR("<b>LT</b> - less than"));
	settings.setValue("GE", _TR("<b>GE</b> - greather than or equal to"));
	settings.setValue("IF",
	                  _TR("<b>IF[</b>condition<b>] N</b>xxxx - if condition is true goto block xxxx"));

	settings.setValue("TLFON", _TR("<b>TLFON</b> - tool life on"));
	settings.setValue("EMPTY", _TR("<b>EMPTY</b> - 'empty' value "));

	settings.setValue("SB", _TR("<b>SB=</b>xxxx - M-tool spindle speed xxxx"));
	settings.setValue("TG", _TR("<b>TG=</b>xx - tool group no. xx"));
	settings.setValue("OG", _TR("<b>OG=</b>xx - offset group no. xx"));
	settings.setValue("GOTO", _TR("<b>GOTO N</b>nnnn - jump to nnnn"));
	settings.setValue("CALL",
	                  _TR("<b>CALL O</b>nnnn [<b>Q</b>xx] - call subprogram nnnn, repeat it xx times"));
	settings.setValue("RTS", _TR("<b>RTS</b> - subprogram end"));
	settings.setValue("VLMON", _TR("<b>VLMON[</b>xx<b>]=</b>yy - load monitor"));


	settings.setValue("VATOL",
	                  _TR("<b>VATOL</b> - active tool number, tool kind + tool number"));
	settings.setValue("VNTOL",
	                  _TR("<b>VNTOL</b> - next tool number, tool kind + tool number"));
	settings.setValue("VMLOK",
	                  _TR("<b>VMLOK</b> - equal zero if not in machine lock status"));
	settings.setValue("VPPCP", _TR("<b>VPPCP</b> - PPC parameter"));
	settings.setValue("VPLNO", _TR("<b>VPLNO</b> - PPC pallet number"));
	settings.setValue("VPLDT", _TR("<b>VPLDT</b> - PPC parameter bit data"));
	settings.setValue("VTLCN", _TR("<b>VTLCN</b> - active tool number"));
	settings.setValue("VTLNN", _TR("<b>VTLNN</b> - next tool number"));


	settings.setValue("VMCOD",
	                  _TR("<b>VMCOD[</b>xx<b>]</b> - present M code of group no. xx"));
	settings.setValue("VSCOD",
	                  _TR("<b>VSCOD</b> - command value of present spindle speed S"));
	settings.setValue("VDCOD",
	                  _TR("<b>VDCOD</b> - present cutter radius compensation offset number"));
	settings.setValue("VFCOD", _TR("<b>VFCOD</b> - command value of present feedrate F"));
	settings.setValue("VGCOD",
	                  _TR("<b>VGCOD[</b>xx<b>]</b> - present G code of group no. xx"));
	settings.setValue("VHCOD", _TR("<b>VHCOD</b> - present tool length offset number"));
	settings.setValue("VACOD", _TR("<b>VACOD</b> - coordinate system number"));

	settings.setValue("VZOFX",
	                  _TR("<i>v.M</i> <b>VZOFX[</b>xx<b>]</b> - zero offset no. xx of X-axis" \
	                      "<br /><i>v.L</i> <b>VZOFX</b> - zero offset of X-axis"));
	settings.setValue("VZOFZ",
	                  _TR("<i>v.M</i> <b>VZOFZ[</b>xx<b>]</b> - zero offset no. xx of Z-axis" \
	                      "<br /><i>v.L</i> <b>VZOFZ</b> - zero offset of Z-axis"));
	settings.setValue("VZOFY",
	                  _TR("<i>v.M</i> <b>VZOFY[</b>xx<b>]</b> - zero offset no. xx of Y-axis"));
	settings.setValue("VZOFW", _TR("<i>v.L</i> <b>VZOFW</b> - zero offset of W-axis"));
	settings.setValue("VZOFC", _TR("<i>v.L</i> <b>VZOFC</b> - zero offset of C-axis"));
	settings.setValue("VZSHZ", _TR("<i>v.L</i> <b>VZSHZ</b> - zero shift of Z-axis"));
	settings.setValue("VZSHX", _TR("<i>v.L</i> <b>VZSHX</b> - zero shift of X-axis"));
	settings.setValue("VZSHC", _TR("<i>v.L</i> <b>VZSHC</b> - zero shift of C-axis"));
	settings.setValue("VTOFZ",
	                  _TR("<i>v.L</i> <b>VTOFZ[</b>xx<b>]</b> - tool offset no. xx of Z-axis"));
	settings.setValue("VTOFX",
	                  _TR("<i>v.L</i> <b>VTOFX[</b>xx<b>]</b> - tool offset no. xx of X-axis"));
	settings.setValue("VRSTT", _TR("<b>VRSTT</b> - equal zero if not in restart state"));
	settings.setValue("VTOFH",
	                  _TR("<i>v.M</i> <b>VTOFH[</b>xx<b>]</b> - tool length, offset no. xx"));
	settings.setValue("VTOFD",
	                  _TR("<i>v.M</i> <b>VTOFD[</b>xx<b>]</b> - cutter radius, offset no. xx"));
	settings.setValue("VNCOM",
	                  _TR("<b>VNCOM[</b>xx<b>]</b> - communication for automation, no. xx"));
	//*******************************************************************************************************************//
	settings.endGroup(); //  OKUMA
	//*******************************************************************************************************************//

	//*******************************************************************************************************************//
	settings.beginGroup("FANUC");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));
	settings.setValue("M98", _TR("<b>M98 P</b>xxxx - macro xxxx call"));
	settings.setValue("M99", _TR("<b>M99</b> - subprogram end"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02",
	                  _TR("<b>G02 {X Y Z</b> | <b>U V W} {I J K</b> | <b>R} F</b> - circular interpolation CW; XYZ - end point (absolute); UW - end point (incremental); IJK - distance from start point to center; R - radius of arc"));
	settings.setValue("G03",
	                  _TR("<b>G03 {X Y Z</b> | <b>U V W} {I J K</b> | <b>R} F</b> - circular interpolation CCW; XYZ - end point (absolute); UW - end point (incremental); IJK - distance from start point to center; R - radius of arc"));
	settings.setValue("G04",
	                  _TR("<b>G04 X U P</b> - dwell XU - in seconds; P - in microseconds"));

	settings.setValue("G10",
	                  _TR("<b>G10 P X Z Y R Q</b> - change of offset value by program"));

	settings.setValue("G17", _TR("<b>G17</b> - XY plane"));
	settings.setValue("G18", _TR("<b>G18</b> - ZX plane"));
	settings.setValue("G19", _TR("<b>G19</b> - YZ plane"));

	settings.setValue("G20", _TR("<b>G20</b> - inch input"));
	settings.setValue("G21", _TR("<b>G21</b> - metric input"));
	settings.setValue("G27", _TR("<b>G27</b> - reference point return check"));
	settings.setValue("G28", _TR("<b>G28</b> - reference point return"));
	settings.setValue("G30",
	                  _TR("<b>G30 P</b>x - x = 2nd, 3rd, 4th reference point return"));
	settings.setValue("G31", _TR("<b>G31</b> - skip function"));

	settings.setValue("G40",
	                  _TR("<b>G40</b> - tool nose/cutter radius compensation cancel"));
	settings.setValue("G41", _TR("<b>G41</b> - tool nose/cutter radius compensation left"));
	settings.setValue("G42", _TR("<b>G42</b> - tool nose/cutter radius compensation right"));

	settings.setValue("G50", _TR("<i>v.T</i> <b>G50 S</b>xxxx - maximum spindle speed" \
	                             "<br /><b>G50 X Z</b> - zero point shift"));
	settings.setValue("G53", _TR("<b>G53</b> - machine coordinate system"));
	settings.setValue("G54", _TR("<b>G54</b> - settable zero offset 1"));
	settings.setValue("G55", _TR("<b>G55</b> - settable zero offset 2"));
	settings.setValue("G56", _TR("<b>G56</b> - settable zero offset 3"));
	settings.setValue("G57", _TR("<b>G57</b> - settable zero offset 4"));
	settings.setValue("G58", _TR("<b>G58</b> - settable zero offset 5"));
	settings.setValue("G59", _TR("<b>G59</b> - settable zero offset 6"));

	settings.setValue("G65", _TR("<b>G65 P</b>xxxx - macro xxxx call"));
	settings.setValue("G66", _TR("<b>G66 P</b>xxxx - macro xxxx modal call"));
	settings.setValue("G67", _TR("<b>G67</b> - macro modal call cancel"));

	settings.setValue("G70",
	                  _TR("<i>v.T</i> <b>G70 P Q</b> - finishing cycle; P - sequence number of the first block of finishing shape, Q - sequence number of last block"));
	settings.setValue("G71",
	                  _TR("<i>v.T</i> <b>G71 U R</b> - U - depth of cut, Q - escaping amount" \
	                      "<br /><i>v.T</i> <b>G71 P Q U W</b> - stock removal in turning; P - first sequence number of finishing shape, Q - last sequence number of finishing shape, U - X finishing allowance, W - Z finishing allowance"));
	settings.setValue("G72",
	                  _TR("<i>v.T</i> <b>G72 W R</b> - U - depth of cut, Q - escaping amount" \
	                      "<br /><i>v.T</i> <b>G72 P Q U W</b> - stock removal in facing; P - first sequence number of finishing shape, Q - last sequence number of finishing shape, U - X finishing allowance, W - Z finishing allowance"));
	settings.setValue("G73", _TR("<i>v.T</i> <b>G73 W R</b>" \
	                             "<br /><i>v.T</i> <b>G73 P Q U W</b> - pattern repeating"));
	settings.setValue("G74", _TR("<i>v.T</i> <b>G74 R</b>" \
	                             "<br /><i>v.T</i> <b>G74 {X Z U W} P Q R</b> - end face peck drilling cycle"));
	settings.setValue("G75", _TR("<i>v.T</i> <b>G75 R</b>" \
	                             "<br /><i>v.T</i> <b>G75 {X Z U W} P Q U W</b> - outer/internal diameter drilling cycle"));
	settings.setValue("G76", _TR("<i>v.T</i> <b>G76 P Q R</b>" \
	                             "<br /><i>v.T</i> <b>G76 {X Z U W} P Q U W</b> - multiple thread cutting cycle"));

	settings.setValue("G90", _TR("<b>G90</b> - absolute programming"));
	settings.setValue("G91", _TR("<b>G91</b> - incremental programming"));
	settings.setValue("G92", _TR("<b>G92 X Z</b> - coordinate system setting"));
	settings.setValue("G94", _TR("<b>G94</b> - feed per minute"));
	settings.setValue("G95", _TR("<b>G95</b> - feed per revolution"));
	settings.setValue("G96", _TR("<b>G96 S</b>xx - constant cutting speed xx"));
	settings.setValue("G97", _TR("<b>G97 S</b>xx - constant spindle speed xx"));
	settings.setValue("G98", _TR("<b>G98</b> - return to initial level"));
	settings.setValue("G99", _TR("<b>G99</b> - return to R level"));

	settings.setValue("EQ", _TR("<b>EQ</b> - equal to"));
	settings.setValue("NE", _TR("<b>NE</b> - not equal to"));
	settings.setValue("GT", _TR("<b>GT</b> - greather than"));
	settings.setValue("LE", _TR("<b>LE</b> - less than or equal to"));
	settings.setValue("LT", _TR("<b>LT</b> - less than"));
	settings.setValue("GE", _TR("<b>GE</b> - greather than or equal to"));
	settings.setValue("IF",
	                  _TR("<b>IF[</b>condition<b>]</b>do something - if condition is true do something"));
	settings.setValue("GOTO", _TR("<b>GOTO</b>nnnn - jump to block nnnn"));

	settings.setValue("WHILE",
	                  _TR("<b>WHILE[</b>condition<b>] DO</b>n <br />...<br />commands<br />... <br /><b>END</b>n  - loop - while condition true do commands beetwen DOn and ENDn"));
	settings.setValue("END", _TR("<b>END</b>n - end of WHILE DOn loop"));

	settings.setValue("EOR", _TR("<b>EOR</b> - exclusive OR"));
	settings.setValue("OR", _TR("<b>OR</b> - logical OR"));
	settings.setValue("AND", _TR("<b>AND</b> - logical AND"));
	settings.setValue("NOT", _TR("<b>NOT</b> - negation"));

	settings.setValue("SIN", _TR("<b>SIN[</b>angle<b>]</b> - sine"));
	settings.setValue("COS", _TR("<b>COS[</b>angle<b>]</b> - cosine"));
	settings.setValue("TAN", _TR("<b>TAN[</b>angle<b>]</b> - tangent"));
	settings.setValue("ATAN", _TR("<b>ATAN[</b>angle<b>]</b> - arctangent 1 or 2"));
	settings.setValue("SQRT", _TR("<b>SQRT[</b>val<b>]</b> - square root"));
	settings.setValue("ABS", _TR("<b>ABS[</b>val<b>]</b> - absolute value"));
	settings.setValue("BIN", _TR("<b>BIN[</b>val<b>]</b> - decimal to binary conversion"));
	settings.setValue("BCD", _TR("<b>BCD[</b>val<b>]</b> - binary to decimal conversion"));
	settings.setValue("ROUND",
	                  _TR("<b>ROUND[</b>val<b>]</b> - integer implementation (rounding)"));
	settings.setValue("FIX",
	                  _TR("<b>FIX[</b>val<b>]</b> - integer implementation (truncation)"));
	settings.setValue("FUP",
	                  _TR("<b>FUP[</b>val<b>]</b> - integer implementation (raising)"));
	//*******************************************************************************************************************//
	settings.endGroup();  //  FANUC
	//*******************************************************************************************************************//

	//*******************************************************************************************************************//
	settings.beginGroup("SINUMERIK");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));
	settings.setValue("M51", _TR("<b>M51</b> - thro spindle coolant on"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02", _TR("<b>G02</b> - circular interpolation CW"));
	settings.setValue("G03", _TR("<b>G03</b> - circular interpolation CCW"));
	settings.setValue("G04", _TR("<b>G04 F</b>xxxx - dwell xxxx seconds"));

	settings.setValue("G17", _TR("<b>G17</b> - XY plane"));
	settings.setValue("G18", _TR("<b>G18</b> - ZX plane"));
	settings.setValue("G19", _TR("<b>G19</b> - YZ plane"));

	settings.setValue("G40",
	                  _TR("<b>G40</b> - tool nose/cutter radius compensation cancel"));
	settings.setValue("G41", _TR("<b>G41</b> - tool nose/cutter radius compensation left"));
	settings.setValue("G42", _TR("<b>G42</b> - tool nose/cutter radius compensation right"));

	settings.setValue("G53", _TR("<b>G53</b> - machine coordinate system"));
	settings.setValue("G54", _TR("<b>G54</b> - settable zero offset 1"));
	settings.setValue("G55", _TR("<b>G55</b> - settable zero offset 2"));
	settings.setValue("G56", _TR("<b>G56</b> - settable zero offset 3"));
	settings.setValue("G57", _TR("<b>G57</b> - settable zero offset 4"));
	settings.setValue("G58",
	                  _TR("<b>G58</b> - programmable offset, absolute axial substitution"));
	settings.setValue("G59",
	                  _TR("<b>G59</b> - programmable offset, additive axial substitution"));

	settings.setValue("G90", _TR("<b>G90</b> - absolute programming"));
	settings.setValue("G91", _TR("<b>G91</b> - incremental programming"));
	settings.setValue("G94", _TR("<b>G94</b> - feed per minute"));
	settings.setValue("G95", _TR("<b>G95</b> - feed per revolution"));
	settings.setValue("G96", _TR("<b>G96 S</b>xx - constant cutting speed xx"));
	settings.setValue("G97", _TR("<b>G97 S</b>xx - constant spindle speed xx"));


	settings.setValue("@100", _TR("<b>@100 K</b>nnnn - jump to block number nnnn"));
	settings.setValue("@121",
	                  _TR("<b>@121 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy not equal to xx jump to nnnn"));
	settings.setValue("@122",
	                  _TR("<b>@122 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy equal to xx jump to nnnn"));
	settings.setValue("@123",
	                  _TR("<b>@123 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is less than or equal to xx jump to nnnn"));
	settings.setValue("@124",
	                  _TR("<b>@124 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is less than xx jump to nnnn"));
	settings.setValue("@125",
	                  _TR("<b>@125 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is greather than or equal to xx jump to nnnn"));
	settings.setValue("@126",
	                  _TR("<b>@126 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is greather than xx jump to nnnn"));

	settings.setValue("@620", _TR("<b>@620 R</b>xx - increment value in param. xx"));
	settings.setValue("@621", _TR("<b>@621 R</b>xx - decrement value in param. xx"));
	settings.setValue("@622", _TR("<b>@622 R</b>xx - 'trunc' value in param. xx"));
	settings.setValue("@630",
	                  _TR("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - sine of angle yy is stored in param. xx"));
	settings.setValue("@631",
	                  _TR("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - cosine of angle yy is stored in param. xx"));
	settings.setValue("@632",
	                  _TR("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - tangent of angle yy is stored in param. xx"));
	settings.setValue("@634",
	                  _TR("<b>@634 R</b>xx <b>R</b>|<b>K</b>yy - arc sine of angle yy is stored in param. xx"));

	settings.setValue("@714", _TR("<b>@714</b> - stop decoding, until buffer is empty"));
	//*******************************************************************************************************************//
	settings.endGroup();  //  SINUMERIK
	//*******************************************************************************************************************//

	//*******************************************************************************************************************//
	settings.beginGroup("SINUMERIK_840");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M07", _TR("<b>M07</b> - thro spindle coolant on"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));
	settings.setValue("M50", _TR("<b>M50</b> - shower coolant on"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02", _TR("<b>G02</b> - circular interpolation CW"));
	settings.setValue("G03", _TR("<b>G03</b> - circular interpolation CCW"));
	settings.setValue("G04", _TR("<b>G04 F</b>xxxx - dwell xxxx seconds"));

	settings.setValue("G17", _TR("<b>G17</b> - XY plane"));
	settings.setValue("G18", _TR("<b>G18</b> - ZX plane"));
	settings.setValue("G19", _TR("<b>G19</b> - YZ plane"));

	settings.setValue("G53",
	                  _TR("<b>G53</b> - Suppression of current frames: Programmable frame including system frame for TOROT and TOFRAME and active settable frame G54 ... G599."));
	settings.setValue("G54", _TR("<b>G54</b> - settable zero offset 1"));
	settings.setValue("G55", _TR("<b>G55</b> - settable zero offset 2"));
	settings.setValue("G56", _TR("<b>G56</b> - settable zero offset 3"));
	settings.setValue("G57", _TR("<b>G57</b> - settable zero offset 4"));
	settings.setValue("G58",
	                  _TR("<b>G58</b> - programmable offset, absolute axial substitution"));
	settings.setValue("G59",
	                  _TR("<b>G59</b> - programmable offset, additive axial substitution"));


	settings.setValue("G40",
	                  _TR("<b>G40</b> - tool nose/cutter radius compensation cancel"));
	settings.setValue("G41", _TR("<b>G41</b> - tool nose/cutter radius compensation left"));
	settings.setValue("G42", _TR("<b>G42</b> - tool nose/cutter radius compensation right"));

	settings.setValue("G60", _TR("<b>G60</b> - Velocity reduction, exact positioning"));
	settings.setValue("G64", _TR("<b>G64</b> - Continuous-path mode"));

	settings.setValue("G90", _TR("<b>G90</b> - absolute programming"));
	settings.setValue("G91", _TR("<b>G91</b> - incremental programming"));
	settings.setValue("G94", _TR("<b>G94</b> - feed per minute"));
	settings.setValue("G95", _TR("<b>G95</b> - feed per revolution"));
	settings.setValue("G96", _TR("<b>G96 S</b>xx - constant cutting speed xx"));
	settings.setValue("G97", _TR("<b>G97 S</b>xx - constant spindle speed xx"));

	settings.setValue("G153",
	                  _TR("<b>G153</b> - suppression as for G53 and including all channel-specific and/or NCU-global basic frame"));
	settings.setValue("G500",
	                  _TR("<b>G500</b> - deactivate all settable G54-G57 frames if G500 does not contain a value"));

	settings.setValue("SUPA",
	                  _TR("<b>SUPA</b> - suppression as for G153 and including system frames for actual-value setting, scratching, zero offset external, PAROT including handwheel offsets (DRF), [zero offset external], overlaid motion"));

	settings.setValue("IF",
	                  _TR("<b>IF(</b>condition<b>)</b> nnnn - if condition is true goto block nnnn or label nnnn:"));
	settings.setValue("GOTO",
	                  _TR("<b>GOTO</b> nnnn - jump forward and if block not found jump backward to block nnnn or label nnnn:"));
	settings.setValue("GOTOF",
	                  _TR("<b>GOTOF</b> nnnn - jump forward to block nnnn or label nnnn:"));
	settings.setValue("GOTOB",
	                  _TR("<b>GOTOB</b> nnnn - jump backward to block nnnn or label nnnn:"));

	settings.setValue("STOPRE", _TR("<b>STOPRE</b> - stop decoding, until buffer is empty"));

	settings.setValue("DIAMOF",
	                  _TR("<b>DIAMOF</b> - diameter programming OFF; radius programming for G90/G91"));
	settings.setValue("DIAMON",
	                  _TR("<b>DIAMON</b> - diameter programming ON; diameter programming for G90/G91"));
	settings.setValue("DIAM",
	                  _TR("<b>DIAM90</b> - diameter programming for G90; radius programming for G91"));


	settings.setValue("RET", _TR("<b>RET</b> - subprogram end"));
	//*******************************************************************************************************************//
	settings.endGroup(); //  SINUMERIK_840
	//*******************************************************************************************************************//

	//*******************************************************************************************************************//
	settings.beginGroup("HEIDENHAIN");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M07", _TR("<b>M07</b> - thro spindle coolant on"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));
	settings.setValue("M50", _TR("<b>M50</b> - shower coolant on"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02", _TR("<b>G02</b> - circular interpolation CW"));
	settings.setValue("G03", _TR("<b>G03</b> - circular interpolation CCW"));
	settings.setValue("G04", _TR("<b>G04 F</b>xxxx - dwell xxxx seconds"));
	//*******************************************************************************************************************//
	settings.endGroup();  //  HEIDENHAIN
	//*******************************************************************************************************************//

	//*******************************************************************************************************************//
	settings.beginGroup("HEIDENHAIN_ISO");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M07", _TR("<b>M07</b> - thro spindle coolant on"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));
	settings.setValue("M50", _TR("<b>M50</b> - shower coolant on"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02", _TR("<b>G02</b> - circular interpolation CW"));
	settings.setValue("G03", _TR("<b>G03</b> - circular interpolation CCW"));
	settings.setValue("G04", _TR("<b>G04 F</b>xxxx - dwell xxxx seconds"));

	settings.setValue("G17", _TR("<b>G17</b> - XY plane"));
	settings.setValue("G18", _TR("<b>G18</b> - ZX plane"));
	settings.setValue("G19", _TR("<b>G19</b> - YZ plane"));
	//*******************************************************************************************************************//
	settings.endGroup();  //  HEIDENHAIN_ISO
	//*******************************************************************************************************************//

	//*******************************************************************************************************************//
	settings.beginGroup("LinuxCNC");
	//*******************************************************************************************************************//
	settings.setValue("M00", _TR("<b>M00</b> - program stop, unconditional"));
	settings.setValue("M01", _TR("<b>M01</b> - optional program stop"));
	settings.setValue("M03", _TR("<b>M03</b> - start spindle CW"));
	settings.setValue("M04", _TR("<b>M04</b> - start spindle CCW"));
	settings.setValue("M05", _TR("<b>M05</b> - spindle stop"));
	settings.setValue("M06", _TR("<b>M06</b> - tool change"));
	settings.setValue("M07", _TR("<b>M07</b> - thro spindle coolant on"));
	settings.setValue("M08", _TR("<b>M08</b> - coolant on"));
	settings.setValue("M09", _TR("<b>M09</b> - coolant off"));
	settings.setValue("M30", _TR("<b>M30</b> - end of program"));
	settings.setValue("M50", _TR("<b>M50</b> - shower coolant on"));

	settings.setValue("G00", _TR("<b>G00</b> - rapid move - positioning"));
	settings.setValue("G01", _TR("<b>G01</b> - linear interpolation"));
	settings.setValue("G02", _TR("<b>G02</b> - circular interpolation CW"));
	settings.setValue("G03", _TR("<b>G03</b> - circular interpolation CCW"));
	settings.setValue("G64", _TR("<b>G64 P {Q}</b> - Path Blending"));
	//*******************************************************************************************************************//
	settings.endGroup();  //  LinuxCNC
	//*******************************************************************************************************************//

	return fileName;
}

#endif // TOOLTIPS_H
