#pragma once

#include <math.h>
#include <CImg.h>


struct InterpolationFunction {
    virtual void operator()(const cimg_library::CImg<unsigned char>&, const Coordinate&,
    uint8_t& r, uint8_t& g, uint8_t& b)const = 0;
};

struct BilinearPixelInterpolaten:public InterpolationFunction {
    virtual void operator()(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
        uint8_t& r, uint8_t& g, uint8_t& b) const override {
        const unsigned int x0 = static_cast<unsigned int>(std::floor(xy.first));
        const unsigned int y0 = static_cast<unsigned int>(std::floor(xy.second));

        const float x = xy.first - x0;
        const float y = xy.second - y0;

        auto const bilinear_interp = [&img, x0, y0, x, y](int c) {
            return img(x0, y0, c) * (1.f - x) * (1.f - y) + img(x0 + 1, y0, c) * x * (1.f - y) +
                img(x0, y0 + 1, c) * (1.f - x) * y + img(x0 + 1, y0 + 1, c) * x * y;
        };

        r = static_cast<uint8_t>(bilinear_interp(0));
        g = static_cast<uint8_t>(bilinear_interp(1));
        b = static_cast<uint8_t>(bilinear_interp(2));
    }
};

struct NearestNeighbourPixelInterpolation :public InterpolationFunction {
    virtual void  operator()(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
        uint8_t& r, uint8_t& g, uint8_t& b) const override {
        const unsigned int x0 = static_cast<unsigned int>(std::round(xy.first));
        const unsigned int y0 = static_cast<unsigned int>(std::round(xy.second));

        r = img(x0, y0, 0);
        g = img(x0, y0, 1);
        b = img(x0, y0, 2);
    }
};