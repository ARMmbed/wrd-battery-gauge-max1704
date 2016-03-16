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

#include "wrd-battery-gauge/BatteryGaugeImplementation.h"

#if YOTTA_CFG_HARDWARE_WRD_BATTERY_GAUGE_PRESENT
#else
#error WRD Battery Gauge missing
#endif

BatteryGaugeImplementation::BatteryGaugeImplementation(void)
    :   BatteryGaugeBase(),
        i2c(YOTTA_CFG_HARDWARE_WRD_BATTERY_GAUGE_I2C_SDA,
            YOTTA_CFG_HARDWARE_WRD_BATTERY_GAUGE_I2C_SCL),
        irq(YOTTA_CFG_HARDWARE_WRD_BATTERY_GAUGE_IRQ_PIN,
            YOTTA_CFG_HARDWARE_WRD_BATTERY_GAUGE_IRQ_LOCATION)
{
    i2c.frequency(400000);

//    irq.mode(PullUp);
    // Delay for initial pullup to take effect
//    wait(.01);
//    irq.fall(this, &BatteryGaugeImplementation::alertISR);
}

/*  Get battery level in per mille.
*/
void BatteryGaugeImplementation::getPerMille(FunctionPointer1<void, uint16_t> callback)
{
    externalCallback = callback;

    FunctionPointer0<void> fp(this, &BatteryGaugeImplementation::getPerMilleDone);
    i2c.read(BATTERY_GAUGE_ADDRESS, REGISTER_SOC, memoryRead, 2, fp);
}

void BatteryGaugeImplementation::getPerMilleDone(void)
{
    if (externalCallback)
    {
        uint16_t value = ((uint16_t) memoryRead[0] << 8) | memoryRead[1];

        // remove rounding error when converting percent to per mille
        if (value > 100 * 256)
        {
            value = 100 * 256;
        }

        externalCallback.call(value * 10 / 256);
    }
}

/*  Get battery voltage in milli volt.
*/
void BatteryGaugeImplementation::getMilliVolt(FunctionPointer1<void, uint16_t> callback)
{
    externalCallback = callback;

    FunctionPointer0<void> fp(this, &BatteryGaugeImplementation::getMilliVoltDone);
    i2c.read(BATTERY_GAUGE_ADDRESS, REGISTER_VCELL, memoryRead, 2, fp);
}

void BatteryGaugeImplementation::getMilliVoltDone()
{
    if (externalCallback)
    {
        uint16_t value = ((uint16_t) memoryRead[0] << 8) | memoryRead[1];

        externalCallback.call(((uint64_t) value) * 78125 / 1000000);
    }
}

void BatteryGaugeImplementation::setPerMilleChangeCallback(FunctionPointer1<void, uint16_t> callback)
{
    (void) callback;
}

void BatteryGaugeImplementation::cancelCallback(FunctionPointer1<void, uint16_t> callback)
{
    (void) callback;
}

/*  Generic functions for reading and writing registers.
*/
void BatteryGaugeImplementation::setRegister(register_t reg, uint16_t value, FunctionPointer0<void>& callback)
{
    memoryWrite[0] = value >> 8;
    memoryWrite[1] = value;

    i2c.write(BATTERY_GAUGE_ADDRESS, reg, memoryWrite, 2, callback);
}

void BatteryGaugeImplementation::alertISR()
{
//    printf("alert\r\n");
}

