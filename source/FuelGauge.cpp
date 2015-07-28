/* mbed
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "fuel-gauge-max1704/FuelGauge.h"



#if 1
#include "swo/swo.h"
#define printf(...) { swoprintf(__VA_ARGS__); }
#else
#define printf(...)
#endif


FuelGauge::FuelGauge(PinName _sda, PinName _scl, PinName _irq)
    :   i2c(_sda, _scl),
        irq(_irq)
{
    i2c.frequency(400000);

    irq.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.01);
    irq.fall(this, &FuelGauge::alertISR);
}

void FuelGauge::getRegister(register_t reg, uint16_t* _returnValuePointer, FunctionPointer& _callback)
{
    returnValuePointer = _returnValuePointer;
    externalCallback = _callback;

    memoryWrite[0] = reg;

    event_callback_t callback(this, &FuelGauge::getRegisterDone);

    i2c.transfer(FUEL_GAUGE_ADDRESS, memoryWrite, 1, memoryRead, 2, callback);
}

void FuelGauge::getRegisterDone(int code __attribute__((unused)))
{
    *returnValuePointer = ((uint16_t) memoryRead[0] << 8) | memoryRead[1];

    if (externalCallback)
    {
        externalCallback.call();
    }
}



void FuelGauge::setRegister(register_t reg, uint16_t value, FunctionPointer& _callback)
{
    externalCallback = _callback;

    memoryWrite[0] = reg;
    memoryWrite[1] = value >> 8;
    memoryWrite[2] = value;

    event_callback_t callback(this, &FuelGauge::setRegisterDone);

    i2c.transfer(FUEL_GAUGE_ADDRESS, memoryWrite, 3, memoryRead, 0, callback);
}

void FuelGauge::setRegisterDone(int code __attribute__((unused)))
{
    if (externalCallback)
    {
        externalCallback.call();
    }
}



void FuelGauge::alertISR()
{
    printf("alert\r\n");
}

