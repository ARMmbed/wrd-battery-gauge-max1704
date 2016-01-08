/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WRD_FUEL_GAUGE_IMPLEMENTATION_H__
#define __WRD_FUEL_GAUGE_IMPLEMENTATION_H__

#include "mbed-drivers/mbed.h"
#include "core-util/FunctionPointer.h"

#include "wrd-fuel-gauge/FuelGaugeBase.h"

using namespace mbed::util;

class FuelGaugeImplementation : public FuelGaugeBase
{
public:
    typedef enum {
        FUEL_GAUGE_ADDRESS = 0x6C
    } address_t;

    typedef enum {
        REGISTER_VCELL          = 0x02,
        REGISTER_SOC            = 0x04,
        REGISTER_MODE           = 0x06,
        REGISTER_VERSION        = 0x08,
        REGISTER_HIBRT          = 0x0A,
        REGISTER_CONFIG         = 0x0C,
        REGISTER_VALRT          = 0x14,
        REGISTER_CRATE          = 0x16,
        REGISTER_VRESET_ID      = 0x18,
        REGISTER_STATUS         = 0x1A,
        REGISTER_TABLE_START    = 0x40,
        REGISTER_CMD            = 0xFE
    } register_t;

    FuelGaugeImplementation(void);

    /**
     * @brief Get battery level in permille.
     *
     * @param callback Function to be called with the fuel level as parameter.
     */
    virtual void getPerMille(FunctionPointer1<void, uint16_t> callback);

    /**
     * @brief Get battery voltage in milli volt.
     *
     * @param callback Function to be called with the battery voltage as parameter.
     */
    virtual void getMilliVolt(FunctionPointer1<void, uint16_t> callback);

    /**
     * @brief Set callback function for when the battery level changes.
     * @details The callback function is be called every time the battery
     *          level changes approximately +/- 1%. The exact value may depend
     *          upon the granularity supported by the hardware and what offers
     *          the best tradeof between power consumption and precision.
     *
     * @param callback Function to be called when the battery level has changed
     *                 with the new level in permille as the parameter.
     */
    virtual void setPerMilleChangeCallback(FunctionPointer1<void, uint16_t> callback);

    /**
     * @brief Cancel callback function set up by setPerMilleChangeCallback.
     *
     * @param callback Function to be removed from callback list.
     */
    virtual void cancelCallback(FunctionPointer1<void, uint16_t> callback);

private:
    void getPerMilleDone(uint16_t);
    void getMilliVoltDone(uint16_t);

private:
    void getRegister(register_t reg, FunctionPointer1<void, uint16_t>& callback);
    void getRegisterDone(Buffer txBuffer, Buffer rxBuffer, int code);

    void setRegister(register_t reg, uint16_t value, FunctionPointer1<void, uint16_t>& callback);
    void setRegisterDone(Buffer txBuffer, Buffer rxBuffer, int code);

    void alertISR();

    I2C i2c;
//    InterruptIn irq;

    char memoryWrite[3];
    char memoryRead[2];

    uint16_t registerValue;
    FunctionPointer1<void, uint16_t> registerCallback;
    FunctionPointer1<void, uint16_t> externalCallback;
};
#if 0
    /*  Reset fuel gauge monitor.
    */
    void reset(void (*_callback)(uint16_t))
    {
        FunctionPointer1<void, uint16_t> callback(_callback);

        setRegister(REGISTER_CMD, 0x5400, callback);
    }

    /*  Set threshold for when to alert about an emepty battery.
    */
    void setEmptyAlert(uint16_t threshold, void (*_callback)(void));

    /*  Clear alert status bit.
    */
    void clearAlert(void (*_callback)(void));

    /*  Enable/disable State of Charge change alert.
    */
    void enableStateChangeAlert(void (*_callback)(void));
    void disableStateChangeAlert(void (*_callback)(void));

    /*  Enable/disable sleep mode.
    */
    void enableSleep(void (*_callback)(void));
    void disableSleep(void (*_callback)(void));


    /*  Read voltage register and call callback.
    */
    template <typename T>
    void getVoltage(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_VCELL, returnValue, callback);
    }

    void getVoltage(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_VCELL, returnValue, callback);
    }

    /*  Read percentage register and call callback.
    */
    template <typename T>
    void getPercentage(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_SOC, returnValue, callback);
    }

    void getPercentage(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_SOC, returnValue, callback);
    }

    /*  Read version register and call callback.
    */
    template <typename T>
    void getVersion(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_VERSION, returnValue, callback);
    }

    void getVersion(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_VERSION, returnValue, callback);
    }

    /*  Read hibernate register and call callback.
    */
    template <typename T>
    void getHibernate(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_HIBRT, returnValue, callback);
    }

    void getHibernate(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_HIBRT, returnValue, callback);
    }

    /*  Read config register and call callback.
    */
    template <typename T>
    void getConfig(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_CONFIG, returnValue, callback);
    }

    void getConfig(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_CONFIG, returnValue, callback);
    }

    /*  Read alert register and call callback.
    */
    template <typename T>
    void getAlert(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_VALRT, returnValue, callback);
    }

    void getAlert(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_VALRT, returnValue, callback);
    }

    /*  Read (dis)charge rate register and call callback.
    */
    template <typename T>
    void getRate(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_CRATE, returnValue, callback);
    }

    void getRate(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_CRATE, returnValue, callback);
    }

    /*  Read reset and id register and call callback.
    */
    template <typename T>
    void getResetID(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_VRESET_ID, returnValue, callback);
    }

    void getResetID(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_VRESET_ID, returnValue, callback);
    }

    /*  Read status register and call callback.
    */
    template <typename T>
    void getStatus(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_STATUS, returnValue, callback);
    }

    void getStatus(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_STATUS, returnValue, callback);
    }

    /*  Read command register and call callback.
    */
    template <typename T>
    void getCommand(uint16_t* returnValue, T* object, void (T::*member)(void))
    {
        FunctionPointer callback(object, member);

        getRegister(REGISTER_CMD, returnValue, callback);
    }

    void getCommand(uint16_t* returnValue, void (*_callback)(void))
    {
        FunctionPointer callback(_callback);

        getRegister(REGISTER_CMD, returnValue, callback);
    }

#endif

#endif // __WRD_FUEL_GAUGE_IMPLEMENTATION_H__
