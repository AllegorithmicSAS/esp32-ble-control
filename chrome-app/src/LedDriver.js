class _LedDriver {
  constructor() {
    this.channels = 0;
    this.name = '';
    this.charChaser = null;
    this.charBrightness = null;
  }

  async connect() {
    const ledDriverSvcId = 'e9f2d3cf-bc82-4a6d-a11b-0efa2345e4de';
    const charInfoId = 'e5710358-a899-44de-89f8-03882aca5bb1';
    const charLedBrightnessId = '9d26c44b-24f3-48d0-ab51-603aa2da1c5a';
    const charChaserId = 'dfecd1b6-d406-41ef-a12d-5178940f575b';

    const dev = await navigator.bluetooth.requestDevice({
      filters: [{ name: 'Led Driver' }],
      optionalServices: [ledDriverSvcId]
    });

    const server = await dev.gatt.connect();
    const ledDriverSvc = await server.getPrimaryService(ledDriverSvcId);
    this.charBrightness = await ledDriverSvc.getCharacteristic(
      charLedBrightnessId
    );
    this.charChaser = await ledDriverSvc.getCharacteristic(charChaserId);

    // Read board info
    const charInfo = await ledDriverSvc.getCharacteristic(charInfoId);
    const infoValue = await charInfo.readValue();
    const channels = infoValue.getUint8(0);

    let name = '';
    let idx = 1;
    while (true) {
      const byte = infoValue.getUint8(idx);
      if (byte === 0x00) break;
      name += String.fromCharCode(byte);
      idx++;
    }

    this.channels = channels;
    this.name = name;
  }

  async setChaserParams({ singleBrightness, multiBrightness, periodMs }) {
    const chaserMsg = Uint8Array.of(
      Math.floor(singleBrightness * 255.0),
      Math.floor(multiBrightness * 255.0),
      Math.floor(periodMs / 100.0)
    );
    await this.charChaser.writeValue(chaserMsg);
  }
}

const LedDriver = new _LedDriver();
export default LedDriver;
