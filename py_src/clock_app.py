import math
import requests
import io
import math
from PIL import Image, ImageDraw
from datetime import datetime

import PyPovGlobe

class ClockApp(PyPovGlobe.PyApplicationBase):
  def __init__(self):
    super().__init__()


  def pyInitialize(self, globe):
      self.img = Image.new("RGB", (globe.getHorizontalNumPixels(), 
                                   globe.getVerticalNumPixelsWithLeds()),
                          color=0)
      self.draw = ImageDraw.Draw(self.img)
      self.font = self.draw.getfont()

      return True


  def pyProcess(self, framebuffer, time):
      now = datetime.now()
      text = now.strftime("%H:%M:%S")

      self.draw.rectangle([(0,0), (framebuffer.getWidth(), framebuffer.getHeight())], fill=0)
      w, h = self.draw.textsize(text)
      self.draw.text(((framebuffer.getWidth()-w)/2,
                      (framebuffer.getHeight()-h)/2),
                      text)

      for j in range(framebuffer.getWidth()):
        for i in range(framebuffer.getHeight()):
            framebuffer[j][i] = self.img.getpixel((j,i))


# https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Lon..2Flat._to_tile_numbers_2
        