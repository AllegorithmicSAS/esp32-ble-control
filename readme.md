# Led Driver firmware

This project contains the firmware for the Led Driver. It currently features Bluetooth Low Energy control. It supports 2 modes of control:

* Chaser mode : Drives all LEDs in turn, one at a time, then enables all LEDs at once. Brightness is adjustable for the _one at a time_ cycle, and also adjustable for the _all on_ cycle. Period is adjustable too. See the **Chaser Control Characteristic** for more details.
* Manual mode : You can drive each LED independently with full control over brightness of each LED. See the **Brightness Control Characteristic** for more details.

# Bluetooth details

* Device name: `Led Driver`
* Led Driver Service UUID : `e9f2d3cf-bc82-4a6d-a11b-0efa2345e4de`

The device exposes multiple characteristics to control the various parameters of the controller board.

### Info Characteristic

* **Purpose** : Get all information about the driver : number of controllable LEDs and name of the driver.
* **UUID** = `e5710358-a899-44de-89f8-03882aca5bb1`
* **Read only**

Value format:

```go
struct {
    // number of controllable LEDs on this board
    ChannelCount uint8

    // name of the board
    BoardName   [20]byte
}
```

### Brightness Control Characteristic

* **Purpose** : Control the brightness of each LED
* **UUID** : `9d26c44b-24f3-48d0-ab51-603aa2da1c5a`
* **Write only**

Value format, **variable size**:

```go
[]struct {
    // which channel to affect
    // valid range [0,ChannelCount[
    Channel    uint8

    // brightness value : 0 = off, 255 = full power
    // valid range [0,255]
    Brightness uint8
}
```

> **BEWARE**: This is an array. You can thus set multiple brightnesses at once, limited by max size of BLE value (20 bytes usually). So you can only control 10 LEDs by sending 1 BLE value packet. If you need to control more LEDs, just send more packets.

### Chaser Control Characteristic

* **Purpose** : Control the chaser mode parameters: brightnesses of both cycles and timings.
* **UUID** : `dfecd1b6-d406-41ef-a12d-5178940f575b`
* **Write only**

Value format:

```go
struct {
    // brightness for one at a time cycle (valid range [0,255])
    OneAtATimeBrightness uint8

    // brightness for all-on cycle (valid range [0,255])
    AllOnBrightness      uint8

    // period in tenth of secs (100ms) : the period to wait between
    // each change in the one at a time cycle.
    // (valid range [1-100])
    Period               uint8
}
```

# Building the project

### Requirements

* Platform IO Core : `brew install platformio`
* USB to UART driver :
  https://www.silabs.com/documents/public/software/Mac_OSX_VCP_Driver.zip

### Initial setup of the project

    make init

### Upload to the board

    make upload

# Test the board from Chrome

https://clems71.github.io/esp32-ble-control/
