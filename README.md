# plantmonitor-sensor
Raspberry Pi Pico based LoRaWAN soil moisture sensor for plantmonitor project


## Setting up the project 

Clone with Submodules:

    git clone -b master --recurse-submodules -j8 https://github.com/ThomasLeister/plantmonitor-sensor.git


### Configure

    cp config.example.h config.h

(configure secrets and region in `config.h`)


### Build

Run Cmake init:

    cmake ./

Run `make`:

    make


## Flashing

(TBD)


## Caveats

Settings from config.h and context will be written to Flash and loaded from Flash at next reboot. This leads to the effect that changes in your `config.h` will not be accepted unless you clear the internal Flash / EEPROM. 

The EEPROM section for settings can be erased by commenting out the lines

    NvmDataMgmtFactoryReset();
    EepromMcuFlush();

and starting up the Raspberry Pi Pico once.  After that, the lines can be commented out, again. 
