#pragma once

#include <CImg.h>

#include "core/helper.hpp"
#include "core/application_base.hpp"
#include "core/projection.hpp"
#include "core/interpolation.hpp"

class ApplicationTest1 : public ApplicationBase {

public:
    virtual bool initialize(Globe& globe);
    virtual void process(Framebuffer& framebuffer, float time);

private:
    int m_horizontal_offset;
};
class ApplicationTest2 : public ApplicationBase {

public:
    virtual bool initialize(Globe& globe);
    virtual void process(Framebuffer& framebuffer, float time);
};

class ApplicationImageViewer : public ApplicationBase {

public:
    ApplicationImageViewer(const char* path,
    const ProjectionFunction& projection,
    const InterpolationFunction& pixelInterpolation);

    virtual bool initialize(Globe& globe);
    virtual void process(Framebuffer& framebuffer, float time);

    float getTargetCycleTimeMs() const {
        return 1000.f;
    }

protected:
    const ProjectionFunction& m_projection;
    const InterpolationFunction& m_pixelInterpolation;
    const std::string m_path;

    cimg_library::CImg<unsigned char> m_img;

    size_t m_top_pixel_skip;
    size_t m_bottom_pixel_skip;

    CoordinateList m_xy_img_for_lonlat;
};


class ApplicationImageRotator : public ApplicationImageViewer {

public:
    ApplicationImageRotator(const char* path,
    const ProjectionFunction& projection,
    const InterpolationFunction& pixelInterpolation);

    bool initialize(Globe& globe);
    void process(Framebuffer& framebuffer, float time);

    float getTargetCycleTimeMs() const {
        return 33.f;
    }

private:
    int m_offset_x;
};
