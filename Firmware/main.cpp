#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "LSM9DS1.h"
#include "aes.h"
#include "aes2.h"
#include <math.h>
#include "LEDService.h"

uint8_t b1   = 205; //"ed:23:c0:d8:75:cd";
uint8_t b1p  = 117; //"ed:23:c0:d8:75:cd";
uint8_t b2   = 215; //"e7:31:1a:8e:b6:d7";
uint8_t b2p  = 182; //"e7:31:1a:8e:b6:d7";
uint8_t b3   = 23;  //"c7:bc:91:9b:2d:17";
uint8_t b3p  = 45;  //"c7:bc:91:9b:2d:17";
uint8_t b4   = 81;  //"ec:75:a5:ed:88:51";
uint8_t b4p  = 136; //"ec:75:a5:ed:88:51";
uint8_t b5   = 67;  //"fe:12:de:f2:c9:43";
uint8_t b5p  = 201; //"fe:12:de:f2:c9:43";
uint8_t b6   = 184; //"c0:3b:5c:fa:00:b8";
uint8_t b6p  = 0;   //"c0:3b:5c:fa:00:b8";
uint8_t b7   = 42;  //"e0:b8:3a:2f:80:2a";
uint8_t b7p  = 128; //"e0:b8:3a:2f:80:2a";
uint8_t b8   = 248; //"f1:55:76:cb:0c:f8";
uint8_t b8p  = 12;  //"f1:55:76:cb:0c:f8";
uint8_t b9   = 61;  //"f1:7f:b1:78:ea:3d";
uint8_t b9p  = 234; //"f1:7f:b1:78:ea:3d";
uint8_t b10  = 98;  //"fd:81:85:98:88:62";
uint8_t b10p = 136; //"fd:81:85:98:88:62";


// 128bit key
uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
uint8_t plaintext[16];
struct AES_ctx ctx;

DigitalOut led1(LED1, 1);
LSM9DS1 imu(p7, p30);

const static char     DEVICE_NAME[] = "Thomas";
static const uint16_t uuid16_list[] = {GattService::UUID_BATTERY_SERVICE};

float x,y,z;
static uint8_t rss = 101;
static uint8_t id  = 0, aa,ab,ac,ga,gb,gc;
static AddressRssSensorService* idPtr;
bool flag = false;

static EventQueue eventQueue(16 * EVENTS_EVENT_SIZE);


// Functions to regulate values in a good range
int gyrVal(float x){
    x         = x/10;
    int signX = 0;
    if (x<0){
        signX = 128;
        x     = -x;
    }
    if (x >127){
        x = 127 + signX;
    }else{
        x = x + signX;
    }
    return x;
}
int accVal(float x){
    x         = x*50;
    int signX = 0;
    if (x<0){
        signX = 128;
        x     = -x;
    }
    if (x >127){
        x = 127 + signX;
    }else{
        x = x + signX;
    }
    return x;
}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}

void scanFun(const Gap::AdvertisementCallbackParams_t *params){
    rss = -params->rssi;
    flag = false;
    // A process to check if received address corresponds to beacon id
    if (params->peerAddr[0]==b1) {
        if (params->peerAddr[1]==b1p) {
            id = 1;
            flag = true;
        }
    }
    else if (params->peerAddr[0]==b2){
        if (params->peerAddr[1]==b2p) {
            id = 2;
            flag = true;
        }
    } 
    else if (params->peerAddr[0]==b3){
        if (params->peerAddr[1]==b3p) {
            id = 3;
            flag = true;
        }
    }   
    else if (params->peerAddr[0]==b4){
        if (params->peerAddr[1]==b4p) {
            id = 4;
            flag = true;
        }
    }   
    else if (params->peerAddr[0]==b5){
        if (params->peerAddr[1]==b5p) {
            id = 5;
            flag = true;
        }
    }    
    else if (params->peerAddr[0]==b6){
        if (params->peerAddr[1]==b6p) {
            id = 6;
            flag = true;
        }
    }   
    else if (params->peerAddr[0]==b7){
        if (params->peerAddr[1]==b7p) {
            id = 7;
            flag = true;
        }
    }   
    else if (params->peerAddr[0]==b8){
        if (params->peerAddr[1]==b8p) {
            id = 8;
            flag = true;
        }
    }   
    else if (params->peerAddr[0]==b9){
        if (params->peerAddr[1]==b9p) {
            id = 9;
            flag = true;
        }
    }   
    else if (params->peerAddr[0]==b10){
        if (params->peerAddr[1]==b10p) {
            id = 10;
            flag = true;
        }
    }
    
    if (flag){
        imu.readAccel();
        x = imu.ax;
        y = imu.ay;
        z = imu.az;
        aa = accVal(x);
        ab = accVal(y);
        ac = accVal(z);
        imu.readGyro();
        x = imu.gx;
        y = imu.gy;
        z = imu.gz;
        ga = gyrVal(x);
        gb = gyrVal(y);
        gc = gyrVal(z);
        plaintext[0] = id;
        plaintext[1] = rss;
        plaintext[2] = aa;
        plaintext[3] = ab;
        plaintext[4] = ac;
        plaintext[5] = ga;
        plaintext[6] = gb;
        plaintext[7] = gc;
        /*
        plaintext[8] = id;
        plaintext[9] = rss;
        plaintext[10] = aa;
        plaintext[11] = ab;
        plaintext[12] = ac;
        plaintext[13] = ga;
        plaintext[14] = gb;
        plaintext[15] = gc;
        */
        // Comment out if you want encrypted data
        //AES_init_ctx(&ctx, key);
        //AES_ECB_encrypt(&ctx, plaintext);
        idPtr->updateAny(plaintext[0], plaintext[1], plaintext[2], plaintext[3], plaintext[4],plaintext[5],plaintext[6],plaintext[7]);
        //idPtr->updateAny(plaintext[8], plaintext[9], plaintext[10], plaintext[11], plaintext[12],plaintext[13],plaintext[14],plaintext[15]);
        flag = false;
    }   
}

void updateSensorValue() {
    BLE &ble = BLE::Instance();
    ble.gap().setScanParams(500, 500,  0, false);
    ble.gap().startScan(scanFun);
}




void blinkCallback(void)
{
    led1 = !led1; 
    BLE &ble = BLE::Instance();
    if (ble.gap().getState().connected) {
        eventQueue.call(updateSensorValue);
    }
}


void onBleInitError(BLE &ble, ble_error_t error)
{
}


void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);
    idPtr   = new AddressRssSensorService(ble, 0);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *) uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *) DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    //ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext* context) {
    BLE &ble = BLE::Instance();
    eventQueue.call(Callback<void()>(&ble, &BLE::processEvents));
}

int main()
{
    uint16_t status = imu.begin();
    eventQueue.call_every(500, blinkCallback);
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble.init(bleInitComplete);
    eventQueue.dispatch_forever();
    return 0;
}
