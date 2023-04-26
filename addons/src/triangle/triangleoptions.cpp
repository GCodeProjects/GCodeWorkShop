/*
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of EdytorNC.
 *
 *  EdytorNC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QSettings>    // for QSettings
#include <QVariant>     // for QVariant

#include "triangleoptions.h"


#define CFG_KEY_ANGLE_A_IN      "AngleAIn"
#define CFG_KEY_ANGLE_A_VALUE   "AngleAValue"
#define CFG_KEY_ANGLE_B_IN      "AngleBIn"
#define CFG_KEY_ANGLE_B_VALUE   "AngleBValue"
#define CFG_KEY_ANGLE_C_IN      "AngleCIn"
#define CFG_KEY_ANGLE_C_VALUE   "AngleCValue"
#define CFG_KEY_SIDE_A_IN       "SideAIn"
#define CFG_KEY_SIDE_A_VALUE    "SideAValue"
#define CFG_KEY_SIDE_B_IN       "SideBIn"
#define CFG_KEY_SIDE_B_VALUE    "SideBValue"
#define CFG_KEY_SIDE_C_IN       "SideCIn"
#define CFG_KEY_SIDE_C_VALUE    "SideCValue"
#define CFG_KEY_RIGHT_TRIANGLE  "RightTriangle"


void TriangleOptions::load(QSettings *settings, const TriangleOptions &defaultOptions)
{
    angleA.in = settings->value(CFG_KEY_ANGLE_A_IN, defaultOptions.angleA.in).toBool();
    angleA.value = settings->value(CFG_KEY_ANGLE_A_VALUE, defaultOptions.angleA.value).toDouble();
    angleB.in = settings->value(CFG_KEY_ANGLE_B_IN, defaultOptions.angleB.in).toBool();
    angleB.value = settings->value(CFG_KEY_ANGLE_B_VALUE, defaultOptions.angleB.value).toDouble();
    angleC.in = settings->value(CFG_KEY_ANGLE_C_IN, defaultOptions.angleC.in).toBool();
    angleC.value = settings->value(CFG_KEY_ANGLE_C_VALUE, defaultOptions.angleC.value).toDouble();
    sideA.in = settings->value(CFG_KEY_SIDE_A_IN, defaultOptions.sideA.in).toBool();
    sideA.value = settings->value(CFG_KEY_SIDE_A_VALUE, defaultOptions.sideA.value).toDouble();
    sideB.in = settings->value(CFG_KEY_SIDE_B_IN, defaultOptions.sideB.in).toBool();
    sideB.value = settings->value(CFG_KEY_SIDE_B_VALUE, defaultOptions.sideB.value).toDouble();
    sideC.in = settings->value(CFG_KEY_SIDE_B_IN, defaultOptions.sideC.in).toBool();
    sideC.value = settings->value(CFG_KEY_SIDE_B_VALUE, defaultOptions.sideC.value).toDouble();
    rightTriangle = settings->value(CFG_KEY_RIGHT_TRIANGLE, defaultOptions.rightTriangle).toBool();
}

void TriangleOptions::save(QSettings *settings)
{
    settings->setValue(CFG_KEY_ANGLE_A_IN, angleA.in);
    settings->setValue(CFG_KEY_ANGLE_A_VALUE, angleA.value);
    settings->setValue(CFG_KEY_ANGLE_B_IN, angleB.in);
    settings->setValue(CFG_KEY_ANGLE_B_VALUE, angleB.value);
    settings->setValue(CFG_KEY_ANGLE_C_IN, angleC.in);
    settings->setValue(CFG_KEY_ANGLE_C_VALUE, angleC.value);
    settings->setValue(CFG_KEY_SIDE_A_IN, sideA.in);
    settings->setValue(CFG_KEY_SIDE_A_VALUE, sideA.value);
    settings->setValue(CFG_KEY_SIDE_B_IN, sideB.in);
    settings->setValue(CFG_KEY_SIDE_B_VALUE, sideB.value);
    settings->setValue(CFG_KEY_SIDE_B_IN, sideC.in);
    settings->setValue(CFG_KEY_SIDE_B_VALUE, sideC.value);
    settings->setValue(CFG_KEY_RIGHT_TRIANGLE, rightTriangle);
}
