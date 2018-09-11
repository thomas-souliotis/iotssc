/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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

#ifndef __BLE_LED_SERVICE_H__
#define __BLE_LED_SERVICE_H__

class LEDService {
public:
    const static uint16_t LED_SERVICE_UUID              = 0xA000;
    const static uint16_t LED_STATE_CHARACTERISTIC_UUID = 0xA001;

    LEDService(BLEDevice &_ble, bool initialValueForLEDCharacteristic) :
        ble(_ble), ledState(LED_STATE_CHARACTERISTIC_UUID, &initialValueForLEDCharacteristic)
    {
        GattCharacteristic *charTable[] = {&ledState};
        GattService         ledService(LED_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(ledService);
    }

    GattAttribute::Handle_t getValueHandle() const
    {
        return ledState.getValueHandle();
    }

private:
    BLEDevice                         &ble;
    ReadWriteGattCharacteristic<bool> ledState;
};


class AddressRssSensorService {
public:
    const static uint16_t MY_SERV = 0xA002;
    const static uint16_t MY_CHAR = 0xA003;
    AddressRssSensorService(BLE &_ble, uint8_t init) :
        ble(_ble),
        everyCharacteristic(MY_CHAR, &val[0], GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {
                val[0]= init;       
                val[1]= init;       
                val[2]= init;       
                val[3]= init;
                val[4]= init;       
                val[5]= init;       
                val[6]= init; 
                val[7]= init;
                /*       
                val[8]= init;       
                val[9]= init; 
                val[10]= init;*/       
        GattCharacteristic *charTable[] = {&everyCharacteristic};
        GattService         everyService(GattService::UUID_BATTERY_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(everyService);
    }
    void updateAny(uint8_t newVal0, uint8_t newVal1, uint8_t newVal2, uint8_t newVal3, uint8_t newVal4, uint8_t newVal5, uint8_t newVal6, uint8_t newVal7/*, uint8_t newVal8, uint8_t newVal9, uint8_t newVal10*/) {
        val[0]  = newVal0;
        val[1]  = newVal1;
        val[2]  = newVal2;    
        val[3]  = newVal3;
        val[4]  = newVal4;
        val[5]  = newVal5;
        val[6]  = newVal6;
        val[7]  = newVal7;
        /*
        val[8]  = newVal8;
        val[9]  = newVal9;
        val[10] = newVal10;*/   
        ble.gattServer().write(everyCharacteristic.getValueHandle(),  &val[0], sizeof(uint8_t) *8);
    }

protected:
    BLE &ble;
    uint8_t    val[11];
    ReadOnlyArrayGattCharacteristic<uint8_t,sizeof(uint8_t)*8> everyCharacteristic;
};

/*
class AddressRssSensorService {
public:
    AddressRssSensorService(BLE &_ble, uint8_t init = 0):
        ble(_ble),
        id(init),
        rss(init),
        accX(init),
        accY(init),
        accZ(init),
        gyrX(init),
        gyrY(init),
        gyrZ(init),        
        firstCharacteristic(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &id, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        secondCharacteristic(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &rss, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        xxAcc(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &accX, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        yyAcc(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &accY, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        zzAcc(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &accZ, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        xxGyr(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &gyrX, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        yyGyr(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &gyrY, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        zzGyr(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &gyrZ, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY){
            
        GattCharacteristic *charTable[] = {&firstCharacteristic, &secondCharacteristic, &xxAcc, &yyAcc, &zzAcc, &xxGyr, &yyGyr, &zzGyr};
        GattService         addressRssSensorService(GattService::UUID_BATTERY_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(addressRssSensorService);
    }
    void updateId(uint8_t newId) {
        id = newId;
        ble.gattServer().write(firstCharacteristic.getValueHandle(), &id, 1);
    }
    void updateAccX(uint8_t newX) {
        accX = newX;
        ble.gattServer().write(secondCharacteristic.getValueHandle(), &accX, 1);
    }
    void updateAccY(uint8_t newY) {
        accY = newY;
        ble.gattServer().write(xxAcc.getValueHandle(), &accY, 1);
    }
    void updateAccZ(uint8_t newZ) {
        accZ = newZ;
        ble.gattServer().write(yyAcc.getValueHandle(), &accZ, 1);
    }
    void updateGyrX(uint8_t newX) {
        gyrX = newX;
        ble.gattServer().write(zzAcc.getValueHandle(), &gyrX, 1);
    }
    void updateGyrY(uint8_t newY) {
        gyrY = newY;
        ble.gattServer().write(xxGyr.getValueHandle(), &gyrY, 1);
    }
    void updateGyrZ(uint8_t newZ) {
        gyrZ = newZ;
        ble.gattServer().write(yyGyr.getValueHandle(), &gyrZ, 1);
    }
    void updateRss(uint8_t newRss) {
        rss = newRss;
        ble.gattServer().write(zzGyr.getValueHandle(), &rss, 1);
    }
    
    
protected:
    BLE &ble;
    uint8_t    id;
    uint8_t    rss;
    uint8_t    accX;
    uint8_t    accY;
    uint8_t    accZ;    
    uint8_t    gyrX;
    uint8_t    gyrY;
    uint8_t    gyrZ;
    ReadOnlyGattCharacteristic<uint8_t> firstCharacteristic;
    ReadOnlyGattCharacteristic<uint8_t> secondCharacteristic;
    ReadOnlyGattCharacteristic<uint8_t> xxAcc;
    ReadOnlyGattCharacteristic<uint8_t> yyAcc;
    ReadOnlyGattCharacteristic<uint8_t> zzAcc;
    ReadOnlyGattCharacteristic<uint8_t> xxGyr;
    ReadOnlyGattCharacteristic<uint8_t> yyGyr;
    ReadOnlyGattCharacteristic<uint8_t> zzGyr;
};
*/
/*
class SensorService {
public:
    SensorService(BLE &_ble, uint8_t level = 100, uint8_t level2 = 100, uint8_t level3 = 100 ):
        ble(_ble),
        x(level),
        y(level2),
        z(level3),
        xx(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &x, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        yy(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &y, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        zz(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &z, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY){

        GattCharacteristic *charTable[] = {&xx, &yy, &zz};
        GattService         sensorService(GattService::UUID_BATTERY_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(sensorService);
    }
    void updateX(uint8_t newX) {
        x = newX;
        ble.gattServer().write(xx.getValueHandle(), &x, 1);
    }
    
    void updateY(uint8_t newY) {
        y = newY;
        ble.gattServer().write(yy.getValueHandle(), &y, 1);
    }
    void updateZ(uint8_t newZ) {
        z = newZ;
        ble.gattServer().write(zz.getValueHandle(), &z, 1);
    }
    
protected:
    BLE &ble;
    uint8_t    x;
    uint8_t    y;
    uint8_t    z;
    ReadOnlyGattCharacteristic<uint8_t> xx;
    ReadOnlyGattCharacteristic<uint8_t> yy;
    ReadOnlyGattCharacteristic<uint8_t> zz;
};

*/

#endif /* #ifndef __BLE_LED_SERVICE_H__ */
