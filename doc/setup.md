# Introduction
These setup instructions should enable you to build your own globe


## Part List
- Raspberry Pi 3 B+ or newer (with Prememt RT kernel patch, documented below)
- DC motor, ~ 1200 RPM under load (results in up to 20 rotations per second, 20 FPS refresh rate)
- High density LED Strip with 144 LEDs/m
  - with APA102 chips, no WS28xx - their update rate is too slow!
  - e.g. Adafruit Dotstar
- Powersupply 24V (50-60W) depends on the number of LEDs and the motor
- Carbon brushes to transfer electric power to the spinning globe
- 3D printed parts (from thingiverse **TODO**)
- Transparent showcase with solid base (used as a housing for the globe)


# Software Setup
In order to build the software, provided in this repository, you need several software packages:

On Linux (Raspberry Pi):
- CMake (3.20.2): https://cmake.org/download/
```bash
sudo apt-get install cmake g++-4.8
```

- SWIG (4.0.2): http://swig.org/Doc4.0/SWIGDocumentation.html#Preface_unix_installation

- ImageMagick6 (6.9.12) https://legacy.imagemagick.org/script/download.php
```bash
sudo apt-get install libmagick++-dev
```

- bcm2835 library for raspberry pi: https://raspberry-projects.com/pi/programming-in-c/io-pins/bcm2835-by-mike-mccauley
  
- In order to use the python webserve, you have to install a few pip packages for python3.
```bash
py -3 pip -m install flask requests
```

On Windows:
- CMake (3.20.2): https://cmake.org/download/
- SWIG (4.0.2): http://www.swig.org/Doc1.3/Windows.html, https://sourceforge.net/projects/swig/files/swigwin/swigwin-4.0.2/swigwin-4.0.2.zip/download?use_mirror=altushost-swe
- ImageMagick6 (6.9.12) https://legacy.imagemagick.org/script/download.php
  - Windows: https://download.imagemagick.org/ImageMagick/download/binaries/ImageMagick-6.9.12-10-Q16-HDRI-x64-dll.exe 
  - Don't forget to check "Install development headers and libraries for c and c++" during installation
- In order to use the python webserve, you have to install a few pip packages for python3.
```bash
py -3 pip -m install flask requests
```

## Configure Globe
The software has a set of configurable parameters in order to work with arbitrary globes (e.g. different number of LEDs). These settings have to be configured in two files (one for the rpi software and a second time for the pico).
- Raspberry Pi Software
  - The framework has two entrypoints. One for the c++ implementation and one for the python wrapper.
  - ~~For the c++ implementation, the settings are set in `cpp_src\PovGlobe\main.cpp` lines 24-27.~~
  - For the python implementation, which includes the webserver, the same settings are hard-coded in `py_src\globe_wrapper.py` line 21-25.
  - Adjust the settings according to you needs. Here is a short description
    - leds = 55 -> 55 Pixels per side
    - radius = 13.25 -> Radius of the globe in centimeters
    - spacing_top = 1.5 -> How much is the last LED at the top away from the rotation axis. 1.5 cm by default.
    - spacing_bottom = 2.0 -> How much is the last LED at the bottom away from the rotation axis. 2 cm by default.
- The Pico Firmware
  - For the pico, the settings can be configured here: `pico_src\Renderer\constants.hpp` 
  - It has a lot more properties but most of them should stay unchanged unless you know what you are doing
  - Important parameters are:
    - N_VERTICAL_RESOLUTION -> Number of leds on one half of the globe (55 by default).
    - N_HORIZONTAL_RESOLUTION -> Number of "pixels" along the horizontal axis. This has to match the configuration of the rpi. The rpi software will determine this value, based on the given phisical dimensions of the globe. When running the rpi software, a summary of these calculated parameters will be printed. This can be used to configure the pico correctly. Picking the wrong value will not damage your pi or pico, but both components will detect the missmatch and no image gets displayed.
    - N_MAGNETS: Number of magnets, used for motion detection (8 by default and changing this might require adapting the code too)
    - The other settings include the pin layout of the pico which is used to communiate with the Rpi and to control its sensors and LEDs.

## Build Instructions
### Windows (powershell)
On windows, we can only build the simulation environment in order to quickly prototype and implement new apps.
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux (Bash)
On linux, we can build the simulation environment, the hardware environment and the pico firmware. All will be build by the following command if all necessary dependencies are installed.
```bash
mkdir build
cd build
cmake ..
make -j4
```
Now, we have to flash the rpi pico firmware in order to get the globe running. This can be done by this script: `pico_src/flash.py`. Run it like this:

```bash
sudo py -3 pico_ src/flash.py <path-to-uf2-file-in-build>
```
The script will mount the pico, upload the firmware and unmount the pico again. After that, the firmware should be flashed.


## First Test
TBD

## Crontab
After building the source code, install a job in crontab to auto-start the app when the RPI boots up.
```bash
@reboot sleep 20 && sudo -E python3 /home/pi/PovGlobe/py_src/app.py > /home/pi/PovGlobe/log.txt
```