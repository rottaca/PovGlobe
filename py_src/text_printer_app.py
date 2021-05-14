import math
import requests
import io
import math
from PIL import Image, ImageDraw
from datetime import datetime

import PyPovGlobe

class TextPrinterApp(PyPovGlobe.PyApplicationBase):
  def __init__(self, text: str, rotating:bool=False):
    super().__init__()
    self.text = text
    self.offset = 0
    self.rotating = rotating

  def pyInitialize(self, globe):
      self.img = Image.new("RGB", (globe.getHorizontalNumPixels(), 
                                   globe.getVerticalNumPixelsWithLeds()),
                          color=0)
      self.draw = ImageDraw.Draw(self.img)
      self.font = self.draw.getfont()
      return True


  def pyProcess(self, framebuffer, time):
      self.draw.rectangle([(0,0), (framebuffer.getWidth(), framebuffer.getHeight())], fill=0)
      w, h = self.draw.textsize(self.text)

      self.draw.text(((framebuffer.getWidth()-w)/2,
                      (framebuffer.getHeight()-h)/2),
                      self.text)

      for j in range(framebuffer.getWidth()):
        for i in range(framebuffer.getHeight()):
            framebuffer[(j+self.offset)% framebuffer.getWidth()][i] = self.img.getpixel((j,i))

      if self.rotating:
        self.offset = (self.offset - 2)
        if self.offset < 0:
            self.offset = framebuffer.getWidth() -1


# https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Lon..2Flat._to_tile_numbers_2
        