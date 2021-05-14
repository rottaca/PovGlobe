import sys
import os
import time
import glob
from pathlib import Path

# build_config="x64-Debug (default)"
build_config = "x64-Release"
sys.path.append(
    os.path.join(
        os.path.dirname(__file__),
        "..",
        "out",
        "build",
        build_config,
        "cpp_src",
        "PyPovGlobe",
    )
)
sys.path.append(
    os.path.join(os.path.dirname(__file__), "..", "build", "cpp_src", "PyPovGlobe")
)
sys.path.append(
    os.path.join(os.path.dirname(__file__), "..", "build", "cpp_src", "PyPovGlobe", "Release")
)

import PyPovGlobe
import tile_server_api
import clock_app
import text_printer_app

num_leds_per_side = 55
radius = 13.25
spacing_top = 1.5
spacing_bottom = 2.0
double_sided = True
usw_hw = os.name != "nt"

projEquirect = PyPovGlobe.EquirectangularProjection()
projMercator = PyPovGlobe.MercatorProjection()
interpNearest = PyPovGlobe.NearestNeighbourPixelInterpolation()
interpBilinear = PyPovGlobe.BilinearPixelInterpolaten()

img_dir = Path(os.path.dirname(__file__)) / ".." / "res" / "img"
all_images = {
    os.path.basename(f): str(Path(f).resolve()) for f in glob.glob(str(img_dir) + "/*")
}

arg_projection = dict(
    type="options",
    name="Projection Type",
    desc="Different projection types to map plane images onto a globe.",
    options={"Equirectangular": projEquirect},  # "Mercator": projMercator
)

arg_interpolation = dict(
    type="options",
    name="Interpolation Type",
    desc="Interpolation type for down/up scaling images.",
    options={"Bilinear":interpBilinear, "Nearest Neighbour": interpNearest},
)

arg_images = dict(type="options", name="Image Source", desc="Available images. Add more images to the res/img/ folder.", options=all_images)

all_apps = [
    dict(
        name="TestApp",
        desc="Shows a simple color test pattern.",
        type=PyPovGlobe.ApplicationTest1,
        args=[],
    ),
    dict(
        name="TestApp2",
        desc="Shows a simple stripe pattern.",
        type=PyPovGlobe.ApplicationTest2,
        args=[],
    ),
    dict(
        name="ImageViewer",
        desc="Shows a static image.",
        type=PyPovGlobe.ApplicationImageViewer,
        args=[
            arg_images,
            arg_projection,
            arg_interpolation,
        ],
    ),
    dict(
        name="ImageRotator",
        desc="Rotates a given image with a constant speed.",
        type=PyPovGlobe.ApplicationImageRotator,
        args=[
            arg_images,
            arg_projection,
            arg_interpolation,
        ],
    ),
    dict(
        name="ClockApp",
        desc="A simple app which renders a digial clock.",
        type=clock_app.ClockApp,
        args=[],
    ),
    dict(
        name="TextPrinterApp",
        desc="A simple app which renders text.",
        type=text_printer_app.TextPrinterApp,
        args=[
            dict(
                type="str",
                name="Text",
                desc="Text to be printed"
            )
        ],
    ),
    dict(
        name="MapTileServer",
        desc="Loads map data from the provided tile server and correctly maps it onto the globe.",
        type=tile_server_api.TileServerApp,
        args=[
            dict(
                type="options",
                name="Tile Servers",
                desc="Default TileServers",
                options=tile_server_api.tile_map_urls,
            )
        ],
    ),
]


all_apps = {a["name"]: a for a in all_apps}

import socket
import fcntl
import struct


class GlobeWrapper:
    def __init__(self):
        if usw_hw:
            self.renderer = PyPovGlobe.RendererLedStripPico()
        else:
            self.renderer = PyPovGlobe.RendererSim()

        self.globe = PyPovGlobe.Globe(
            num_leds_per_side,
            radius,
            spacing_top,
            spacing_bottom,
            double_sided,
            self.renderer,
        )
        
        ip_address = self.get_ip_address("wlan0")

        self.running_app_args = []
        self.running_app = text_printer_app.TextPrinterApp(f"{ip_address}:5000", True)
        self.running_app_name = "TextPrinterApp"
        self.globe.runRendererAsync()

        self.globe.runApplicationAsync(self.running_app)

    def get_ip_address(self, ifname):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        return socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', bytes(ifname[:15], 'utf-8'))
        )[20:24])

    def get_all_apps(self):
        return all_apps

    def set_horizontal_offset(self, offset: int):
        self.globe.setHorizontalOffset(offset)

    def get_horizontal_offset(self) ->int:
        return self.globe.getHorizontalOffset()

    def app_by_name(self, name, args):
        if name not in all_apps:
            return False

        app = all_apps[name]
        app_cls = app["type"]

        self.running_app_args = args
        self.running_app_name = name
        new_app = app_cls(*self.running_app_args)

        self.globe.runApplicationAsync(new_app)
        self.running_app = new_app

        return True
