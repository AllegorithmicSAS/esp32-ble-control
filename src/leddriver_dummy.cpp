#include "leddriver.h"

#include <Arduino.h>

#include "logger.h"

static auto Log = Logger("LedDriverDummy");

static const auto DriverInfo =  LedDriverInfo{
    .nChannels = 1,
    .name = "dummy driver"
};

// GPIO on which the built in led is connected (might vary depending on debug board)
// 2,5,16 are common values - some boards don't even have a built in LED
const uint8_t BuiltInLed = 2;

struct LedDriverDummy : public ILedDriver {
protected:
    bool init() override {
        pinMode(BuiltInLed, OUTPUT);
        digitalWrite(BuiltInLed, HIGH); // led off - led is wired as open drain config
    }

    void setBrightness(size_t channel, float brightness) override {
        if (channel != 0) {
            Log.error("this driver only support 1 channel, ignoring call to setBrightness for channel %d", channel);
            return;
        }
        digitalWrite(BuiltInLed, brightness > .5f ? LOW : HIGH);
    }
    
    const LedDriverInfo * info() const override {
        return &DriverInfo;
    }
};

std::unique_ptr<ILedDriver> makeDummyLedDriver() {
    return std::unique_ptr<ILedDriver>(new LedDriverDummy);
}
