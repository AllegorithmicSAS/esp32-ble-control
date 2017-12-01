#include <Arduino.h>

#include "logger.h"
#include "leddriver.h"
#include "ledcontroller.h"
#include "transport_ble.h"

static auto Log = Logger("EntryPoint");

std::unique_ptr<ILedDriver> ledDriver;
std::unique_ptr<LedController> ledController;

void setup()
{
    Serial.begin(115200);

    Log.info("starting firmware");

    Log.info("starting led dummy driver");
    // ledDriver = makeDummyLedDriver();
    ledDriver = makeTLC59116LedDriver();
    ledDriver->init();

    ledController.reset(new LedController(ledDriver.get()));

    Log.info("starting BLE transport");
    transportBleInit(ledController.get());

    Log.info("done starting firmware");
}

void loop()
{
    ledController->loop();
}
