#include "transport_ble.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <string.h>

#include "ledcontroller.h"

#define SERVICE_UUID "e9f2d3cf-bc82-4a6d-a11b-0efa2345e4de"
#define CHARACTERISTIC_INFO_UUID "e5710358-a899-44de-89f8-03882aca5bb1"
#define CHARACTERISTIC_BRIGHTNESSES_UUID "9d26c44b-24f3-48d0-ab51-603aa2da1c5a"
#define CHARACTERISTIC_CHASER_UUID "dfecd1b6-d406-41ef-a12d-5178940f575b"

struct InfoMessage
{
    uint8_t channels;
    char name[20];
};

struct ChannelBrightnessMessage
{
    uint8_t channel;
    uint8_t brightness;
};

struct ChaserMessage
{
    uint8_t singleLedBrightness;
    uint8_t multiLedBrightness;
    uint8_t periodInTenthOfSecs;
};

class BrightnessCallbacks : public BLECharacteristicCallbacks
{
  public:
    BrightnessCallbacks(LedController *ledDriver) : ledDriver(ledDriver) {}

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        const std::string value = pCharacteristic->getValue();
        const size_t nChannels = value.size() >> 1;

        const ChannelBrightnessMessage *scratchMsg = (const ChannelBrightnessMessage *)value.c_str();
        for (size_t i = 0; i < nChannels; i++)
        {
            Serial.print("channel ");
            Serial.print(scratchMsg[i].channel);
            Serial.print(" = ");
            Serial.println(scratchMsg[i].brightness);
            ledDriver->setBrightness(scratchMsg[i].channel, float(scratchMsg[i].brightness) / 255.f);
        }
    }

  private:
    LedController *ledDriver;
};

class ChaserCallbacks : public BLECharacteristicCallbacks
{
  public:
    ChaserCallbacks(LedController *ledDriver) : ledDriver(ledDriver) {}

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        const std::string value = pCharacteristic->getValue();

        if (value.size() != sizeof(ChaserMessage))
        {
            // TODO: log error
            return;
        }

        const ChaserMessage *scratchMsg = (const ChaserMessage *)value.c_str();

        ChaserParams params;
        params.singleLedBrightness = float(scratchMsg->singleLedBrightness) / 255.f;
        params.multiLedBrightness = float(scratchMsg->multiLedBrightness) / 255.f;
        params.periodInSecs = float(scratchMsg->periodInTenthOfSecs) / 10.f;
        ledDriver->setChaserParams(params);
    }

  private:
    LedController *ledDriver;
};

static_assert(sizeof(InfoMessage) == 21, "invalid padding for struct");
static_assert(sizeof(ChannelBrightnessMessage) == 2, "invalid padding for struct");

void transportBleInit(LedController *driver)
{
    BLEDevice::init("Led Driver");

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // create info characteristic
    {
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_INFO_UUID,
            BLECharacteristic::PROPERTY_READ);

        auto infoMsg = InfoMessage{
            .channels = uint8_t(driver->info()->nChannels),
            .name = {'\0'},
        };
        strncpy(infoMsg.name, driver->info()->name, sizeof(infoMsg.name) - 1);
        pCharacteristic->setValue((uint8_t *)&infoMsg, sizeof(infoMsg));
    }

    // create brightnesses characteristic
    {
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_BRIGHTNESSES_UUID,
            BLECharacteristic::PROPERTY_WRITE_NR);
        pCharacteristic->setCallbacks(new BrightnessCallbacks(driver));
    }

    // create chaser characteristic
    {
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_CHASER_UUID,
            BLECharacteristic::PROPERTY_WRITE_NR);
        pCharacteristic->setCallbacks(new ChaserCallbacks(driver));
    }

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();
}
