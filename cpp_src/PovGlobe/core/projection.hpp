#pragma once

#include <vector>
#include <math.h>

#include "helper.hpp"
#include "globe.hpp"


class ProjectionFunction {
public:
    virtual void operator()(float in1, float in2, float& out1, float& out2) const = 0;
};


class MercatorProjection : public ProjectionFunction {
public:
    // https://en.wikipedia.org/wiki/Miller_cylindrical_projection
    // lat = 0 at equator
    virtual void operator()(float lon, float lat, float& x, float& y) const override {
        x = lon;
        y = log(tan(lat / 2 + pi / 4.f));
    }
};
class InvMercatorProjection : public ProjectionFunction {
public:
    virtual void operator()(float x, float y, float& lon, float& lat) const override {
        lon = x;
        lat = atan(exp(y) * 2 - pi / 2.f);
    }
};


class EquirectangularProjection : public ProjectionFunction {
public:
    virtual void operator()(float lon, float lat, float& x, float& y) const override {
        x = lon;
        y = lat;
    }
};
class InvEquirectangularProjection : public ProjectionFunction {
public:
    virtual void operator()(float x, float y, float& lon, float& lat) const override {
        lon = x;
        lat = y;
    }
};

using Coordinate = std::pair<float, float>;
using CoordinateList = std::vector<Coordinate>;

inline std::vector<std::pair<float, float>> project(const CoordinateList& coordinates,
    const ProjectionFunction& projection) {
    CoordinateList res(coordinates.size());

    for (size_t i = 0; i < coordinates.size(); i++)
    {
        projection(coordinates[i].first, coordinates[i].second, res[i].first, res[i].second);
    }
    return res;
}

inline CoordinateList buildImageProjectionLUT(const ProjectionFunction& projection, const Globe& globe,
                                       int img_height, int img_width, float max_img_lat, float min_img_lat) {

    const int globe_height_total = globe.getTotalVerticalNumPixels();
    const int globe_height_with_leds = globe.getVerticalNumPixelsWithLeds();
    const int globe_width = globe.getHorizontalNumPixels();
    const int globe_width_half = globe_width / 2;

    const float max_img_lon = pi;
    const float min_img_lon = -max_img_lon;

    const float max_globe_lon = pi;
    const float min_globe_lon = -max_globe_lon;
    const float max_globe_lat_leds = pi / 2.f * (1.0f - (globe.getSpacingTopRatio() * 2.0f));
    const float min_globe_lat_leds = -pi / 2.f * (1.0f - (globe.getSpacingBottomRatio() * 2.0f));

    // Calculate lat/lon gps locations for each pixel of the globe
    // Take into account the limits of the globe (where are leds located along the vertical axis
    CoordinateList lonlat;
    for (size_t x = 0; x < globe_width; x++)
    {
        const float ratio_x = x / (globe_width - 1.f);
        const float lon = ratio_x * (max_globe_lon - min_globe_lon) + min_globe_lon;

        for (size_t y = 0; y < globe_height_with_leds; y++)
        {
            const float ratio_y = y / (globe_height_with_leds - 1.f);
            const float lat = ratio_y * (max_globe_lat_leds - min_globe_lat_leds) + min_globe_lat_leds;
            lonlat.push_back(std::make_pair(lon, lat));
        }
    }

    assert(lonlat.size() == globe_width * globe_height_with_leds);
    
    // Apply given projection to lat/lon pairs
    auto projectedCoordinates = project(lonlat, projection);

    // Project image limits to get max/min pixel positions for scaling
    float max_img_proj_lat, min_img_proj_lat;
    float max_img_proj_lon, min_img_proj_lon;
    projection(min_img_lon, min_img_lat, min_img_proj_lon, min_img_proj_lat);
    projection(max_img_lon, max_img_lat, max_img_proj_lon, max_img_proj_lat);

    // rescale projected coordinates to image plane
    for (auto& p : projectedCoordinates) {
        const float ratio_x = (p.first - min_img_proj_lon) / (max_img_proj_lon - min_img_proj_lon);
        const float ratio_y = (p.second - min_img_proj_lat) / (max_img_proj_lat - min_img_proj_lat);
        p.first = ratio_x * img_width;
        p.second = ratio_y * img_height;
    }

    return projectedCoordinates;
}
