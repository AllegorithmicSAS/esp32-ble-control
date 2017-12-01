// This is the implementation of the led controller. It currently relies on
// 2x TLC59116 on I2C bus, one with hardware address 0 and one with
// hardware address 1.
// On each TLC59116, 12 outputs are dedicated to 4 high power LEDs (outputs are
// coupled by 3), and 4 outputs are dedicated to 4 standard LEDs.
// The total number of LEDs controllable by this controller is thus 8 high power LEDs
// and 8 standard LEDs.
// Each of the TLC drivers is capable of ~104mA nominal per output
// (180ohms Rref resistor in place). Thus, the admissible power for the high power
// LEDs is 312mA.

#include "leddriver.h"

#include <Arduino.h>
#include <Wire.h>

#include "logger.h"

static auto Log = Logger("LedDriverTLC59116");

static const auto DriverInfo = LedDriverInfo{
    .nChannels = 8,
    .name = "8x hi-power only",
};

const uint8_t WIRE_RESET = 23;

// in mA
const float MAX_CURRENT_PER_OUT = 104.f;
// const float MAX_CURRENT_STD_OUT = 20.f;

struct LedDriverTLC59116 : public ILedDriver
{
  protected:
    bool init() override
    {
        Wire.begin(-1, -1, 400000U);

        // release TLC59116s from reset state
        pinMode(WIRE_RESET, OUTPUT);
        digitalWrite(WIRE_RESET, HIGH);

        // wait for TLC59116 to be up
        delay(250);

        // setup the drivers
        for (uint8_t addr = 0; addr < 2; addr++)
        {
            writeReg(addr, 0x00, 0x01);
            writeReg(addr, 0x01, 0x00);
            writeReg(addr, 0x14, 0xAA);
            writeReg(addr, 0x15, 0xAA);
            writeReg(addr, 0x16, 0xAA);
            writeReg(addr, 0x17, 0xAA);
        }
    }

    void setBrightness(size_t channel, float brightness) override
    {
        // TODO: standard leds
        if (channel >= 8)
        {
            Log.error("this driver only support 16 channels, ignoring call to setBrightness for channel %d", channel);
            return;
        }

        // conform the value to accepted range
        brightness = brightness > 1.f ? 1.f : (brightness < 0.f ? 0.f : brightness);

        // scale to proper 8bits range
        const uint8_t brightnessU8 = uint8_t(brightness * 255.f);

        // determine which of the 2 TLC59116 drives the LED channel
        const uint8_t tlcAddr = (channel >> 2) & 0x01;
        const uint8_t baseLedChannel = channel & 0x3;

        writeReg(tlcAddr, pwmRegister(baseLedChannel * 3 + 0), brightnessU8);
        writeReg(tlcAddr, pwmRegister(baseLedChannel * 3 + 1), brightnessU8);
        writeReg(tlcAddr, pwmRegister(baseLedChannel * 3 + 2), brightnessU8);
    }

    const LedDriverInfo *info() const override
    {
        return &DriverInfo;
    }

  private:
    uint8_t pwmRegister(uint8_t channel)
    {
        static const uint8_t TLC59116_PWM0 = 0x02;
        return TLC59116_PWM0 + channel;
    }

    void writeReg(uint8_t addr, uint8_t reg, uint8_t val)
    {
        Wire.beginTransmission(0x60 + addr);
        Wire.write(reg);
        Wire.write(val);
        Wire.endTransmission();
        delayMicroseconds(500);
    }
};

std::unique_ptr<ILedDriver> makeTLC59116LedDriver()
{
    return std::unique_ptr<ILedDriver>(new LedDriverTLC59116);
}
