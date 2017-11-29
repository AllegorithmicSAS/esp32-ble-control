#include <Arduino.h>

#include "logger.h"
#include "leddriver.h"
#include "transport_ble.h"
#include "transport_wifi.h"

static auto Log = Logger("EntryPoint");
std::unique_ptr<ILedDriver> ledDriver;

void setup() {
    Serial.begin(115200);

    Log.error("toto");
    Log.info("starting firmware");

    Log.info("starting led dummy driver");
    ledDriver = makeDummyLedDriver();
    ledDriver->init();

    Log.info("starting BLE transport");
    transportBleInit(ledDriver.get());

    Log.info("starting Wifi transport");
    transportWifiInit(ledDriver.get());

    Log.info("done starting firmware");
}

void loop() {
    ledDriver->setBrightness(0, 1.f);
    delay(100);
    ledDriver->setBrightness(0, 0.f);
    delay(1000);
}
