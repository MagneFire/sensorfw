/**
   @file heartrate.h
   @brief QObject based datatype for HeartRateData

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#ifndef HEARTRATE_H
#define HEARTRATE_H

#include <QDBusArgument>
#include <datatypes/heartratedata.h>

/**
 * QObject facade for #HeartRateData.
 */
class HeartRate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(HrmStatus hrmStatus READ status)
    Q_PROPERTY(int bpm READ bpm)

public:
    /**
     * Default constructor.
     */
    HeartRate() {}

    /**
     * Constructor.
     *
     * @param HeartRateData Source object.
     */
    HeartRate(const HeartRateData& heartRateData);

    /**
     * Copy constructor.
     *
     * @param HeartRate Source object.
     */
    HeartRate(const HeartRate& heartRate);

    /**
     * Accessor for contained #HeartRateData.
     *
     * @return contained #HeartRateData.
     */
    const HeartRateData& heartRateData() const { return data_; }

    /**
     * Accessor for hrm status.
     *
     * @return heart rate monitor status.
     */
    HrmStatus status() const { return data_.status_; }

    /**
     * Accessor for bpm.
     *
     * @return beat per minutes.
     */
    int bpm() const { return data_.bpm_; }

    /**
     * Assignment operator.
     *
     * @param origin Source object for assigment.
     */
    HeartRate& operator=(const HeartRate& origin)
    {
        data_ = origin.heartRateData();
        return *this;
    }

private:
    HeartRateData data_; /**< Contained data */

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, HeartRate& heartRate);
};

Q_DECLARE_METATYPE( HeartRate )

/**
 * Marshall the HeartRate data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param heartRate data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const HeartRate &heartRate)
{
    argument.beginStructure();
    argument << heartRate.heartRateData().status_ << heartRate.heartRateData().bpm_;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall HeartRate data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param heartRate unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, HeartRate &heartRate)
{
    argument.beginStructure();
    argument >> heartRate.data_.status_ >> heartRate.data_.bpm_;
    argument.endStructure();
    return argument;
}

#endif // HEARTRATE_H
