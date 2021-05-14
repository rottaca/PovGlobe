
## Part List
- Raspberry Pi 3 B+ or newer (with Prememt RT kernel patch, documented below)
- DC motor, ~ 1200 RPM under load (results in 20 rotations per second, 20 FPS refresh rate)
- High density LED Strip with 144 LEDs/m
  - with APA102 chips, no WS28xx - their update rate is too slow!
  - e.g. Adafruit Dotstar
- Powersupply 24V (50-60W) Depends on the number of LEDs and the motor
- Carbon brushes for a slip ring to transfer electric power to the spinning globe
- 3D printed parts (from thingiverse **TODO**)
- Transparent showcase with solid base (used as a housing for the globe




# Software
- CMake (3.20.2): https://cmake.org/download/
- SWIG (4.0.2): http://www.swig.org/Doc1.3/Windows.html, https://sourceforge.net/projects/swig/files/swigwin/swigwin-4.0.2/swigwin-4.0.2.zip/download?use_mirror=altushost-swe
- ImageMagick6 (6.9.12) https://legacy.imagemagick.org/script/download.php
 - Windows: https://download.imagemagick.org/ImageMagick/download/binaries/ImageMagick-6.9.12-10-Q16-HDRI-x64-dll.exe 
 - Don't forget to check "Install development headers and libraries for c and c++" during installation

On linux:
- BCM

Pip: 
- Flask
- requests



Crontab:
@reboot sleep 20 && sudo -E python3 /home/pi/PovGlobe/py_src/app.py > /home/pi/PovGlobe/log.txt