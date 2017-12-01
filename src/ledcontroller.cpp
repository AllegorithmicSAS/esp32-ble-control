#include "ledcontroller.h"

#include <Arduino.h>
#include <cmath>
#include <nvs_flash.h>

struct NVS
{
  NVS()
  {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
      // NVS partition was truncated and needs to be erased
      // Retry nvs_flash_init
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    nvs_open("nvs", NVS_READWRITE, &hdl);
  }

  template <typename T>
  void write(const char *ns, const T &value)
  {
    nvs_set_blob(hdl, ns, (const void *)&value, sizeof(T));
  }

  template <typename T>
  size_t read(const char *ns, T &value) const
  {
    size_t sz = 0;
    nvs_get_blob(hdl, ns, (void *)&value, &sz);
    return sz;
  }

private:
  nvs_handle hdl;
};

LedController::LedController(ILedDriver *ledDriver) : chaserMode(true), ledDriver(ledDriver), nvs(new NVS)
{
  brightnessesNonChaser.resize(ledDriver->info()->nChannels);
  nvs->read("chaseParams", chaserParams);
  setChaserParams(chaserParams);
}

LedController::~LedController()
{
}

// return true if keep the same mode
bool LedController::checkedDelay(size_t ms)
{
  ms = std::max<size_t>(ms, 200);
  const bool prevMode = chaserMode;
  const size_t count = ms / 200;
  for (size_t i = 0; i < count; i++)
    delay(200);
  delay(ms % 200);
  return prevMode == chaserMode;
}

void LedController::loop()
{
  if (chaserMode)
  {
    const size_t nChannels = ledDriver->info()->nChannels;
    for (size_t i = 0; i < nChannels; i++)
    {
      ledDriver->setBrightness((i + nChannels - 1) % nChannels, 0.f);
      ledDriver->setBrightness(i, chaserParams.singleLedBrightness);
      if (!checkedDelay(size_t(chaserParams.periodInSecs * 1000.f)))
        return;
    }

    for (size_t i = 0; i < nChannels; i++)
      ledDriver->setBrightness(i, chaserParams.multiLedBrightness);
    if (!checkedDelay(size_t(chaserParams.periodInSecs * 1000.f)))
      return;
    for (size_t i = 0; i < nChannels; i++)
      ledDriver->setBrightness(i, 0.f);
    if (!checkedDelay(size_t(chaserParams.periodInSecs * 1000.f)))
      return;
  }
  else
  {
    if (!checkedDelay(200))
      return;

    for (size_t idx = 0; idx < brightnessesNonChaser.size(); idx++)
    {
      // TODO: mutex?
      ledDriver->setBrightness(idx, brightnessesNonChaser[idx]);
    }
  }
}

template <typename T>
T saturate(T val, T min, T max)
{
  return std::min(std::max(val, min), max);
}

void LedController::setChaserParams(const ChaserParams &params)
{
  // TODO: mutex?
  chaserParams = params;
  chaserParams.multiLedBrightness = saturate(chaserParams.multiLedBrightness, 0.f, 1.f);
  chaserParams.singleLedBrightness = saturate(chaserParams.singleLedBrightness, 0.f, 1.f);
  chaserParams.periodInSecs = saturate(chaserParams.periodInSecs, 0.3f, 10.f);
  nvs->write("chaseParams", chaserParams);
  chaserMode = true;
}
