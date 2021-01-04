#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <math.h>
#include <mutex>
#include <assert.h>
#include <chrono>

#include <CImg.h>

constexpr double pi = 3.14159265358979323846;

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void initialize(uint32_t height, uint32_t width, uint32_t channels = 3);

    inline uint8_t& operator() (uint32_t row, uint32_t col, uint32_t channel);
    inline const uint8_t& operator() (uint32_t row, uint32_t col, uint32_t channel) const;

    inline const uint8_t& operator[](uint32_t idx) const;
    inline uint8_t& operator[](uint32_t idx);

    inline std::mutex& getMutex() const;
    inline const uint8_t* values() const;
    inline uint8_t* values();

    inline uint32_t getSize() const;
    inline uint32_t getWidth() const;
    inline uint32_t getHeight() const;
    inline uint32_t getChannels() const;

private:
    std::vector<uint8_t> m_values;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_channels;

    mutable std::mutex m_accessMutex;
};


class LoopTimer
{
public:
    LoopTimer(std::string tag);
    ~LoopTimer();

    std::chrono::duration<float, std::milli> loopDone();
    inline float getAvgDuration();
    inline void resetLoopStartTime();

private:
    std::chrono::time_point<std::chrono::steady_clock> m_last_time;
    std::chrono::time_point<std::chrono::steady_clock> m_last_time_print;
    float m_avgLoopTime;
    std::string m_tag;
};

float LoopTimer::getAvgDuration()
{
    return m_avgLoopTime;
}

void LoopTimer::resetLoopStartTime()
{
    m_last_time = std::chrono::steady_clock::now();
}

uint8_t& Framebuffer::operator()(uint32_t col, uint32_t row, uint32_t channel)
{
    assert(row < m_height);
    assert(col < m_width);
    assert(channel < m_channels);
    const uint32_t idx = col * (m_height * m_channels) + row * m_channels + channel;
    return m_values[idx];
}

const uint8_t& Framebuffer::operator()(uint32_t col, uint32_t row, uint32_t channel) const
{
    assert(row < m_height);
    assert(col < m_width);
    assert(channel < m_channels);
    const uint32_t idx = col * (m_height * m_channels) + row * m_channels + channel;
    return m_values[idx];
}

const uint8_t& Framebuffer::operator[](uint32_t idx) const {
    assert(idx < m_values.size());
    return m_values[idx];
}
uint8_t& Framebuffer::operator[](uint32_t idx) {
    assert(idx < m_values.size());
    return m_values[idx];
}

std::mutex& Framebuffer::getMutex() const {
    return m_accessMutex;
}

const uint8_t* Framebuffer::values() const
{
    return m_values.data();
}

uint8_t* Framebuffer::values()
{
    return m_values.data();
}

uint32_t Framebuffer::getWidth() const
{
    return m_width;
}

uint32_t Framebuffer::getSize() const
{
    return m_values.size();
}


uint32_t Framebuffer::getHeight() const
{
    return m_height;
}


uint32_t Framebuffer::getChannels() const
{
    return m_channels;
}


// https://en.wikipedia.org/wiki/Miller_cylindrical_projection
// lat = 0 at equator
inline void mercatorProjection(float lon, float lat, float& x, float& y) {
    x = lon;
    y = log(tan(lat / 2 + pi / 4.f));
}

inline void invMercatorProjection(float x, float y, float& lon, float& lat) {
    lon = x;
    lat = atan(exp(y) * 2 - pi / 2.f);
}



// https://en.wikipedia.org/wiki/Miller_cylindrical_projection
// lat = 0 at equator
inline void equirectangularProjection(float lon, float lat, float& x, float& y) {
    x = lon;
    y = lat;
}

inline void invEquirectangularProjection(float x, float y, float& lon, float& lat) {
    lon = x;
    lat = y;
}

using Coordinate = std::pair<float, float>;
using CoordinateList = std::vector<Coordinate>;
using ProjectionFunction = std::function<void(float, float, float&, float&)>;

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

using InterpolationFunction = std::function<void(const cimg_library::CImg<unsigned char>&, const Coordinate&,
    uint8_t&, uint8_t&, uint8_t&)>;

inline void interpolatePixelBilinear(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
    uint8_t& r, uint8_t& g, uint8_t& b) {
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

inline void interpolateNearestNeighbour(const cimg_library::CImg<unsigned char>& img, const Coordinate& xy,
    uint8_t& r, uint8_t& g, uint8_t& b) {
    const unsigned int x0 = static_cast<unsigned int>(std::round(xy.first));
    const unsigned int y0 = static_cast<unsigned int>(std::round(xy.second));

    r = img(x0, y0, 0);
    g = img(x0, y0, 1);
    b = img(x0, y0, 2);
}