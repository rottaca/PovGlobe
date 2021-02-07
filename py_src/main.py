import sys
import os
import time

#sys.path.append(os.path.join(os.path.dirname(__file__), "..", "out", "build", "x64-Debug (default)", "src", "wrapper_python"))
sys.path.append(os.path.join(os.path.dirname(__file__), "..", "out", "build", "x64-Release", "cpp_src", "PyPovGlobe"))
import PyPovGlobe
import tile_server_api

num_leds_per_side = 55
radius = 13.25
spacing_top = 1.5
spacing_bottom = 2.0
double_sided=True

rpm = PyPovGlobe.RpmMeasureSim()
renderer = PyPovGlobe.RendererSim(rpm)
globe = PyPovGlobe.Globe(num_leds_per_side, radius, spacing_top, spacing_bottom, double_sided, renderer)

proj = PyPovGlobe.EquirectangularProjection()
interp = PyPovGlobe.NearestNeighbourPixelInterpolation()

class MyApp(PyPovGlobe.ApplicationBase):
  def __init__(self):
    PyPovGlobe.ApplicationBase.__init__(self)
    self.tile_img = None
    self.proj = PyPovGlobe.EquirectangularProjection()
    self.interp = PyPovGlobe.NearestNeighbourPixelInterpolation()

  def initialize(self, globe):
      self.tile_img = PyPovGlobe.tile_server_api.get_world(zoom=1)
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


res_dir = os.path.join(os.path.dirname(__file__), "..", "res")

all_apps = [
    PyPovGlobe.ApplicationImageRotator(os.path.join(res_dir, "img", "2_no_clouds_8k.jpg"), proj, interp),
    PyPovGlobe.ApplicationImageRotator(os.path.join(res_dir, "img", "kisspng-world-map-globe-equirectangular-projection-map.png"), proj, interp),
    PyPovGlobe.ApplicationImageViewer(os.path.join(res_dir, "img", "soccer2_sph.png"), proj, interp),
    PyPovGlobe.ApplicationImageViewer(os.path.join(res_dir, "img", "tennisenn_sph.png"), proj, interp),
    PyPovGlobe.ApplicationImageRotator(os.path.join(res_dir, "img", "1_earth_8k.jpg"), proj, interp),
    #MyApp()
]

globe.runRendererAsync()

app_idx = 0
while(True):
    globe.stopCurrentApp()
    globe.runApplicationAsync(all_apps[app_idx])

    time.sleep(3)

    app_idx+=1
    if app_idx >= len(all_apps):
        app_idx = 0

globe.shutdown()
