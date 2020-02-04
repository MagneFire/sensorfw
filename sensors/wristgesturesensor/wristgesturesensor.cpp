/**
   @file wristgesturesensor.cpp
   @brief WristGestureSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2020 Darrel Griët

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#include "wristgesturesensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"

WristGestureSensorChannel::WristGestureSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<TimedUnsigned>(1),
        prevWristGesture(0,0)
{
    SensorManager& sm = SensorManager::instance();

    wristGestureAdaptor_ = sm.requestDeviceAdaptor("wristgestureadaptor");
    if (!wristGestureAdaptor_) {
        setValid(false);
        return;
    }

    wristgestureReader_ = new BufferReader<TimedUnsigned>(1);

    outputBuffer_ = new RingBuffer<TimedUnsigned>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(wristgestureReader_, "wristgesture");
    filterBin_->add(outputBuffer_, "buffer");

    // Join filterchain buffers
    filterBin_->join("wristgesture", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(wristGestureAdaptor_, "wristgesture", wristgestureReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setDescription("wristgesture sensor for wake up detection");
    setRangeSource(wristGestureAdaptor_);
    addStandbyOverrideSource(wristGestureAdaptor_);
    setIntervalSource(wristGestureAdaptor_);

    setValid(true);
}

WristGestureSensorChannel::~WristGestureSensorChannel()
{
    if (isValid()) {
        SensorManager& sm = SensorManager::instance();

        disconnectFromSource(wristGestureAdaptor_, "wristgesture", wristgestureReader_);

        sm.releaseDeviceAdaptor("wristgestureadaptor");

        delete wristgestureReader_;
        delete outputBuffer_;
        delete marshallingBin_;
        delete filterBin_;
    }
}

bool WristGestureSensorChannel::start()
{
    sensordLogD() << "Starting WristGestureSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        wristGestureAdaptor_->startSensor();
    }
    return true;
}

bool WristGestureSensorChannel::stop()
{
    sensordLogD() << "Stopping WristGestureSensorChannel";

    if (AbstractSensorChannel::stop()) {
        wristGestureAdaptor_->stopSensor();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void WristGestureSensorChannel::emitData(const TimedUnsigned& value)
{
    prevWristGesture.value_ = value.value_;
    writeToClients((const void *)&value, sizeof(value));
}
