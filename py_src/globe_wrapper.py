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

import PyPovGlobe
import tile_server_api
import ClockApp

num_leds_per_side = 55
radius = 13.25
spacing_top = 1.5
spacing_bottom = 2.0
double_sided = True
usw_hw = os.name != "nt"

proj = PyPovGlobe.EquirectangularProjection()
interp = PyPovGlobe.NearestNeighbourPixelInterpolation()
img_dir = Path(os.path.dirname(__file__)) / ".." / "res" / "img"

all_images = {
    os.path.basename(f): str(Path(f).resolve()) for f in glob.glob(str(img_dir) + "/*")
}

arg_projection = dict(
    type="proj",
    name="Projection Type",
    desc="",
    options={"EquirectangularProjection": proj},
)

arg_interpolation = dict(
    type="interp",
    name="Interpolation Type",
    desc="",
    options={"NearestNeighbourPixelInterpolation": interp},
)

arg_images = dict(type="img_path", name="Image Path", desc="", options=all_images)

all_apps = [
    dict(
        name="ImageViewer",
        type=PyPovGlobe.ApplicationImageViewer,
        args=[
            arg_images,
            arg_projection,
            arg_interpolation,
        ],
    ),
    dict(
        name="ImageRotator",
        type=PyPovGlobe.ApplicationImageRotator,
        args=[
            arg_images,
            arg_projection,
            arg_interpolation,
        ],
    ),
    dict(
        name="ClockApp",
        type=ClockApp.ClockApp,
        args=[],
    ),
]


all_apps = {a["name"]: a for a in all_apps}


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
        self.running_app_args = None
        self.running_app = None
        self.running_app_name = None
        self.globe.runRendererAsync()

    def get_all_apps(self):
        return all_apps

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
