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


#include "mbed-drivers/mbed.h"

#include "wrd-battery-gauge/BatteryGaugeImplementation.h"

#include "swo/swo.h"

#if !YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_GAUGE_PRESENT
#error platform not supported
#endif


BatteryGaugeImplementation monitor(YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_GAUGE_I2C_SDA,
                                   YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_GAUGE_I2C_SCL,
                                   YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_GAUGE_PIN_IRQ);

static void milliVoltDone(int milliVolt)
{
    swoprintf("volt: %d.%03dV\r\n", milliVolt / 1000, milliVolt % 1000);
}

static void permilleDone(int charge)
{
    swoprintf("charge: %d.%d%%\r\n", charge / 10, charge % 10);

    monitor.getMilliVolt(milliVoltDone);
}

static void batteryTask()
{
    swoprintf("battery\r\n");

    monitor.getPerMille(permilleDone);

//    monitor.getVoltage(&value, voltageDone);
//    monitor.getPercentage(&version, i2cDone);
//    monitor.getVersion(&version, i2cDone);
//    monitor.getHibernate(&version, i2cDone);
//    monitor.getConfig(&version, i2cDone);
//    monitor.getAlert(&version, i2cDone);
//    monitor.getRate(&version, i2cDone);
//    monitor.getResetID(&version, i2cDone);
//    monitor.getStatus(&version, i2cDone);
//    monitor.getCommand(&version, i2cDone);
}

void app_start(int, char *[])
{
    minar::Scheduler::postCallback(batteryTask);
}
