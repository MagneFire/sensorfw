/**
   @file orientationplugin.h
   @brief Plugin for WristGestureSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2020 Darrel Griët

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Darrel Griët <idanlcontact@gmail.com>

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

#ifndef WRISTGESTUREPLUGIN_H
#define WRISTGESTUREPLUGIN_H

#include "plugin.h"

class WristGesturePlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
private:
    void Register(class Loader& l);
    QStringList Dependencies();
};

#endif
