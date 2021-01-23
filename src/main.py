import sys
import os
#sys.path.append(os.path.join(os.path.dirname(__file__), "..", "out", "build", "x64-Debug (default)", "src", "wrapper_python"))
sys.path.append(os.path.join(os.path.dirname(__file__), "..", "out", "build", "x64-Release", "src", "wrapper_python"))

import time
import PovGlobe

width = 110
height = 55
radius = 13.25
spacing_top = 1.0
spacing_bottom = 2.0
double_sided=True

rpm = PovGlobe.RpmMeasureSim()
renderer = PovGlobe.RendererSim(rpm)

globe = PovGlobe.Globe(height, width, radius, spacing_top, spacing_bottom, double_sided, renderer)

#app = PovGlobe.ApplicationTest1()

proj = PovGlobe.EquirectangularProjection()
interp = PovGlobe.NearestNeighbourPixelInterpolation()
#app = PovGlobe.ApplicationImageRotator(r"D:\Benutzer\Andreas\GitProjects\PovGlobe\res\img\1_earth_8k.jpg",
#                                       proj, interp)

class MyApp(PovGlobe.ApplicationBase):
  def __init__(self):
    PovGlobe.ApplicationBase.__init__(self)
    self.tile_img = None
    self.proj = PovGlobe.EquirectangularProjection()
    self.interp = PovGlobe.NearestNeighbourPixelInterpolation()

  def initialize(self, globe):
      self.tile_img = PovGlobe.tile_server_api.get_world(zoom=1)
      print(self.tile_img)

      top_pixel_skip_exact = globe.getSpacingTop() / globe.getHalfCircumference() * globe.getHeight()
      top_pixel_skip = int(round(top_pixel_skip_exact))
      bottom_pixel_skip_exact = globe.getSpacingBottom() / globe.getHalfCircumference() * globe.getHeight()
      bottom_pixel_skip = int(round(bottom_pixel_skip_exact))
      
      self.m_xy_img_for_lonlat = buildImageProjectionLUT(self.proj,
                                self.tile_img.height(), self.tile_img.width(), top_pixel_skip,
                                globe.getHeight(), globe.getWidth())

      #self.tile_img.show()
      #self.tile_img.resize(globe.getWidth(), globe.getHeight() + m_top_pixel_skip + m_bottom_pixel_skip, 1, 3);


  def process(self, framebuffer, time):
      for i in range(framebuffer.getHeight()):
        for j in range(framebuffer.getWidth()):
            #xy = m_xy_img_for_lonlat[i * framebuffer.getWidth() + j];

            framebuffer[i][j] = self.tile_img.getpixel((i*3, j*3))



app = MyApp()


globe.runRendererAsync()
globe.runApplicationAsync(app)


time.sleep(100)
globe.shutdown()
