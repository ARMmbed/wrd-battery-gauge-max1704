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

#ifndef __WRD_BATTERY_GAUGE_IMPLEMENTATION_H__
#define __WRD_BATTERY_GAUGE_IMPLEMENTATION_H__

#include "mbed-drivers/mbed.h"

#include "wrd-battery-gauge/BatteryGaugeBase.h"
#include "wrd-utilities/I2CRegister.h"
#include "wrd-gpio-switch/InterruptInEx.h"

using namespace mbed::util;

class BatteryGaugeImplementation : public BatteryGaugeBase
{
public:
    typedef enum {
        BATTERY_GAUGE_ADDRESS = 0x6C
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

    BatteryGaugeImplementation(void);

    /**
     * @brief Get battery level in permille.
     *
     * @param callback Function to be called with the battery level as parameter.
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
    void getPerMilleDone(void);
    void getMilliVoltDone(void);

private:
    void setRegister(register_t reg, uint16_t value, FunctionPointer0<void>& callback);

    void alertISR();

    I2CRegister i2c;
    InterruptInEx irq;

    char memoryWrite[3];
    char memoryRead[2];

    FunctionPointer1<void, uint16_t> externalCallback;
};

#endif // __WRD_BATTERY_GAUGE_IMPLEMENTATION_H__
