#pragma once

#include <vector>
#include <math.h>

#include "helper.hpp"


struct ProjectionFunction {
    virtual void operator()(float in1, float in2, float& out1, float& out2) const = 0;
};


struct MercatorProjection : public ProjectionFunction {
    // https://en.wikipedia.org/wiki/Miller_cylindrical_projection
    // lat = 0 at equator
    virtual void operator()(float lon, float lat, float& x, float& y) const override {
        x = lon;
        y = log(tan(lat / 2 + pi / 4.f));
    }
};
struct InvMercatorProjection : public ProjectionFunction {
    virtual void operator()(float x, float y, float& lon, float& lat) const override {
        lon = x;
        lat = atan(exp(y) * 2 - pi / 2.f);
    }
};


struct EquirectangularProjection : public ProjectionFunction {
    virtual void operator()(float lon, float lat, float& x, float& y) const override {
        x = lon;
        y = lat;
    }
};
struct InvEquirectangularProjection : public ProjectionFunction {
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

inline CoordinateList buildImageProjectionLUT(const ProjectionFunction& projection,
                                              int img_height, int img_width, int img_height_offset,
                                              int globe_height, int globe_width) {

    const float half_img_height = img_height / 2.0f;
    const float half_img_width = img_width / 2.0f;
    const float half_height = globe_height / 2.0f;
    const float half_width = globe_width / 2.0f;

    CoordinateList lonlat;
    for (size_t i = 0; i < globe_height; i++)
    {
        const float lat = (i + img_height_offset - half_height) / half_img_height * pi / 2.0f;
        for (size_t j = 0; j < globe_width; j++)
        {
            const float lon = (j - half_width) / half_width * pi;
            lonlat.push_back(std::make_pair(lon, lat));
        }
    }

    auto projectedCoordinates = project(lonlat, projection);

    float max_x;
    float max_y;
    projection(pi,
        (half_height) / half_img_height * pi / 2.0f,
        max_x, max_y);

    // rescale
    for (auto& p : projectedCoordinates) {
        p.first = p.first / max_x * half_img_width + half_img_width;
        p.second = p.second / max_y * half_img_height + half_img_height;
    }

    return projectedCoordinates;
}
