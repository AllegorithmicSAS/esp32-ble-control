#pragma once

#include <vector>
#include "leddriver.h"

struct NVS;

struct ChaserParams
{
  ChaserParams() : singleLedBrightness(0.75f), multiLedBrightness(0.25f), periodInSecs(2.f) {}

  float singleLedBrightness;
  float multiLedBrightness;
  float periodInSecs;
};

struct LedController
{
  LedController(ILedDriver *ledDriver);

  ~LedController();

  bool checkedDelay(size_t ms);

  inline void setBrightness(size_t channel, float brightness)
  {
    // TODO: mutex ?
    chaserMode = false;
    brightnessesNonChaser[channel] = brightness;
  }

  inline const LedDriverInfo *info() const
  {
    return ledDriver->info();
  }

  void setChaserParams(const ChaserParams &params);

  void loop();

private:
  bool chaserMode;
  ChaserParams chaserParams;
  ILedDriver *ledDriver;
  std::unique_ptr<NVS> nvs;
  std::vector<float> brightnessesNonChaser;
};
