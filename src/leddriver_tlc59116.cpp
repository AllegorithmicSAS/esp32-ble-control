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

namespace {
    enum Leds {
        HiPower1,
        HiPower2,
        HiPower3,
        HiPower4,
        HiPower5,
        HiPower6,
        HiPower7,
        HiPower8,
    
        Standard1,
        Standard2,
        Standard3,
        Standard4,
        Standard5,
        Standard6,
        Standard7,
        Standard8,
    };
    
    const uint8_t WIRE_RESET = 23;

    // in mA
    const float MAX_CURRENT_PER_OUT = 104.f;
    const float MAX_CURRENT_STD_OUT = 20.f;

    uint8_t pwmRegister(uint8_t channel) {
        static const uint8_t TLC59116_PWM0 = 0x02;
        return TLC59116_PWM0 + channel;
    }

    void writeReg(uint8_t addr, uint8_t reg, uint8_t val) {
        Wire.beginTransmission(0x60 + addr);
        Wire.write(reg);
        Wire.write(val);
        Wire.endTransmission();
    }
}

void ledsInit() {
    Wire.begin();

    // release TLC59116s from reset state
    pinMode(WIRE_RESET, OUTPUT);
    digitalWrite(WIRE_RESET, HIGH);
    
    // wait for TLC59116 to be up
    delay(100);

    // setup the drivers
    for (uint8_t addr=0; addr < 2; addr++) {
        writeReg(addr, 0x00, 0x01);
        writeReg(addr, 0x01, 0x00);
        writeReg(addr, 0x14, 0xAA);
        writeReg(addr, 0x15, 0xAA);
        writeReg(addr, 0x16, 0xAA);
        writeReg(addr, 0x17, 0xAA);
    }
}

// Set a Led brightness, value is normalized between 0 and 1
void ledsSetBrightness(Leds led, float value) {
    // conform the value to accepted range
    value = value > 1.f ? 1.f : (value < 0.f ? 0.f : value);

    if (led >= HiPower1 && led <= HiPower8) {
        // hi power leds
        const uint8_t addr = ((led - HiPower1) >> 2);
    } else {
        // low power leds
        const uint8_t pwmChannel = (led - Standard1) & 0x3;
        const uint8_t deviceAddr = ((led - Standard1) >> 2);
        static const float ledStdScale = 255.f * MAX_CURRENT_STD_OUT / MAX_CURRENT_PER_OUT;
        value *= ledStdScale;
        writeReg(deviceAddr, pwmRegister(pwmChannel), uint8_t(value));
    }
}
