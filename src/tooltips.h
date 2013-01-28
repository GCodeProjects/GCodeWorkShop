/***************************************************************************
 *   Copyright (C) 2006-2013 by Artur Kozioł                               *
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


QString writeTooltipFile()
{

    QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "EdytorNC", "EdytorNC");
    QString config_dir = QFileInfo(cfg.fileName()).absolutePath() + "/";

    QString fileName = config_dir + "cnc_tips_" + QLocale::system().name() + ".txt";

    QSettings settings(fileName, QSettings::IniFormat);


    //*******************************************************************************************************************//
    settings.beginGroup("OKUMA");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M02", QT_TR_NOOP("<b>M02</b> - end of program"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M12", QT_TR_NOOP("<b>M12</b> - M-tool spindle STOP"));
    settings.setValue("M13", QT_TR_NOOP("<b>M13</b> - M-tool spindle CW"));
    settings.setValue("M14", QT_TR_NOOP("<b>M14</b> - M-tool spindle CCW"));
    settings.setValue("M15", QT_TR_NOOP("<b>M15</b> - B or C-axis positioning, plus direction CW"));
    settings.setValue("M16", QT_TR_NOOP("<b>M16</b> - B or C-axis positioning, minus direction CCW"));
    settings.setValue("M19", QT_TR_NOOP("<b>M19</b> - oriented spindle stop"));
    settings.setValue("M20", QT_TR_NOOP("<b>M20</b> - tailstock barrier OFF"));
    settings.setValue("M21", QT_TR_NOOP("<b>M21</b> - tailstock barrier ON"));
    settings.setValue("M22", QT_TR_NOOP("<b>M22</b> - chamfering OFF (for thread cutting cycle)"));
    settings.setValue("M23", QT_TR_NOOP("<b>M23</b> - chamfering ON (for thread cutting cycle)"));
    settings.setValue("M24", QT_TR_NOOP("<b>M24</b> - chuck barrier OFF"));
    settings.setValue("M25", QT_TR_NOOP("<b>M25</b> - chuck barrier ON"));
    settings.setValue("M26", QT_TR_NOOP("<b>M26</b> - thread lead along Z-axis"));
    settings.setValue("M27", QT_TR_NOOP("<b>M27</b> - thread lead along X-axis"));
    settings.setValue("M28", QT_TR_NOOP("<b>M28</b> - tool interference check function OFF"));
    settings.setValue("M29", QT_TR_NOOP("<b>M28</b> - tool interference check function ON"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));

    settings.setValue("M40", QT_TR_NOOP("<b>M40</b> - spindle gear range neutral"));
    settings.setValue("M41", QT_TR_NOOP("<b>M41</b> - spindle gear range 1"));
    settings.setValue("M42", QT_TR_NOOP("<b>M42</b> - spindle gear range 2"));
    settings.setValue("M43", QT_TR_NOOP("<b>M43</b> - spindle gear range 3"));
    settings.setValue("M44", QT_TR_NOOP("<b>M44</b> - spindle gear range 4"));
    settings.setValue("M48", QT_TR_NOOP("<b>M48</b> - spindle speed override ignore cancel"));
    settings.setValue("M49", QT_TR_NOOP("<b>M49</b> - spindle speed override ignore"));

    settings.setValue("M52", QT_TR_NOOP("<i>v.M</i> <b>M52</b> - mode of return to upper limit level" \
                                        "<br /><i>v.L</i> <b>M52</b> - "));
    settings.setValue("M53", QT_TR_NOOP("<i>v.M</i> <b>M53</b> - mode of return to a specified point level set by G71" \
                                        "<br /><i>v.L</i> <b>M53</b> - "));
    settings.setValue("M54", QT_TR_NOOP("<i>v.M</i> <b>M54</b> - mode of return to the point R level" \
                                        "<br /><i>v.L</i> <b>M54</b> - "));

    settings.setValue("M55", QT_TR_NOOP("<b>M55</b> - tailstock spindle retract"));
    settings.setValue("M56", QT_TR_NOOP("<b>M56</b> - tailstock spindle advanced"));
    settings.setValue("M58", QT_TR_NOOP("<b>M58</b> - chucking pressure low"));
    settings.setValue("M59", QT_TR_NOOP("<b>M59</b> - chucking pressure high"));

    settings.setValue("M60", QT_TR_NOOP("<b>M60</b> - cancel of M61"));
    settings.setValue("M61", QT_TR_NOOP("<b>M61</b> - Ignoring fixed rpm arrival in constant speed cutting"));
    settings.setValue("M62", QT_TR_NOOP("<b>M62</b> - cancel of M64"));
    settings.setValue("M63", QT_TR_NOOP("<b>M63</b> - ignoring spindle rotation M code answer"));
    settings.setValue("M64", QT_TR_NOOP("<b>M64</b> - ignoring general M code answer"));
    settings.setValue("M65", QT_TR_NOOP("<b>M65</b> - ignoring T code answer"));
    settings.setValue("M66", QT_TR_NOOP("<b>M66</b> - turret indexing position free"));

    settings.setValue("M78", QT_TR_NOOP("<b>M78</b> - steady rest unclamp"));
    settings.setValue("M79", QT_TR_NOOP("<b>M79</b> - steady rest clamp"));
    settings.setValue("M83", QT_TR_NOOP("<b>M83</b> - chuck clamp"));
    settings.setValue("M84", QT_TR_NOOP("<b>M84</b> - chuck unclamp"));
    settings.setValue("M85", QT_TR_NOOP("<b>M85</b> - no return to the cutting starting point after the completion of rough turning cycle (LAP)"));
    settings.setValue("M88", QT_TR_NOOP("<b>M88</b> - air blower OFF"));
    settings.setValue("M89", QT_TR_NOOP("<b>M89</b> - air blower ON"));
    settings.setValue("M90", QT_TR_NOOP("<b>M90</b> - door/cover close"));
    settings.setValue("M91", QT_TR_NOOP("<b>M91</b> - door/cover open"));

    settings.setValue("M98", QT_TR_NOOP("<b>M98</b> - tailstock spindle thrust low"));
    settings.setValue("M99", QT_TR_NOOP("<b>M99</b> - tailstock spindle thrust high"));
    settings.setValue("M109", QT_TR_NOOP("<b>M109</b> - cancel of M110"));
    settings.setValue("M110", QT_TR_NOOP("<b>M110</b> - C-axis joint"));

    settings.setValue("M122", QT_TR_NOOP("<b>M122</b> - work rest retraction"));
    settings.setValue("M123", QT_TR_NOOP("<b>M123</b> - Cwork rest advance"));

    settings.setValue("M130", QT_TR_NOOP("<b>M130</b> - cutting feed; spindle rotating condition OFF"));
    settings.setValue("M131", QT_TR_NOOP("<b>M131</b> - cutting feed; spindle rotating condition ON"));
    settings.setValue("M132", QT_TR_NOOP("<b>M132</b> - single block ineffective"));
    settings.setValue("M133", QT_TR_NOOP("<b>M133</b> - single block effective"));
    settings.setValue("M136", QT_TR_NOOP("<b>M136</b> - feedrate override ineffective"));
    settings.setValue("M137", QT_TR_NOOP("<i>v.M</i> <b>M137</b> - feedrate override effective" \
                                         "<br /><i>v.L</i> <b>M137</b> - touch setter interlock release ON"));
    settings.setValue("M138", QT_TR_NOOP("<i>v.M</i> <b>M138</b> - dry run ineffective" \
                                         "<br /><i>v.L</i> <b>M138</b> - touch setter interlock release OFF"));
    settings.setValue("M139", QT_TR_NOOP("<b>M139</b> - dry run effective"));
    settings.setValue("M140", QT_TR_NOOP("<i>v.M</i> <b>M140</b> - slide hold ineffective" \
                                         "<br /><i>v.L</i> <b>M140</b> - main motor overload monitoring OFF"));
    settings.setValue("M141", QT_TR_NOOP("<i>v.M</i> <b>M141</b> - slide hold effective" \
                                         "<br /><i>v.L</i> <b>M141</b> - main motor overload monitoring ON"));
    settings.setValue("M142", QT_TR_NOOP("<b>M142</b> - coolant pressure low"));
    settings.setValue("M143", QT_TR_NOOP("<b>M143</b> - coolant pressure high"));
    settings.setValue("M144", QT_TR_NOOP("<b>M144</b> - additional coolant 1 OFF"));
    settings.setValue("M145", QT_TR_NOOP("<b>M145</b> - additional coolant 1 ON"));
    settings.setValue("M146", QT_TR_NOOP("<b>M146</b> - C-axis unclamp"));
    settings.setValue("M147", QT_TR_NOOP("<b>M147</b> - C-axis clamp"));
    settings.setValue("M152", QT_TR_NOOP("<b>M152</b> - M-tools spindle interlock ON"));
    settings.setValue("M153", QT_TR_NOOP("<b>M153</b> - M-tools spindle interlock OFF"));
    settings.setValue("M156", QT_TR_NOOP("<b>M156</b> - center work interlock OFF"));
    settings.setValue("M157", QT_TR_NOOP("<b>M157</b> - center work interlock ON"));
    settings.setValue("M161", QT_TR_NOOP("<b>M161</b> - feedrate override fix (100%)"));
    settings.setValue("M162", QT_TR_NOOP("<b>M162</b> - cancel of M163"));
    settings.setValue("M163", QT_TR_NOOP("<b>M163</b> - M-tools spindle speed override fix (100%)"));

    settings.setValue("M184", QT_TR_NOOP("<b>M184</b> - chuck internal interlock release OFF"));
    settings.setValue("M185", QT_TR_NOOP("<b>M185</b> - chuck internal interlock release ON"));
    settings.setValue("M186", QT_TR_NOOP("<b>M186</b> - work rest base unclamp"));
    settings.setValue("M187", QT_TR_NOOP("<b>M187</b> - work rest base clamp"));
    settings.setValue("M215", QT_TR_NOOP("<b>M215</b> - load monitor G00 ignore OFF"));
    settings.setValue("M216", QT_TR_NOOP("<b>M216</b> - load monitor G00 ignore ON"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02 X Y Z [I J K</b> | <b>L]</b> - circular interpolation CW"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03 X Y Z [I J K</b> | <b>L]</b> - circular interpolation CCW"));
    settings.setValue("G04", QT_TR_NOOP("<b>G04 F</b>xxxx - dwell xxxx seconds"));
    settings.setValue("G09", QT_TR_NOOP("<b>G09</b> - exact stop"));
    settings.setValue("G10", QT_TR_NOOP("<b>G10</b> - cancel of G11"));
    settings.setValue("G11", QT_TR_NOOP("<b>G11</b> - parallel and rotation shift of coordinate system"));

    settings.setValue("G13", QT_TR_NOOP("<b>G13</b> - turret selection: Turret A"));
    settings.setValue("G14", QT_TR_NOOP("<b>G14</b> - turret selection: Turret B"));
    settings.setValue("G15", QT_TR_NOOP("<b>G15 H</b>xx - selection of work coordinate system no. xx, modal"));
    settings.setValue("G16", QT_TR_NOOP("<b>G15 H</b>xx - selection of work coordinate system no. xx, one-shot"));

    settings.setValue("G17", QT_TR_NOOP("<b>G17</b> - XY plane"));
    settings.setValue("G18", QT_TR_NOOP("<b>G18</b> - ZX plane"));
    settings.setValue("G19", QT_TR_NOOP("<b>G19</b> - YZ plane"));
    settings.setValue("G20", QT_TR_NOOP("<b>G20</b> - inch input confirmation"));
    settings.setValue("G21", QT_TR_NOOP("<b>G21</b> - metric input confirmation"));

    settings.setValue("G40", QT_TR_NOOP("<b>G40</b> - tool nose/cutter radius compensation cancel"));
    settings.setValue("G41", QT_TR_NOOP("<b>G41</b> - tool nose/cutter radius compensation left"));
    settings.setValue("G42", QT_TR_NOOP("<b>G42</b> - tool nose/cutter radius compensation right"));
    settings.setValue("G50", QT_TR_NOOP("<i>v.L</i> <b>G50 S</b>xxxx - maximum spindle speed" \
                                        "<br /><i>v.L</i> <b>G50 X Z</b> - zero point shift"));

    settings.setValue("G53", QT_TR_NOOP("<i>v.M</i> <b>G53</b> - cancel tool length offset" \
                                        "<br /><i>v.L</i> <b>G53</b> - "));
    settings.setValue("G54", QT_TR_NOOP("<i>v.M</i> <b>G54 H</b>xx - tool length offset X-axis, xx - offset no." \
                                        "<br /><i>v.L</i> <b>G54</b> - "));
    settings.setValue("G55", QT_TR_NOOP("<i>v.M</i> <b>G55 H</b>xx - tool length offset Y-axis, xx - offset no." \
                                        "<br /><i>v.L</i> <b>G55</b> - "));
    settings.setValue("G56", QT_TR_NOOP("<i>v.M</i> <b>G56 H</b>xx - tool length offset Z-axis, xx - offset no." \
                                        "<br /><i>v.L</i> <b>G56</b> - "));
    settings.setValue("G57", QT_TR_NOOP("<i>v.M</i> <b>G57 H</b>xx - tool length offset 4-axis, xx - offset no." \
                                        "<br /><i>v.L</i> <b>G57</b> - "));
    settings.setValue("G58", QT_TR_NOOP("<i>v.M</i> <b>G58 H</b>xx - tool length offset 5-axis, xx - offset no." \
                                        "<br /><i>v.L</i> <b>G58</b> - "));
    settings.setValue("G59", QT_TR_NOOP("<i>v.M</i> <b>G59 H</b>xx - tool length offset 6-axis, xx - offset no." \
                                        "<br /><i>v.L</i> <b>G59</b> - "));

    settings.setValue("G61", QT_TR_NOOP("<b>G61</b> - exact stop mode"));
    settings.setValue("G62", QT_TR_NOOP("<b>G62</b> - programmable mirror image function"));
    settings.setValue("G17", QT_TR_NOOP("<b>G17</b> - cutting mode"));


    settings.setValue("G71", QT_TR_NOOP("<i>v.M</i> <b>G71 Z</b>xx - return level xx command" \
                                        "<br /><i>v.L</i> <b>G71</b> - "));
    settings.setValue("G73", QT_TR_NOOP("<i>v.M</i> <b>G73</b> - high speed deep hole drilling" \
                                        "<br /><i>v.L</i> <b>G73</b> - "));
    settings.setValue("G74", QT_TR_NOOP("<i>v.M</i> <b>G74</b> - reverse tapping" \
                                        "<br /><i>v.L</i> <b>G74</b> - "));
    settings.setValue("G76", QT_TR_NOOP("<i>v.M</i> <b>G76</b> - fine boring" \
                                        "<br /><i>v.L</i> <b>G76</b> - "));
    settings.setValue("G75", QT_TR_NOOP("<i>v.L</i> <b>G75 G01 [X</b> | <b>Z] L</b>xxxx - chamfering 45deg. xxxx - direction and size"));
    settings.setValue("G76", QT_TR_NOOP("<i>v.L</i> <b>G76 G01 [X</b> | <b>Z] L</b>xxxx - rounding. xxxx - direction and size"));

    settings.setValue("G80", QT_TR_NOOP("<i>v.M</i> <b>G80</b> - fixed cycle mode cancel" \
                                        "<br /><i>v.L</i> <b>G80</b> - end of shape designation (LAP)"));

    settings.setValue("G81", QT_TR_NOOP("<i>v.M</i> <b>G81 R X Y Z</b> - drilling cycle" \
                                        "<br /><i>v.L</i> <b>G81</b> - start of longitudinal shape designation (LAP)"));

    settings.setValue("G82", QT_TR_NOOP("<i>v.M</i> <b>G82 R X Y Z</b> - counter bore cycle" \
                                        "<br /><i>v.L</i> <b>G82</b> - start of transverse shape designation (LAP)"));

    settings.setValue("G83", QT_TR_NOOP("<i>v.M</i> <b>G83 R X Y Z</b> - deep hole drilling cycle" \
                                        "<br /><i>v.L</i> <b>G83</b> - start of blank material shape definition (LAP)"));

    settings.setValue("G84", QT_TR_NOOP("<i>v.M</i> <b>G84 R X Y Z</b> - tapping cycle" \
                                        "<br /><i>v.L</i> <b>G84</b> -   change of cutting conditions in bar turning cycle (LAP)"));

    settings.setValue("G85", QT_TR_NOOP("<i>v.M</i> <b>G85 R X Y Z</b> - boring cycle" \
                                        "<br /><i>v.L</i> <b>G85</b> - call of rough bar turning cycle (LAP)"));

    settings.setValue("G86", QT_TR_NOOP("<i>v.M</i> <b>G86 R X Y Z</b> - boring cycle" \
                                        "<br /><i>v.L</i> <b>G86</b> - call of rough copy turning cycle (LAP)"));

    settings.setValue("G87", QT_TR_NOOP("<i>v.M</i> <b>G87 R X Y Z</b> - back boring cycle" \
                                        "<br /><i>v.L</i> <b>G87</b> - call finish turning cycle (LAP)"));

    settings.setValue("G88", QT_TR_NOOP("<i>v.M</i> <b>G88 R X Y Z</b> - drilling cycle" \
                                        "<br /><i>v.L</i> <b>G88</b> - call of continuous thread cutting cycle (LAP)"));

    settings.setValue("G89", QT_TR_NOOP("<i>v.M</i> <b>G89 R X Y Z</b> - boring cycle" \
                                        "<br /><i>v.L</i> <b>G89</b> - "));


    settings.setValue("G90", QT_TR_NOOP("<b>G90</b> - absolute programming"));
    settings.setValue("G91", QT_TR_NOOP("<b>G91</b> - incremental programming"));
    settings.setValue("G92", QT_TR_NOOP("<b>G92</b> - setting of work coordinate system"));
    settings.setValue("G94", QT_TR_NOOP("<b>G94</b> - feed per minute"));
    settings.setValue("G95", QT_TR_NOOP("<b>G95</b> - feed per revolution"));
    settings.setValue("G96", QT_TR_NOOP("<b>G96 S</b>xx - constant cutting speed xx"));
    settings.setValue("G97", QT_TR_NOOP("<b>G97 S</b>xx - constant spindle speed xx"));

    settings.setValue("NCYL", QT_TR_NOOP("<b>NCYL</b> - if specified in fixed cycle, positioning to the definied hole position is performed, but the cycle axis does not operate"));
    settings.setValue("NOEX", QT_TR_NOOP("<b>NOEX</b> - if specified in fixed cycle, no axis movements may be performed"));

    settings.setValue("SIN", QT_TR_NOOP("<b>SIN[</b>angle<b>]</b> - sine"));
    settings.setValue("COS", QT_TR_NOOP("<b>COS[</b>angle<b>]</b> - cosine"));
    settings.setValue("TAN", QT_TR_NOOP("<b>TAN[</b>angle<b>]</b> - tangent"));
    settings.setValue("ATAN", QT_TR_NOOP("<b>ATAN[</b>angle<b>]</b> - arctangent 1 or 2"));
    settings.setValue("SQRT", QT_TR_NOOP("<b>SQRT[</b>val<b>]</b> - square root"));
    settings.setValue("ABS", QT_TR_NOOP("<b>ABS[</b>val<b>]</b> - absolute value"));
    settings.setValue("BIN", QT_TR_NOOP("<b>BIN[</b>val<b>]</b> - decimal to binary conversion"));
    settings.setValue("BCD", QT_TR_NOOP("<b>BCD[</b>val<b>]</b> - binary to decimal conversion"));
    settings.setValue("ROUND", QT_TR_NOOP("<b>ROUND[</b>val<b>]</b> - integer implementation (rounding)"));
    settings.setValue("FIX", QT_TR_NOOP("<b>FIX[</b>val<b>]</b> - integer implementation (truncation)"));
    settings.setValue("FUP", QT_TR_NOOP("<b>FUP[</b>val<b>]</b> - integer implementation (raising)"));
    settings.setValue("DROUND", QT_TR_NOOP("<b>DROUND[</b>val<b>]</b> - unit integer implementation (rounding)"));
    settings.setValue("DFIX", QT_TR_NOOP("<b>DFIX[</b>val<b>]</b> - unit integer implementation (truncation)"));
    settings.setValue("DFUP", QT_TR_NOOP("<b>DFUP[</b>val<b>]</b> - unit integer implementation (raising)"));
    settings.setValue("MOD", QT_TR_NOOP("<b>MOD[</b>val<b>,</b>yy<b>]</b> - remainder of val/yy"));

    settings.setValue("VDIN", QT_TR_NOOP("<b>VDIN[</b>xx<b>]</b> - imput variable no. xx"));
    settings.setValue("VDOUT", QT_TR_NOOP("<b>VDOUT[</b>xx<b>]</b> - output variable no. xx"));
    settings.setValue("VUACM", QT_TR_NOOP("<b>VUACM[</b>n<b>]='</b>text<b>'</b> - sub message for user definied alarms, n - subscript expression, text - max. 16 chracters"));

    settings.setValue("MODIN", QT_TR_NOOP("<b>MODIN O</b>nnnn [<b>Q</b>]xx - subprogram call after axis movement, nnnn - prog. name, xx - number of repetitions"));
    settings.setValue("MODOUT", QT_TR_NOOP("<b>MODOUT</b> - cancels last MODIN command"));

    settings.setValue("OMIT", QT_TR_NOOP("<b>OMIT</b> - coordinate calculation function, omit"));
    settings.setValue("RSTRT", QT_TR_NOOP("<b>RSTRT</b> - coordinate calculation function, restart"));

    settings.setValue("LAA", QT_TR_NOOP("<b>LAA</b> - line at angle"));
    settings.setValue("ARC", QT_TR_NOOP("<b>ARC</b> - arc"));
    settings.setValue("GRDX", QT_TR_NOOP("<b>GRDX</b> - grid X"));
    settings.setValue("GRDY", QT_TR_NOOP("<b>GRDY</b> - grid Y"));
    settings.setValue("DGRDX", QT_TR_NOOP("<b>DGRDX</b> - double grid X"));
    settings.setValue("DGRDY", QT_TR_NOOP("<b>DGRDY</b> - double grid Y"));
    settings.setValue("SQRX", QT_TR_NOOP("<b>SQRX</b> - square X"));
    settings.setValue("SQRY", QT_TR_NOOP("<b>SQRY</b> - square Y"));
    settings.setValue("BHC", QT_TR_NOOP("<b>BHC X Y I J K </b> - bolt hole circle, X Y - circle center, I - radius, J - angle of first hole, K - no. of holes"));

    settings.setValue("EQ", QT_TR_NOOP("<b>EQ</b> - equal to"));
    settings.setValue("NE", QT_TR_NOOP("<b>NE</b> - not equal to"));
    settings.setValue("GT", QT_TR_NOOP("<b>GT</b> - greather than"));
    settings.setValue("LE", QT_TR_NOOP("<b>LE</b> - less than or equal to"));
    settings.setValue("LT", QT_TR_NOOP("<b>LT</b> - less than"));
    settings.setValue("GE", QT_TR_NOOP("<b>GE</b> - greather than or equal to"));
    settings.setValue("IF", QT_TR_NOOP("<b>IF[</b>condition<b>] N</b>xxxx - if condition is true goto block xxxx"));

    settings.setValue("TLFON", QT_TR_NOOP("<b>TLFON</b> - tool life on"));
    settings.setValue("EMPTY", QT_TR_NOOP("<b>EMPTY</b> - 'empty' value "));

    settings.setValue("SB", QT_TR_NOOP("<b>SB=</b>xxxx - M-tool spindle speed xxxx"));
    settings.setValue("TG", QT_TR_NOOP("<b>TG=</b>xx - tool group no. xx"));
    settings.setValue("OG", QT_TR_NOOP("<b>OG=</b>xx - offset group no. xx"));
    settings.setValue("GOTO", QT_TR_NOOP("<b>GOTO N</b>nnnn - jump to nnnn"));
    settings.setValue("CALL", QT_TR_NOOP("<b>CALL O</b>nnnn [<b>Q</b>xx] - call subprogram nnnn, repeat it xx times"));
    settings.setValue("RTS", QT_TR_NOOP("<b>RTS</b> - subprogram end"));
    settings.setValue("VLMON", QT_TR_NOOP("<b>VLMON[</b>xx<b>]=</b>yy - load monitor"));


    settings.setValue("VATOL", QT_TR_NOOP("<b>VATOL</b> - active tool number, tool kind + tool number"));
    settings.setValue("VNTOL", QT_TR_NOOP("<b>VNTOL</b> - next tool number, tool kind + tool number"));
    settings.setValue("VMLOK", QT_TR_NOOP("<b>VMLOK</b> - equal zero if not in machine lock status"));
    settings.setValue("VPPCP", QT_TR_NOOP("<b>VPPCP</b> - PPC parameter"));
    settings.setValue("VPLNO", QT_TR_NOOP("<b>VPLNO</b> - PPC pallet number"));
    settings.setValue("VPLDT", QT_TR_NOOP("<b>VPLDT</b> - PPC parameter bit data"));
    settings.setValue("VTLCN", QT_TR_NOOP("<b>VTLCN</b> - active tool number"));
    settings.setValue("VTLNN", QT_TR_NOOP("<b>VTLNN</b> - next tool number"));


    settings.setValue("VMCOD", QT_TR_NOOP("<b>VMCOD[</b>xx<b>]</b> - present M code of group no. xx"));
    settings.setValue("VSCOD", QT_TR_NOOP("<b>VSCOD</b> - command value of present spindle speed S"));
    settings.setValue("VDCOD", QT_TR_NOOP("<b>VDCOD</b> - present cutter radius compensation offset number"));
    settings.setValue("VFCOD", QT_TR_NOOP("<b>VFCOD</b> - command value of present feedrate F"));
    settings.setValue("VGCOD", QT_TR_NOOP("<b>VGCOD[</b>xx<b>]</b> - present G code of group no. xx"));
    settings.setValue("VHCOD", QT_TR_NOOP("<b>VHCOD</b> - present tool length offset number"));
    settings.setValue("VACOD", QT_TR_NOOP("<b>VACOD</b> - coordinate system number"));

    settings.setValue("VZOFX", QT_TR_NOOP("<i>v.M</i> <b>VZOFX[</b>xx<b>]</b> - zero offset no. xx of X-axis" \
                                          "<br /><i>v.L</i> <b>VZOFX</b> - zero offset of X-axis"));
    settings.setValue("VZOFZ", QT_TR_NOOP("<i>v.M</i> <b>VZOFZ[</b>xx<b>]</b> - zero offset no. xx of Z-axis" \
                                          "<br /><i>v.L</i> <b>VZOFZ</b> - zero offset of Z-axis"));
    settings.setValue("VZOFY", QT_TR_NOOP("<i>v.M</i> <b>VZOFY[</b>xx<b>]</b> - zero offset no. xx of Y-axis"));
    settings.setValue("VZOFW", QT_TR_NOOP("<i>v.L</i> <b>VZOFW</b> - zero offset of W-axis"));
    settings.setValue("VZOFC", QT_TR_NOOP("<i>v.L</i> <b>VZOFC</b> - zero offset of C-axis"));
    settings.setValue("VZSHZ", QT_TR_NOOP("<i>v.L</i> <b>VZSHZ</b> - zero shift of Z-axis"));
    settings.setValue("VZSHX", QT_TR_NOOP("<i>v.L</i> <b>VZSHX</b> - zero shift of X-axis"));
    settings.setValue("VZSHC", QT_TR_NOOP("<i>v.L</i> <b>VZSHC</b> - zero shift of C-axis"));
    settings.setValue("VTOFZ", QT_TR_NOOP("<i>v.L</i> <b>VTOFZ[</b>xx<b>]</b> - tool offset no. xx of Z-axis"));
    settings.setValue("VTOFX", QT_TR_NOOP("<i>v.L</i> <b>VTOFX[</b>xx<b>]</b> - tool offset no. xx of X-axis"));
    settings.setValue("VRSTT", QT_TR_NOOP("<b>VRSTT</b> - equal zero if not in restart state"));
    settings.setValue("VTOFH", QT_TR_NOOP("<i>v.M</i> <b>VTOFH[</b>xx<b>]</b> - tool length, offset no. xx"));
    settings.setValue("VTOFD", QT_TR_NOOP("<i>v.M</i> <b>VTOFD[</b>xx<b>]</b> - cutter radius, offset no. xx"));
    settings.setValue("VNCOM", QT_TR_NOOP("<b>VNCOM[</b>xx<b>]</b> - communication for automation, no. xx"));



    //*******************************************************************************************************************//
    settings.endGroup(); //  OKUMA
    //*******************************************************************************************************************//



    //*******************************************************************************************************************//
    settings.beginGroup("FANUC");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));
    settings.setValue("M98", QT_TR_NOOP("<b>M98 P</b>xxxx - macro xxxx call"));
    settings.setValue("M99", QT_TR_NOOP("<b>M99</b> - subprogram end"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02 {X Y Z</b> | <b>U V W} {I J K</b> | <b>R} F</b> - circular interpolation CW; XYZ - end point (absolute); UW - end point (incremental); IJK - distance from start point to center; R - radius of arc"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03 {X Y Z</b> | <b>U V W} {I J K</b> | <b>R} F</b> - circular interpolation CCW; XYZ - end point (absolute); UW - end point (incremental); IJK - distance from start point to center; R - radius of arc"));
    settings.setValue("G04", QT_TR_NOOP("<b>G04 X U P</b> - dwell XU - in seconds; P - in microseconds"));

    settings.setValue("G10", QT_TR_NOOP("<b>G10 P X Z Y R Q</b> - change of offset value by program"));

    settings.setValue("G17", QT_TR_NOOP("<b>G17</b> - XY plane"));
    settings.setValue("G18", QT_TR_NOOP("<b>G18</b> - ZX plane"));
    settings.setValue("G19", QT_TR_NOOP("<b>G19</b> - YZ plane"));

    settings.setValue("G20", QT_TR_NOOP("<b>G20</b> - inch input"));
    settings.setValue("G21", QT_TR_NOOP("<b>G21</b> - metric input"));
    settings.setValue("G27", QT_TR_NOOP("<b>G27</b> - reference point return check"));
    settings.setValue("G28", QT_TR_NOOP("<b>G28</b> - reference point return"));
    settings.setValue("G30", QT_TR_NOOP("<b>G30 P</b>x - x = 2nd, 3rd, 4th reference point return"));
    settings.setValue("G31", QT_TR_NOOP("<b>G31</b> - skip function"));

    settings.setValue("G40", QT_TR_NOOP("<b>G40</b> - tool nose/cutter radius compensation cancel"));
    settings.setValue("G41", QT_TR_NOOP("<b>G41</b> - tool nose/cutter radius compensation left"));
    settings.setValue("G42", QT_TR_NOOP("<b>G42</b> - tool nose/cutter radius compensation right"));

    settings.setValue("G50", QT_TR_NOOP("<i>v.T</i> <b>G50 S</b>xxxx - maximum spindle speed" \
                                        "<br /><b>G50 X Z</b> - zero point shift"));
    settings.setValue("G53", QT_TR_NOOP("<b>G53</b> - machine coordinate system"));
    settings.setValue("G54", QT_TR_NOOP("<b>G54</b> - settable zero offset 1"));
    settings.setValue("G55", QT_TR_NOOP("<b>G55</b> - settable zero offset 2"));
    settings.setValue("G56", QT_TR_NOOP("<b>G56</b> - settable zero offset 3"));
    settings.setValue("G57", QT_TR_NOOP("<b>G57</b> - settable zero offset 4"));
    settings.setValue("G58", QT_TR_NOOP("<b>G58</b> - settable zero offset 5"));
    settings.setValue("G59", QT_TR_NOOP("<b>G59</b> - settable zero offset 6"));

    settings.setValue("G65", QT_TR_NOOP("<b>G65 P</b>xxxx - macro xxxx call"));
    settings.setValue("G66", QT_TR_NOOP("<b>G66 P</b>xxxx - macro xxxx modal call"));
    settings.setValue("G67", QT_TR_NOOP("<b>G67</b> - macro modal call cancel"));

    settings.setValue("G70", QT_TR_NOOP("<i>v.T</i> <b>G70 P Q</b> - finishing cycle; P - sequence number of the first block of finishing shape, Q - sequence number of last block"));
    settings.setValue("G71", QT_TR_NOOP("<i>v.T</i> <b>G71 U R</b> - U - depth of cut, Q - escaping amount" \
                                        "<br /><i>v.T</i> <b>G71 P Q U W</b> - stock removal in turning; P - first sequence number of finishing shape, Q - last sequence number of finishing shape, U - X finishing allowance, W - Z finishing allowance"));
    settings.setValue("G72", QT_TR_NOOP("<i>v.T</i> <b>G72 W R</b> - U - depth of cut, Q - escaping amount" \
                                        "<br /><i>v.T</i> <b>G72 P Q U W</b> - stock removal in facing; P - first sequence number of finishing shape, Q - last sequence number of finishing shape, U - X finishing allowance, W - Z finishing allowance"));
    settings.setValue("G73", QT_TR_NOOP("<i>v.T</i> <b>G73 W R</b>" \
                                        "<br /><i>v.T</i> <b>G73 P Q U W</b> - pattern repeating"));
    settings.setValue("G74", QT_TR_NOOP("<i>v.T</i> <b>G74 R</b>" \
                                        "<br /><i>v.T</i> <b>G74 {X Z U W} P Q R</b> - end face peck drilling cycle"));
    settings.setValue("G75", QT_TR_NOOP("<i>v.T</i> <b>G75 R</b>" \
                                        "<br /><i>v.T</i> <b>G75 {X Z U W} P Q U W</b> - outer/internal diameter drilling cycle"));
    settings.setValue("G76", QT_TR_NOOP("<i>v.T</i> <b>G76 P Q R</b>" \
                                        "<br /><i>v.T</i> <b>G76 {X Z U W} P Q U W</b> - multiple thread cutting cycle"));

    settings.setValue("G90", QT_TR_NOOP("<b>G90</b> - absolute programming"));
    settings.setValue("G91", QT_TR_NOOP("<b>G91</b> - incremental programming"));
    settings.setValue("G92", QT_TR_NOOP("<b>G92 X Z</b> - coordinate system setting"));
    settings.setValue("G94", QT_TR_NOOP("<b>G94</b> - feed per minute"));
    settings.setValue("G95", QT_TR_NOOP("<b>G95</b> - feed per revolution"));
    settings.setValue("G96", QT_TR_NOOP("<b>G96 S</b>xx - constant cutting speed xx"));
    settings.setValue("G97", QT_TR_NOOP("<b>G97 S</b>xx - constant spindle speed xx"));
    settings.setValue("G98", QT_TR_NOOP("<b>G98</b> - return to initial level"));
    settings.setValue("G99", QT_TR_NOOP("<b>G99</b> - return to R level"));

    settings.setValue("EQ", QT_TR_NOOP("<b>EQ</b> - equal to"));
    settings.setValue("NE", QT_TR_NOOP("<b>NE</b> - not equal to"));
    settings.setValue("GT", QT_TR_NOOP("<b>GT</b> - greather than"));
    settings.setValue("LE", QT_TR_NOOP("<b>LE</b> - less than or equal to"));
    settings.setValue("LT", QT_TR_NOOP("<b>LT</b> - less than"));
    settings.setValue("GE", QT_TR_NOOP("<b>GE</b> - greather than or equal to"));
    settings.setValue("IF", QT_TR_NOOP("<b>IF[</b>condition<b>]</b>do something - if condition is true do something"));
    settings.setValue("GOTO", QT_TR_NOOP("<b>GOTO</b>nnnn - jump to block nnnn"));

    settings.setValue("WHILE", QT_TR_NOOP("<b>WHILE[</b>condition<b>] DO</b>n <br />...<br />commands<br />... <br /><b>END</b>n  - loop - while condition true do commands beetwen DOn and ENDn"));
    settings.setValue("END", QT_TR_NOOP("<b>END</b>n - end of WHILE DOn loop"));

    settings.setValue("EOR", QT_TR_NOOP("<b>EOR</b> - exclusive OR"));
    settings.setValue("OR", QT_TR_NOOP("<b>OR</b> - logical OR"));
    settings.setValue("AND", QT_TR_NOOP("<b>AND</b> - logical AND"));
    settings.setValue("NOT", QT_TR_NOOP("<b>NOT</b> - negation"));

    settings.setValue("SIN", QT_TR_NOOP("<b>SIN[</b>angle<b>]</b> - sine"));
    settings.setValue("COS", QT_TR_NOOP("<b>COS[</b>angle<b>]</b> - cosine"));
    settings.setValue("TAN", QT_TR_NOOP("<b>TAN[</b>angle<b>]</b> - tangent"));
    settings.setValue("ATAN", QT_TR_NOOP("<b>ATAN[</b>angle<b>]</b> - arctangent 1 or 2"));
    settings.setValue("SQRT", QT_TR_NOOP("<b>SQRT[</b>val<b>]</b> - square root"));
    settings.setValue("ABS", QT_TR_NOOP("<b>ABS[</b>val<b>]</b> - absolute value"));
    settings.setValue("BIN", QT_TR_NOOP("<b>BIN[</b>val<b>]</b> - decimal to binary conversion"));
    settings.setValue("BCD", QT_TR_NOOP("<b>BCD[</b>val<b>]</b> - binary to decimal conversion"));
    settings.setValue("ROUND", QT_TR_NOOP("<b>ROUND[</b>val<b>]</b> - integer implementation (rounding)"));
    settings.setValue("FIX", QT_TR_NOOP("<b>FIX[</b>val<b>]</b> - integer implementation (truncation)"));
    settings.setValue("FUP", QT_TR_NOOP("<b>FUP[</b>val<b>]</b> - integer implementation (raising)"));



    //*******************************************************************************************************************//
    settings.endGroup();  //  FANUC
    //*******************************************************************************************************************//



    //*******************************************************************************************************************//
    settings.beginGroup("SINUMERIK");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));
    settings.setValue("M51", QT_TR_NOOP("<b>M51</b> - thro spindle coolant on"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02</b> - circular interpolation CW"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03</b> - circular interpolation CCW"));
    settings.setValue("G04", QT_TR_NOOP("<b>G04 F</b>xxxx - dwell xxxx seconds"));

    settings.setValue("G17", QT_TR_NOOP("<b>G17</b> - XY plane"));
    settings.setValue("G18", QT_TR_NOOP("<b>G18</b> - ZX plane"));
    settings.setValue("G19", QT_TR_NOOP("<b>G19</b> - YZ plane"));

    settings.setValue("G40", QT_TR_NOOP("<b>G40</b> - tool nose/cutter radius compensation cancel"));
    settings.setValue("G41", QT_TR_NOOP("<b>G41</b> - tool nose/cutter radius compensation left"));
    settings.setValue("G42", QT_TR_NOOP("<b>G42</b> - tool nose/cutter radius compensation right"));

    settings.setValue("G53", QT_TR_NOOP("<b>G53</b> - machine coordinate system"));
    settings.setValue("G54", QT_TR_NOOP("<b>G54</b> - settable zero offset 1"));
    settings.setValue("G55", QT_TR_NOOP("<b>G55</b> - settable zero offset 2"));
    settings.setValue("G56", QT_TR_NOOP("<b>G56</b> - settable zero offset 3"));
    settings.setValue("G57", QT_TR_NOOP("<b>G57</b> - settable zero offset 4"));
    settings.setValue("G58", QT_TR_NOOP("<b>G58</b> - programmable offset, absolute axial substitution"));
    settings.setValue("G59", QT_TR_NOOP("<b>G59</b> - programmable offset, additive axial substitution"));

    settings.setValue("G90", QT_TR_NOOP("<b>G90</b> - absolute programming"));
    settings.setValue("G91", QT_TR_NOOP("<b>G91</b> - incremental programming"));
    settings.setValue("G94", QT_TR_NOOP("<b>G94</b> - feed per minute"));
    settings.setValue("G95", QT_TR_NOOP("<b>G95</b> - feed per revolution"));
    settings.setValue("G96", QT_TR_NOOP("<b>G96 S</b>xx - constant cutting speed xx"));
    settings.setValue("G97", QT_TR_NOOP("<b>G97 S</b>xx - constant spindle speed xx"));


    settings.setValue("@100", QT_TR_NOOP("<b>@100 K</b>nnnn - jump to block number nnnn"));
    settings.setValue("@121", QT_TR_NOOP("<b>@121 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy not equal to xx jump to nnnn"));
    settings.setValue("@122", QT_TR_NOOP("<b>@122 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy equal to xx jump to nnnn"));
    settings.setValue("@123", QT_TR_NOOP("<b>@123 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is less than or equal to xx jump to nnnn"));
    settings.setValue("@124", QT_TR_NOOP("<b>@124 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is less than xx jump to nnnn"));
    settings.setValue("@125", QT_TR_NOOP("<b>@125 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is greather than or equal to xx jump to nnnn"));
    settings.setValue("@126", QT_TR_NOOP("<b>@126 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is greather than xx jump to nnnn"));

    settings.setValue("@620", QT_TR_NOOP("<b>@620 R</b>xx - increment value in param. xx"));
    settings.setValue("@621", QT_TR_NOOP("<b>@621 R</b>xx - decrement value in param. xx"));
    settings.setValue("@622", QT_TR_NOOP("<b>@622 R</b>xx - 'trunc' value in param. xx"));
    settings.setValue("@630", QT_TR_NOOP("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - sine of angle yy is stored in param. xx"));
    settings.setValue("@631", QT_TR_NOOP("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - cosine of angle yy is stored in param. xx"));
    settings.setValue("@632", QT_TR_NOOP("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - tangent of angle yy is stored in param. xx"));
    settings.setValue("@634", QT_TR_NOOP("<b>@634 R</b>xx <b>R</b>|<b>K</b>yy - arc sine of angle yy is stored in param. xx"));

    settings.setValue("@714", QT_TR_NOOP("<b>@714</b> - stop decoding, until buffer is empty"));


    //*******************************************************************************************************************//
    settings.endGroup();  //  SINUMERIK
    //*******************************************************************************************************************//


    //*******************************************************************************************************************//
    settings.beginGroup("SINUMERIK_840");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M07", QT_TR_NOOP("<b>M07</b> - thro spindle coolant on"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));
    settings.setValue("M50", QT_TR_NOOP("<b>M50</b> - shower coolant on"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02</b> - circular interpolation CW"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03</b> - circular interpolation CCW"));
    settings.setValue("G04", QT_TR_NOOP("<b>G04 F</b>xxxx - dwell xxxx seconds"));

    settings.setValue("G17", QT_TR_NOOP("<b>G17</b> - XY plane"));
    settings.setValue("G18", QT_TR_NOOP("<b>G18</b> - ZX plane"));
    settings.setValue("G19", QT_TR_NOOP("<b>G19</b> - YZ plane"));

    settings.setValue("G53", QT_TR_NOOP("<b>G53</b> - Suppression of current frames: Programmable frame including system frame for TOROT and TOFRAME and active settable frame G54 ... G599."));
    settings.setValue("G54", QT_TR_NOOP("<b>G54</b> - settable zero offset 1"));
    settings.setValue("G55", QT_TR_NOOP("<b>G55</b> - settable zero offset 2"));
    settings.setValue("G56", QT_TR_NOOP("<b>G56</b> - settable zero offset 3"));
    settings.setValue("G57", QT_TR_NOOP("<b>G57</b> - settable zero offset 4"));
    settings.setValue("G58", QT_TR_NOOP("<b>G58</b> - programmable offset, absolute axial substitution"));
    settings.setValue("G59", QT_TR_NOOP("<b>G59</b> - programmable offset, additive axial substitution"));


    settings.setValue("G40", QT_TR_NOOP("<b>G40</b> - tool nose/cutter radius compensation cancel"));
    settings.setValue("G41", QT_TR_NOOP("<b>G41</b> - tool nose/cutter radius compensation left"));
    settings.setValue("G42", QT_TR_NOOP("<b>G42</b> - tool nose/cutter radius compensation right"));

    settings.setValue("G60", QT_TR_NOOP("<b>G60</b> - Velocity reduction, exact positioning"));
    settings.setValue("G64", QT_TR_NOOP("<b>G64</b> - Continuous-path mode"));

    settings.setValue("G90", QT_TR_NOOP("<b>G90</b> - absolute programming"));
    settings.setValue("G91", QT_TR_NOOP("<b>G91</b> - incremental programming"));
    settings.setValue("G94", QT_TR_NOOP("<b>G94</b> - feed per minute"));
    settings.setValue("G95", QT_TR_NOOP("<b>G95</b> - feed per revolution"));
    settings.setValue("G96", QT_TR_NOOP("<b>G96 S</b>xx - constant cutting speed xx"));
    settings.setValue("G97", QT_TR_NOOP("<b>G97 S</b>xx - constant spindle speed xx"));

    settings.setValue("G153", QT_TR_NOOP("<b>G153</b> - suppression as for G53 and including all channel-specific and/or NCU-global basic frame"));
    settings.setValue("G500", QT_TR_NOOP("<b>G500</b> - deactivate all settable G54-G57 frames if G500 does not contain a value"));

    settings.setValue("SUPA", QT_TR_NOOP("<b>SUPA</b> - suppression as for G153 and including system frames for actual-value setting, scratching, zero offset external, PAROT including handwheel offsets (DRF), [zero offset external], overlaid motion"));

    settings.setValue("IF", QT_TR_NOOP("<b>IF(</b>condition<b>)</b> nnnn - if condition is true goto block nnnn or label nnnn:"));
    settings.setValue("GOTO", QT_TR_NOOP("<b>GOTO</b> nnnn - jump forward and if block not found jump backward to block nnnn or label nnnn:"));
    settings.setValue("GOTOF", QT_TR_NOOP("<b>GOTOF</b> nnnn - jump forward to block nnnn or label nnnn:"));
    settings.setValue("GOTOB", QT_TR_NOOP("<b>GOTOB</b> nnnn - jump backward to block nnnn or label nnnn:"));

    settings.setValue("STOPRE", QT_TR_NOOP("<b>STOPRE</b> - stop decoding, until buffer is empty"));

    settings.setValue("DIAMOF", QT_TR_NOOP("<b>DIAMOF</b> - diameter programming OFF; radius programming for G90/G91"));
    settings.setValue("DIAMON", QT_TR_NOOP("<b>DIAMON</b> - diameter programming ON; diameter programming for G90/G91"));
    settings.setValue("DIAM", QT_TR_NOOP("<b>DIAM90</b> - diameter programming for G90; radius programming for G91"));


    settings.setValue("RET", QT_TR_NOOP("<b>RET</b> - subprogram end"));



    //*******************************************************************************************************************//
    settings.endGroup(); //  SINUMERIK_840
    //*******************************************************************************************************************//




    //*******************************************************************************************************************//
    settings.beginGroup("HEIDENHAIN");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M07", QT_TR_NOOP("<b>M07</b> - thro spindle coolant on"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));
    settings.setValue("M50", QT_TR_NOOP("<b>M50</b> - shower coolant on"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02</b> - circular interpolation CW"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03</b> - circular interpolation CCW"));
    settings.setValue("G04", QT_TR_NOOP("<b>G04 F</b>xxxx - dwell xxxx seconds"));







    //*******************************************************************************************************************//
    settings.endGroup();  //  HEIDENHAIN
    //*******************************************************************************************************************//


    //*******************************************************************************************************************//
    settings.beginGroup("HEIDENHAIN_ISO");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M07", QT_TR_NOOP("<b>M07</b> - thro spindle coolant on"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));
    settings.setValue("M50", QT_TR_NOOP("<b>M50</b> - shower coolant on"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02</b> - circular interpolation CW"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03</b> - circular interpolation CCW"));
    settings.setValue("G04", QT_TR_NOOP("<b>G04 F</b>xxxx - dwell xxxx seconds"));

    settings.setValue("G17", QT_TR_NOOP("<b>G17</b> - XY plane"));
    settings.setValue("G18", QT_TR_NOOP("<b>G18</b> - ZX plane"));
    settings.setValue("G19", QT_TR_NOOP("<b>G19</b> - YZ plane"));




    //*******************************************************************************************************************//
    settings.endGroup();  //  HEIDENHAIN_ISO
    //*******************************************************************************************************************//


    //*******************************************************************************************************************//
    settings.beginGroup("LinuxCNC");
    //*******************************************************************************************************************//
    settings.setValue("M00", QT_TR_NOOP("<b>M00</b> - program stop, unconditional"));
    settings.setValue("M01", QT_TR_NOOP("<b>M01</b> - optional program stop"));
    settings.setValue("M03", QT_TR_NOOP("<b>M03</b> - start spindle CW"));
    settings.setValue("M04", QT_TR_NOOP("<b>M04</b> - start spindle CCW"));
    settings.setValue("M05", QT_TR_NOOP("<b>M05</b> - spindle stop"));
    settings.setValue("M06", QT_TR_NOOP("<b>M06</b> - tool change"));
    settings.setValue("M07", QT_TR_NOOP("<b>M07</b> - thro spindle coolant on"));
    settings.setValue("M08", QT_TR_NOOP("<b>M08</b> - coolant on"));
    settings.setValue("M09", QT_TR_NOOP("<b>M09</b> - coolant off"));
    settings.setValue("M30", QT_TR_NOOP("<b>M30</b> - end of program"));
    settings.setValue("M50", QT_TR_NOOP("<b>M50</b> - shower coolant on"));

    settings.setValue("G00", QT_TR_NOOP("<b>G00</b> - rapid move - positioning"));
    settings.setValue("G01", QT_TR_NOOP("<b>G01</b> - linear interpolation"));
    settings.setValue("G02", QT_TR_NOOP("<b>G02</b> - circular interpolation CW"));
    settings.setValue("G03", QT_TR_NOOP("<b>G03</b> - circular interpolation CCW"));
    settings.setValue("G64", QT_TR_NOOP("<b>G64 P {Q}</b> - Path Blending"));







    //*******************************************************************************************************************//
    settings.endGroup();  //  LinuxCNC
    //*******************************************************************************************************************//


    return fileName;

}



#endif // TOOLTIPS_H
