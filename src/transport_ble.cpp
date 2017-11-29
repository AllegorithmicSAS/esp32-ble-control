#include "transport_ble.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <string.h>
// #include <ArduinoJson.h>

#define SERVICE_UUID             "e9f2d3cf-bc82-4a6d-a11b-0efa2345e4de"
#define CHARACTERISTIC_INFO_UUID "e5710358-a899-44de-89f8-03882aca5bb1"
#define CHARACTERISTIC_BRIGHTNESSES_UUID "9d26c44b-24f3-48d0-ab51-603aa2da1c5a"

class Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        Serial.println("new value = ");
        Serial.println(value.c_str());
    }
};

struct InfoMessage {
    uint8_t channels;
    char name[20];
};

struct ChannelBrightness {
    int8_t channel;
    uint8_t brightness;
};

struct BrightnessMessage {
    ChannelBrightness brightnesses[8];
};

static_assert(sizeof(InfoMessage) == 21, "invalid padding for struct");
static_assert(sizeof(BrightnessMessage) == 16, "invalid padding for struct");

void transportBleInit(ILedDriver * driver) {
    BLEDevice::init("Led Driver");

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    {
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_INFO_UUID,
            BLECharacteristic::PROPERTY_READ
        );

        auto infoMsg = InfoMessage{
            .channels = uint8_t(driver->info()->nChannels),
            .name = {'\0'},
        };
        strncpy(infoMsg.name, driver->info()->name, sizeof(infoMsg.name) - 1);
        pCharacteristic->setValue((uint8_t*)&infoMsg, sizeof(infoMsg));
    }

    {
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_BRIGHTNESSES_UUID,
            BLECharacteristic::PROPERTY_WRITE_NR
        );
        pCharacteristic->setCallbacks(new Callbacks());
    }
  
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}
