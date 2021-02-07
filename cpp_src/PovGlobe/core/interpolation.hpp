#pragma once

#include <math.h>
#include <CImg.h>
#include <vector>


struct InterpolationFunction {
    virtual void operator()(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
    uint8_t& r, uint8_t& g, uint8_t& b) const = 0;

    virtual void operator()(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
    std::vector<uint8_t>& rgb) const {
        this->operator()(img, xy, rgb[0], rgb[1], rgb[2]);
    }
};

struct BilinearPixelInterpolaten: public InterpolationFunction {
    virtual void operator()(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
        uint8_t& r, uint8_t& g, uint8_t& b) const override {
        const unsigned int x0 = static_cast<unsigned int>(std::floor(xy.first));
        const unsigned int y0 = static_cast<unsigned int>(std::floor(xy.second));
        const unsigned int x1 = (x0 + 1) % img.width();
        const unsigned int y1 = (y0 + 1) % img.height();

        const float x = xy.first - x0;
        const float y = xy.second - y0;

        if (y0 < 0 || y0 >= img.height())
        {
            r = g = b = 0;
        }
        else {

            auto const bilinear_interp = [&img, x0, y0, x1, y1, x, y](int c) {
                return img(x0, y0, c) * (1.f - x) * (1.f - y) + img(x1, y0, c) * x * (1.f - y) +
                    img(x0, y1, c) * (1.f - x) * y + img(x1, y0 + 1, c) * x * y;
            };

            r = static_cast<uint8_t>(bilinear_interp(0));
            g = static_cast<uint8_t>(bilinear_interp(1));
            b = static_cast<uint8_t>(bilinear_interp(2));
        }
    }
};

struct NearestNeighbourPixelInterpolation : public InterpolationFunction {
    virtual void  operator()(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
        uint8_t& r, uint8_t& g, uint8_t& b) const override {
        const unsigned int x0 = static_cast<unsigned int>(std::round(xy.first));
        const unsigned int y0 = static_cast<unsigned int>(std::round(xy.second));

        if (y0 < 0 || y0 >= img.height())
        {
            r = g = b = 0;
        }
        else {
            r = img(x0 % img.width(), y0, 0);
            g = img(x0 % img.width(), y0, 1);
            b = img(x0 % img.width(), y0, 2);
        }

    }
};