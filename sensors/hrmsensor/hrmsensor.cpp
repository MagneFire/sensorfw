/**
   @file hrmsensor.cpp
   @brief heart rate monitor sensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2019 Florent Revest

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

#include "hrmsensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "datatypes/orientation.h"

HrmSensorChannel::HrmSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<HeartRateData>(1)
{
    SensorManager& sm = SensorManager::instance();

    hrmAdaptor_ = sm.requestDeviceAdaptor("hrmadaptor");
    if (!hrmAdaptor_) {
        setValid(false);
        return;
    }

    hrmReader_ = new BufferReader<HeartRateData>(1);

    outputBuffer_ = new RingBuffer<HeartRateData>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(hrmReader_, "hrm");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("hrm", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(hrmAdaptor_, "hrm", hrmReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setDescription("beats per minute");
    setRangeSource(hrmAdaptor_);
    addStandbyOverrideSource(hrmAdaptor_);
    setIntervalSource(hrmAdaptor_);

    setValid(true);
}

HrmSensorChannel::~HrmSensorChannel()
{
    if (isValid()) {
        SensorManager& sm = SensorManager::instance();

        disconnectFromSource(hrmAdaptor_, "hrm", hrmReader_);

        sm.releaseDeviceAdaptor("hrmadaptor");

        delete hrmReader_;
        delete outputBuffer_;
        delete marshallingBin_;
        delete filterBin_;
    }
}

bool HrmSensorChannel::start()
{
    sensordLogD() << "Starting HrmSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        hrmAdaptor_->startSensor();
    }
    return true;
}

bool HrmSensorChannel::stop()
{
    sensordLogD() << "Stopping HrmSensorChannel";

    if (AbstractSensorChannel::stop()) {
        hrmAdaptor_->stopSensor();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void HrmSensorChannel::emitData(const HeartRateData& value)
{
    if (value.bpm_ != previousValue_.bpm_ || value.status_ != previousValue_.status_) {
        previousValue_.bpm_ = value.bpm_;
        previousValue_.status_ = value.status_;

        writeToClients((const void*)(&value), sizeof(value));
    }
}
