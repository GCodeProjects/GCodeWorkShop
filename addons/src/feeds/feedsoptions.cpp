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

#include "feedsoptions.h"


#define CFG_KEY_DIAM_VALUE  "Diam"
#define CFG_KEY_DIAM_IN     "DiamIn"
#define CFG_KEY_FEED_VALUE  "Feed"
#define CFG_KEY_FEED_IN     "FeedIn"
#define CFG_KEY_FZ_VALUE    "Fz"
#define CFG_KEY_FZ_IN       "FzIn"
#define CFG_KEY_SPEED_VALUE "Speed"
#define CFG_KEY_SPEED_IN    "SpeedIn"
#define CFG_KEY_USE_INCH    "UseInch"
#define CFG_KEY_VC_VALUE    "Vc"
#define CFG_KEY_VC_IN       "VcIn"
#define CFG_KEY_Z_VALUE     "z"
#define CFG_KEY_Z_IN        "zIn"


void FeedsOptions::load(QSettings *settings, const FeedsOptions &defaultOptions)
{
    diam.value = settings->value(CFG_KEY_DIAM_VALUE, defaultOptions.diam.value).toDouble();
    diam.in = settings->value(CFG_KEY_DIAM_IN, defaultOptions.diam.in).toBool();
    feed.value = settings->value(CFG_KEY_FEED_VALUE, defaultOptions.feed.value).toDouble();
    feed.in = settings->value(CFG_KEY_FEED_IN, defaultOptions.feed.in).toBool();
    Fz.value = settings->value(CFG_KEY_FZ_VALUE, defaultOptions.Fz.value).toDouble();
    Fz.in = settings->value(CFG_KEY_FZ_IN, defaultOptions.Fz.in).toBool();
    speed.value = settings->value(CFG_KEY_SPEED_VALUE, defaultOptions.speed.value).toDouble();
    speed.in = settings->value(CFG_KEY_SPEED_IN, defaultOptions.speed.in).toBool();
    Vc.value = settings->value(CFG_KEY_VC_VALUE, defaultOptions.Vc.value).toDouble();
    Vc.in = settings->value(CFG_KEY_VC_IN, defaultOptions.Vc.in).toBool();
    useInch = settings->value(CFG_KEY_USE_INCH, defaultOptions.useInch).toBool();
    z.value = settings->value(CFG_KEY_Z_VALUE, defaultOptions.z.value).toDouble();
    z.in = settings->value(CFG_KEY_Z_IN, defaultOptions.z.in).toBool();
}

void FeedsOptions::save(QSettings *settings)
{
    settings->setValue(CFG_KEY_DIAM_VALUE, diam.value);
    settings->setValue(CFG_KEY_DIAM_IN, diam.in);
    settings->setValue(CFG_KEY_FEED_VALUE, feed.value);
    settings->setValue(CFG_KEY_FEED_IN, feed.in);
    settings->setValue(CFG_KEY_FZ_VALUE, Fz.value);
    settings->setValue(CFG_KEY_FZ_IN, Fz.in);
    settings->setValue(CFG_KEY_SPEED_VALUE, speed.value);
    settings->setValue(CFG_KEY_SPEED_IN, speed.in);
    settings->setValue(CFG_KEY_VC_VALUE, Vc.value);
    settings->setValue(CFG_KEY_VC_IN, Vc.in);
    settings->setValue(CFG_KEY_USE_INCH, useInch);
    settings->setValue(CFG_KEY_Z_VALUE, z.value);
    settings->setValue(CFG_KEY_Z_IN, z.in);
}
