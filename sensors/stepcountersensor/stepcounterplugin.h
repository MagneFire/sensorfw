/**
   @file stepcounterplugin.h
   @brief Plugin for stepcounter sensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2016 kimmoli

   @author Kimmo Lindholm <kimmo.lindholm@eke.fi>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef STEPCOUNTERPLUGIN_H
#define STEPCOUNTERPLUGIN_H

#include "plugin.h"

class StepCounterPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
private:
    void Register(class Loader& l);
    void Init(class Loader& l);
    QStringList Dependencies();
};

#endif
