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
Now, we have to flash the rpi pico firmware in order to get the globe running. This can be done by this script: `pico_src/flash.py`. Run it like this.

```bash
sudo py -3 pico_ src/flash.py <path-to-uf2-file-in-build>
```
This should mount the pico, upload the firmware and unmount the pico again. After that, the firmware should be flashed.


## First Test
TBD

## Crontab
After building the source code, install a job in crontab to auto-start the app when the RPI boots up.
```bash
@reboot sleep 20 && sudo -E python3 /home/pi/PovGlobe/py_src/app.py > /home/pi/PovGlobe/log.txt
```