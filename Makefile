build:
	pio run

upload:
	pio run -t upload --upload-port=/dev/ttyS4

init:
	pio platform install https://github.com/platformio/platform-espressif32.git#feature/stage
	pio run
