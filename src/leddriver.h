#pragma once

#include <string>
#include <memory>

struct LedDriverInfo {
    size_t        nChannels;
    const char *  name;
};

struct ILedDriver {
    ILedDriver() {}
    virtual ~ILedDriver() {}

    virtual bool init() {}
    virtual void setBrightness(size_t channel, float brightness) = 0;
    virtual const LedDriverInfo * info() const = 0;

private:
    ILedDriver( const ILedDriver& other ) = delete;
    ILedDriver& operator=( const ILedDriver& ) = delete;
};

std::unique_ptr<ILedDriver> makeDummyLedDriver();
