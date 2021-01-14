import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "..", "out", "build", "x64-Release", "src", "wrapper_python"))

import PovGlobe
import time

width = 120
height = 55
radius = 13.25
spacing_top = 1.0
spacing_bottom = 2.0
double_sided=True

rpm = PovGlobe.RpmMeasureSim()
renderer = PovGlobe.RendererSim(rpm)

globe = PovGlobe.Globe(height, width, radius, spacing_top, spacing_bottom, double_sided, renderer)

#app = PovGlobe.ApplicationTest1()

#proj = PovGlobe.EquirectangularProjection()
#interp = PovGlobe.NearestNeighbourPixelInterpolation()
#app = PovGlobe.ApplicationImageRotator(r"D:\Benutzer\Andreas\GitProjects\PovGlobe\res\img\1_earth_8k.jpg",
#                                       proj, interp)

class MyApp(PovGlobe.ApplicationBase):
  def __init__(self):
    PovGlobe.ApplicationBase.__init__(self)

  def initialize(self, globe):
      pass

  def process(self, framebuffer, time):
      pass

app = MyApp()


globe.runRendererAsync()
globe.runApplicationAsync(app)


time.sleep(10)
globe.shutdown()
