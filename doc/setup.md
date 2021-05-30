# Introduction
These setup instructions should enable you to build your own globe.

Please be adviced that signifficant knowledge about electronics is necessary in order to build this globe.
Also a good understanding of software development is necessary to implement new algorithms/visualizations.

Due to my limited time (:smile:), only basic documenation is available and you probably have to figure out some things on you own (mostly hardware related). If you worked on a similar project before, this should not be an issue for you!


## Part List
Several crutial parts are necessary to build this globe. Here is a list of the most important parts:
- Raspberry Pi 3 B+ or newer with standard os (e.g. ubuntu, no GUI required)
- DC motor, ~ 1200 RPM under load (results in up to 20 rotations per second, 20 FPS refresh rate)
  - I used this motor: www.amazon.de/gp/product/B0768DVMYN
  - I was not able to achieve these high rotation speeds because of vibrations and had to slow down the motor
  - Therefore I used an adjustable step-down converter to manually control the motor speed
- High density LED Strip with 144 LEDs/m
  - with APA102/SK9822 chips, **no WS28xx - their update rate is too slow!**
  - I used these: www.amazon.de/BTF-LIGHTING-ähnlich-Individuell-adressierbar-Nicht-wasserdichtes/dp/B07BPQV7FW
  - 
- Powersupply 24V (50-60W) depends on the number of LEDs and the motor
  - High voltage is necessary for power transfer with carbon brushes
  - I used something similar to this (mabye a bit overpowered): www.amazon.de/Docooler-100V-Spannungswandler-Schalter-Spg-Versorgungsteil/dp/B01LAVDD8S
- Carbon brushes to transfer electric power to the spinning globe 
  - I had some old ones laying around
  - The have to fit the 3d printed housing or you build your own
  - 5mm wide x 8mm high and 12mm long
  - Similar to this one www.amazon.de/vhbw-Kohle-Bürste-Motorkohle-Schleifkohle-Elektrowerkzeug/dp/B0799PMH36
- 8 Neodym Magnets (10 x 2 mm)
  -  www.amazon.de/gp/product/B06X977K8L
-  LM2596 DC-DC Buck Converter for converting the 24V into 5V for LEDs + RPi + Pico
   -  www.amazon.de/LAOMAO-Step-up-Converter-Raspberry-DIY-Projects-1-St%C3%BCck/dp/B00HV59922
- 3D printed parts 
  - Download it from thingiverse
  - www.thingiverse.com/thing:4871230
- Transparent showcase with solid base (used as a housing for the globe)
  - 300 mm square base, >= 320mm height
  - I build my own base (140mm high) to hide the motor
  - The case I used is no longer available. This one is similar but too small: www.amazon.de/HMF-Vitrine-Schaukasten-Modellautos-Transparent/dp/B08GQ5JS6B


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

A real test-mode needs to be implemented. Currenlty no stand-alone test software is available.

### Simulation
To make sure that the build was suceesfull, lets start with the simulation environment which renders the globe onto a window
on your monitor instead of using your hardware.

Executing the following command will launch two windows which show the globe surface as a 2D image and as a 3d sphere.
Running the command without arguments will use the compiled parameters, defined in main.cpp. They can be overwritten by 
the commandline arguments, listed below.
#### Linux
```bash
build/cpp_src/PovGlobe/Release/PovGlobe
```
#### Windows
```bash
.\build\cpp_src\PovGlobe\Release\PovGlobe.exe
```

You should see something like this:
```
--------------------- Globe Initialized --------------------
Globe Radius:                     13.25cm                                                                              
Num LEDs per side:                55                                                                                    
Num Pixels for half circle:       60                                                                                    
Num Pixels horizontal:            120                                                                                   
Leds are attached on both sides:  no                                                                                    
Spacing top    (cm / ratio in %): 1.5 cm (3.60351 %)                                                                    
Spacing bottom (cm / ratio in %): 2 cm (4.80468 %)                                                                      
No algo specified. Running default algo ApplicationTest1.                                                               
Initialize Renderer...                                                                                                  
Waiting for app to stop...                                                                                              
App stopped.                                                                                                            
Initialize Application...                                                                                               
[Render Thread]: Loop Time is avg 8.60606 ms, max 23 ms).                                                               
[Application Processing]: Loop Time is avg 9.88889 ms, max 50 ms).                                                      
[Render Thread]: Loop Time is avg 7.14141 ms, max 30 ms).                                                               
[Application Processing]: Loop Time is avg 19.9798 ms, max 50 ms).                                                      
[Render Thread]: Loop Time is avg 6.36364 ms, max 13 ms).                                                               
[Application Processing]: Loop Time is avg 30.1212 ms, max 52 ms).   
```

| Argument | Description |
|-----------|------------|
| -a \<ALGO> | Specify the name of the algorithm you want to execute. The list is hardcoded in main.cpp. If not specified, the default test-app is launched. | 
| -f \<PATH> | Optional argument for some apps to specify a file path (e.g. which image to load). | 
| -h \<HEIGHT> | Number of pixels/LEDs used for the vertical dimension | 
| -r \<RADIUS> | Radius of the globe in centimeters, used in combination with -t and -b. This is used to correctly visualize which part of the globe is covered with LEDs. If you don't care, leave it unchanged during simulation but make sure its correct when using the hardware in order to avoid image distortions. | 
| -t \<SPACING> | Spacing from the upper end of the veritcal axis to the first led in centimeters, used in combination with -r and -b. | 
| -b \<SPACING> | Spacing from the bottom end of the veritcal axis to the first led in centimeters, used in combination with -r and -t. | 
| -d | Your globe has LEDs on BOTH sides of the rotator. Only relevant for hardware rendering. | 
| -k | If specified, the globe is using the hardware instead of the simulation environment. | 
--------

For example you can simulate a high-resolution globe, visualizing a custom image by executing:
```bash
PovGlobe -h 512 -a ImageViewer -f my_img.png
```

### Hardware-Test
A first test is to run the Pico firmware by itself. Only connect the pico to the LED strip and the hall sensor and rotate the globe. On boot up, the Pico will draw a few simple visualizations and then waits for the Raspberry Pi to take over control.

If this test is successful, we can continue using the Raspberry Pi. Make sure you were able to succesfully build the software by following the steps above, labeled "Build Instructions".

To make things easy, you can start by executing the core framework itself (without the python wrapper and the webserver). Take a look at the instrictions in the "Simulation" section above.
By passing "-k" to the PovGlobe executable, the globe will use the available hardware to visualize the image. In order to access the hardware we need sudo rights:
#### Linux (no hw support on windows)
```bash
sudo build/cpp_src/PovGlobe/Release/PovGlobe -h <NUM_LEDS> -k
```

### Webserver
If that is successful, you can give the webserver a try. It is located in **\<ROOT>/py_src/**.
Open the file globe_wrapper.py and adjust the settings if needed (starting from line 20):
```python
num_leds_per_side = 55
radius = 13.25
spacing_top = 1.5
spacing_bottom = 2.0
double_sided = True
```

By default, the webserver will start with hardware support when it detects a linux operating system.
This can be overwritten by modifying this line:
```python
usw_hw = not running_windows
```
After you are done with configuration, we can start the webserver by executing:
```python
sudo -E python3 /home/pi/PovGlobe/py_src/app.py
```
You should see something like this:
```
--------------------- Globe Initialized --------------------
Globe Radius:                     13.25 cm
Num LEDs per side:                55
Num Pixels for half circle:       60
Num Pixels horizontal:            120
Leds are attached on both sides:  yes
Spacing top    (cm / ratio in %): 1.5 cm (3.60351 %)
Spacing bottom (cm / ratio in %): 2 cm (4.80468 %)
Initialize Renderer...
Waiting for app to stop...
App stopped.
Initialize Application...
 * Serving Flask app "app" (lazy loading)
 * Environment: production
   WARNING: This is a development server. Do not use it in a production deployment.
   Use a production WSGI server instead.
 * Debug mode: off
 * Running on http://0.0.0.0:5000/ (Press CTRL+C to quit)
[Render Thread]: Loop Time is avg 6.53535 ms, max 16 ms).
[Application Processing]: Loop Time is avg 9.74747 ms, max 46 ms).
[Render Thread]: Loop Time is avg 9.76768 ms, max 21 ms).
[Application Processing]: Loop Time is avg 20.0202 ms, max 49 ms).  
```
Check if you are able to open the webinterface by using a browser to navigate to *http://localhost:5000*. 
Replace "localhost" with the ip adress of your host if you open the browser on a different machine.

## Crontab
After building the source code, install a job in crontab to auto-start the app when the RPI boots up.
```bash
@reboot sleep 20 && sudo -E python3 /home/pi/PovGlobe/py_src/app.py > /home/pi/PovGlobe/log.txt
```