#include "application_examples.hpp"
#include "core/globe.hpp"
#include <iostream>
#include <fstream>

void ApplicationTest1::initialize(Globe& globe)
{
    m_last_pixel = 0;
}

void ApplicationTest1::process(Framebuffer& framebuffer, float time)
{
    for (int i = 0; i < framebuffer.getHeight(); i++) {
        framebuffer(0, i, 0) = 255;
        framebuffer(1, i, 1) = 255;
    }

    framebuffer(framebuffer.getWidth() / 2, framebuffer.getHeight() / 2, 0) = 255;
    framebuffer(framebuffer.getWidth() / 2, framebuffer.getHeight() / 2, 1) = 255;
    framebuffer(framebuffer.getWidth() / 2, framebuffer.getHeight() / 2, 2) = 255;

    framebuffer(framebuffer.getWidth() / 2, 15, 0) = 255;
    framebuffer(framebuffer.getWidth() / 2, 15, 1) = 255;
    framebuffer(framebuffer.getWidth() / 2, 15, 2) = 255;

}


ApplicationImageViwewer::ApplicationImageViwewer(const char* path,
    const ProjectionFunction projection,
    const InterpolationFunction pixelInterpolation
)
    : ApplicationBase()
    , m_path(path)
    , m_projection(projection)
    , m_pixelInterpolation(pixelInterpolation)
{}

void ApplicationImageViwewer::initialize(Globe& globe) {
    std::cout << "Opening image file " << m_path << std::endl;

    if (!std::ifstream(m_path.c_str()).good()) {
        std::cerr << "Image file " << m_path << " does not exist!" << std::endl;
        exit(4);
    }

    m_img = cimg_library::CImg<unsigned char>(m_path.c_str());

    const float top_pixel_skip_exact = globe.getSpacingTop() / globe.getHalfCircumference() * globe.getHeight();
    m_top_pixel_skip = static_cast<int>(round(top_pixel_skip_exact));
    const float bottom_pixel_skip_exact = globe.getSpacingBottom() / globe.getHalfCircumference() * globe.getHeight();
    m_bottom_pixel_skip = static_cast<int>(round(bottom_pixel_skip_exact));

    m_img.resize(globe.getWidth(), globe.getHeight() + m_top_pixel_skip + m_bottom_pixel_skip, 1, 3);

    m_xy_img_for_lonlat = buildImageProjectionLUT(m_projection,
        m_img.height(), m_img.width(), m_top_pixel_skip,
        globe.getHeight(), globe.getWidth());
}

void ApplicationImageViwewer::process(Framebuffer& framebuffer, float time)
{
    for (size_t i = 0; i < framebuffer.getHeight(); i++)
    {
        for (size_t j = 0; j < framebuffer.getWidth(); j++)
        {
            const auto& xy = m_xy_img_for_lonlat[i * framebuffer.getWidth() + j];

            m_pixelInterpolation(m_img, xy,
                framebuffer(j, i, 0),
                framebuffer(j, i, 1),
                framebuffer(j, i, 2));
        }
    }
}


ApplicationImageRotator::ApplicationImageRotator(const char* path,
    const ProjectionFunction projection,
    const InterpolationFunction pixelInterpolation)
    : ApplicationImageViwewer(path, projection, pixelInterpolation)
    , m_offset_x(0)
{}

void ApplicationImageRotator::initialize(Globe& globe) {
    ApplicationImageViwewer::initialize(globe);
    m_offset_x = 0;
}

void ApplicationImageRotator::process(Framebuffer& framebuffer, float time)
{
    for (size_t i = 0; i < framebuffer.getHeight(); i++)
    {
        for (size_t j = 0; j < framebuffer.getWidth(); j++)
        {
            const size_t j_offsetted = (j + m_offset_x) % framebuffer.getWidth();
            const auto& xy = m_xy_img_for_lonlat[i * framebuffer.getWidth() + j_offsetted];

            m_pixelInterpolation(m_img, xy,
                framebuffer(j, i, 0),
                framebuffer(j, i, 1),
                framebuffer(j, i, 2));
        }
    }

    m_offset_x++;
}
