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

#if 0
#include "swo/swo.h"
#define printf(...) { swoprintf(__VA_ARGS__); }
#else
#define printf(...)
#endif


BatteryGaugeImplementation::BatteryGaugeImplementation(void)
    :   BatteryGaugeBase(),
        i2c(YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_GAUGE_I2C_SDA,
            YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_GAUGE_I2C_SCL)
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

    FunctionPointer1<void, uint16_t> fp(this, &BatteryGaugeImplementation::getPerMilleDone);

    getRegister(REGISTER_SOC, fp);
}

void BatteryGaugeImplementation::getPerMilleDone(uint16_t value)
{
    if (externalCallback)
    {
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

    FunctionPointer1<void, uint16_t> fp(this, &BatteryGaugeImplementation::getMilliVoltDone);

    getRegister(REGISTER_VCELL, fp);
}

void BatteryGaugeImplementation::getMilliVoltDone(uint16_t value)
{
    if (externalCallback)
    {
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
void BatteryGaugeImplementation::getRegister(register_t reg, FunctionPointer1<void, uint16_t>& _callback)
{
    registerCallback = _callback;

    memoryWrite[0] = reg;

    I2C::event_callback_t callback(this, &BatteryGaugeImplementation::getRegisterDone);

    i2c.transfer(BATTERY_GAUGE_ADDRESS, memoryWrite, 1, memoryRead, 2, callback);
}

void BatteryGaugeImplementation::getRegisterDone(Buffer txBuffer, Buffer rxBuffer, int code)
{
    (void) txBuffer;
    (void) rxBuffer;
    (void) code;

    registerValue = ((uint16_t) memoryRead[0] << 8) | memoryRead[1];

    if (registerCallback)
    {
        registerCallback.call(registerValue);
    }
}



void BatteryGaugeImplementation::setRegister(register_t reg, uint16_t value, FunctionPointer1<void, uint16_t>& _callback)
{
    registerCallback = _callback;

    memoryWrite[0] = reg;
    memoryWrite[1] = value >> 8;
    memoryWrite[2] = value;

    I2C::event_callback_t callback(this, &BatteryGaugeImplementation::setRegisterDone);

    i2c.transfer(BATTERY_GAUGE_ADDRESS, memoryWrite, 3, memoryRead, 0, callback);
}

void BatteryGaugeImplementation::setRegisterDone(Buffer, Buffer, int)
{
    if (registerCallback)
    {
        registerCallback.call(0);
    }
}



void BatteryGaugeImplementation::alertISR()
{
    printf("alert\r\n");
}

