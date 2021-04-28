import sys
import os
import time

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


class GlobeWrapper:
    num_leds_per_side = 55
    radius = 13.25
    spacing_top = 1.5
    spacing_bottom = 2.0
    double_sided = True
    usw_hw = True

    def __init__(self):
        if self.usw_hw:
            self.renderer = PyPovGlobe.RendererLedStripPico()
        else:
            self.renderer = PyPovGlobe.RendererSim()

        self.globe = PyPovGlobe.Globe(
            self.num_leds_per_side,
            self.radius,
            self.spacing_top,
            self.spacing_bottom,
            self.double_sided,
            self.renderer,
        )

        self.proj = PyPovGlobe.EquirectangularProjection()
        self.interp = PyPovGlobe.NearestNeighbourPixelInterpolation()

        self.res_dir = os.path.join(os.path.dirname(__file__), "..", "res")

        self.all_apps = {
            "ImageRotator_EarthNoClouds": PyPovGlobe.ApplicationImageRotator(
                os.path.join(self.res_dir, "img", "2_no_clouds_8k.jpg"),
                self.proj,
                self.interp,
            ),
            "ImageRotator_Countries": PyPovGlobe.ApplicationImageRotator(
                os.path.join(
                    self.res_dir,
                    "img",
                    "kisspng-world-map-globe-equirectangular-projection-map.png",
                ),
                self.proj,
                self.interp,
            ),
            "ImageRotator_EarthClouds": PyPovGlobe.ApplicationImageRotator(
                os.path.join(self.res_dir, "img", "1_earth_8k.jpg"),
                self.proj,
                self.interp,
            ),
            "ImageViewer_Soccer": PyPovGlobe.ApplicationImageViewer(
                os.path.join(self.res_dir, "img", "soccer2_sph.png"),
                self.proj,
                self.interp,
            ),
            "ImageViewer_Tennis": PyPovGlobe.ApplicationImageViewer(
                os.path.join(self.res_dir, "img", "tennisenn_sph.png"),
                self.proj,
                self.interp,
            ),
            "ClockApp": ClockApp.ClockApp(),
        }

        self.globe.runRendererAsync()
        self.app_idx = 0

    def get_all_app_names(self):
        return tuple(self.all_apps.keys())

    def next_app(self):
        key, val = list(self.all_apps.items())[self.app_idx]
        self.globe.runApplicationAsync(val)
        self.app_idx += 1
        if self.app_idx >= len(self.all_apps):
            self.app_idx = 0

        return key

    def app_by_name(self, name):
        if name not in self.all_apps:
            return False

        self.globe.runApplicationAsync(self.all_apps[name])

        return True
