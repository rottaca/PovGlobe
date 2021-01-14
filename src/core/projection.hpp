#pragma once

#include <vector>
#include <math.h>

#include "helper.hpp"


class ProjectionFunction {
public:
    ProjectionFunction() = default;
    virtual float minIn1() const {
        return m_minIn1;
    }
    virtual float maxIn1() const {
        return m_maxIn1;
    }
    virtual float minIn2() const {
        return m_minIn2;
    }
    virtual float maxIn2() const {
        return  m_maxIn2;
    }

    virtual void operator()(float in1, float in2, float& out1, float& out2) const = 0;

protected:
    ProjectionFunction(float minIn1, float maxIn1, float minIn2, float maxIn2)
        : m_minIn1(minIn1)
        , m_maxIn1(maxIn1)
        , m_minIn2(minIn2)
        , m_maxIn2(maxIn2) {}

    float m_minIn1;
    float m_maxIn1;
    float m_minIn2;
    float m_maxIn2;
};


class MercatorProjection : public ProjectionFunction {
public:
    MercatorProjection() : ProjectionFunction(
        -180 * deg2rad,
        +180 * deg2rad,
        -90  * deg2rad,
        +90  * deg2rad
    ) {}

    // https://en.wikipedia.org/wiki/Miller_cylindrical_projection
    // lat = 0 at equator
    virtual void operator()(float lon, float lat, float& x, float& y) const override {
        x = lon;
        y = log(tan(lat / 2 + pi / 4.f));
    }
};
class InvMercatorProjection : public ProjectionFunction {
public:
    InvMercatorProjection() : ProjectionFunction(
        -90 * deg2rad,
        +90 * deg2rad,
        -180 * deg2rad,
        +180 * deg2rad
    ) {}
    virtual void operator()(float x, float y, float& lon, float& lat) const override {
        lon = x;
        lat = atan(exp(y) * 2 - pi / 2.f);
    }
};


class EquirectangularProjection : public ProjectionFunction {
public:
    EquirectangularProjection() : ProjectionFunction(
        -180 * deg2rad,
        +180 * deg2rad,
        -90 * deg2rad,
        +90 * deg2rad
    ) {}
    virtual void operator()(float lon, float lat, float& x, float& y) const override {
        x = lon;
        y = lat;
    }
};
class InvEquirectangularProjection : public ProjectionFunction {
public:
    InvEquirectangularProjection() : ProjectionFunction(
        -90 * deg2rad,
        +90 * deg2rad,
        -180 * deg2rad,
        +180 * deg2rad
    ) {}
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

    // TODO 
    const float xMin = projection.minIn1();
    const float xMax = projection.maxIn1();
    const float yMin = projection.minIn2();
    const float yMax = projection.maxIn2();

    CoordinateList lonlat;
    for (size_t i = 0; i < globe_height; i++)
    {
        const float ratio_y = (i + img_height_offset - half_height) / half_img_height;
        const float lat = ratio_y * pi / 2.0f;
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
