SDK_HOME = /home/pi/source/github/esp-open-sdk/sdk
CC = $(SDK_HOME)/../xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
LD = $(CC)
ESPTOOL = esptool.py

LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -ldriver -lmbedtls -lssl -lcrypto -Wl,--end-group -lgcc

CPPFLAGS = 
CFLAGS = -I. -mlongcalls -I$(SDK_HOME)/include -I$(SDK_HOME)/driver_lib/driver -I$(SDK_HOME)/driver_lib/include/driver
LDFLAGS = -T$(SDK_HOME)/ld/eagle.app.v6.ld -L$(SDK_HOME)/lib

EXE = main
ELF = $(EXE).elf
BIN_PREFIX = $(EXE)-
BIN0 = $(BIN_PREFIX)0x00000.bin
BIN1 = $(BIN_PREFIX)0x10000.bin

SOURCES = main.c clock.c clockio_esp8266.c connection_esp8266.c delay_esp8266.c display.c display_esp8266.c temperature_sensor.c temperature_sensor_esp8266.c
OBJECTS = $(SOURCES:%.c=%.o)

BAUD = 921600

$(BIN0): $(ELF)
	$(ESPTOOL) elf2image --output $(BIN_PREFIX) $^

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

flash: $(BIN0)
	$(ESPTOOL) -b $(BAUD) write_flash 0 $(BIN0) 0x10000 $(BIN1)

.PHONY clean: $(SOURCES)
	-rm *.o $(ELF) $(BIN0) $(BIN1)
