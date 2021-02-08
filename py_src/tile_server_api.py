import math
import requests
import io
import math
from PIL import Image

import PyPovGlobe

def deg2num(lat_deg, lon_deg, zoom):
  lat_rad = math.radians(lat_deg)
  n = 2.0 ** zoom
  xtile = int((lon_deg + 180.0) / 360.0 * n)
  ytile = int((1.0 - math.asinh(math.tan(lat_rad)) / math.pi) / 2.0 * n)
  return (xtile, ytile)


def num2deg(xtile, ytile, zoom):
  n = 2.0 ** zoom
  lon_deg = xtile / n * 360.0 - 180.0
  lat_rad = math.atan(math.sinh(math.pi * (1 - 2 * ytile / n)))
  lat_deg = math.degrees(lat_rad)
  return (lat_deg, lon_deg)

def getTileFromTileIdx(xtile, ytile, zoom):    
    url = f"http://a.tile.openstreetmap.org/{zoom}/{xtile}/{ytile}.png"
    print(url)
    response = requests.get(url)
    image_bytes = io.BytesIO(response.content)
    img = Image.open(image_bytes)
    return img

def getTileFromLatLon(lat_deg, lon_deg, zoom):
    xtile, ytile = deg2num(lat_deg, lon_deg, zoom)
    return getTileFromTileIdx(xtile, ytile, zoom)

def get_concat_h(im1, im2):
    dst = Image.new('RGB', (im1.width + im2.width, im1.height))
    dst.paste(im1, (0, 0))
    dst.paste(im2, (im1.width, 0))
    return dst

def get_concat_v(im1, im2):
    dst = Image.new('RGB', (im1.width, im1.height + im2.height))
    dst.paste(im1, (0, 0))
    dst.paste(im2, (0, im1.height))
    return dst

def get_world(zoom):
    x_tiles = list(range(2**zoom))
    y_tiles = list(range(2**zoom))

    img_full = None
    for x in x_tiles:
        img_col = None
        for y in y_tiles:
            img = getTileFromTileIdx(x,y, zoom)
            if img_col:
                img_col = get_concat_v(img_col, img)
            else:
                img_col = img
        if img_full:
            img_full = get_concat_h(img_full, img_col)
        else:
            img_full = img_col

    return img_full


class TileServerApp(PyPovGlobe.PyApplicationBase):
  def __init__(self):
    super().__init__()
    self.tile_img = None
    self.proj = PyPovGlobe.MercatorProjection() #PyPovGlobe.EquirectangularProjection() #
    self.interp = PyPovGlobe.NearestNeighbourPixelInterpolation()

  def pyInitialize(self, globe):
      zoom = 1
      self.tile_img = get_world(zoom=zoom)
      self.tile_img.show()

      # Whats the maximum lat of this globe
      max_lat = num2deg(0,0,zoom=zoom)[0] / 180.0 * math.pi
      min_lat = num2deg(2**zoom,2**zoom, zoom=zoom)[0] / 180.0 * math.pi
      print(min_lat, max_lat)
      self.m_xy_img_for_lonlat = PyPovGlobe.buildImageProjectionLUT(self.proj, globe, 
                                                                    self.tile_img.height, self.tile_img.width, 
                                                                    max_lat, min_lat)

  def pyProcess(self, framebuffer, time):
      for j in range(framebuffer.getWidth()):
        for i in range(framebuffer.getHeight()):
            x, y = self.m_xy_img_for_lonlat[j * framebuffer.getHeight() + i];
            xy = (int(x) % self.tile_img.width , 
                  int(y) % self.tile_img.height)

            framebuffer[j][i] = self.tile_img.getpixel(xy)


# https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Lon..2Flat._to_tile_numbers_2
        