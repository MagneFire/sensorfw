/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
** Copyright (C) 2019 Florent Revest
** Contact: revestflo@gmail.com
**
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef HYBRISHRMADAPTOR_H
#define HYBRISHRMADAPTOR_H
#include "hybrisadaptor.h"

#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/heartratedata.h"
#include <QTime>

/**
 * @brief Adaptor for hybris hrm sensor.
 *
 * Adaptor for heart rate monitor sensor. 
 *
 * Returns the beat per minute of the user's heart.
 *
 */
class HybrisHrmAdaptor : public HybrisAdaptor
{
    Q_OBJECT

public:
    static DeviceAdaptor* factoryMethod(const QString& id) {
        return new HybrisHrmAdaptor(id);
    }
    HybrisHrmAdaptor(const QString& id);
    ~HybrisHrmAdaptor();

    bool startSensor();
    void stopSensor();

    void sendInitialData();

protected:
    void processSample(const sensors_event_t& data);
    void init();

private:
    DeviceAdaptorRingBuffer<HeartRateData>* buffer;
    QByteArray powerStatePath;

};
#endif
