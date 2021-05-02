#include "application_examples.hpp"
#include "core/globe.hpp"
#include <iostream>
#include <fstream>

bool ApplicationTest1::initialize(Globe& globe)
{
    m_horizontal_offset = 0;
    return true;
}

void ApplicationTest1::process(Framebuffer& framebuffer, float time)
{
    const int w = framebuffer.getWidth();
    const int w_2 = w / 2;
    const int h = framebuffer.getHeight();
    const int h_2 = h / 2;

    framebuffer.fill(0);

    const int col = (w_2 + m_horizontal_offset) % w;
    if (time > 3.0){
        m_horizontal_offset = (m_horizontal_offset + 1) % w;
    }

    for (int i = 0; i < h; i++) {
        framebuffer(0, i, 0) = 255;
        framebuffer(1, i, 1) = 255;
    }

    framebuffer(col, h_2, 0) = 0;
    framebuffer(col, h_2, 1) = 255;
    framebuffer(col, h_2, 2) = 0;

    framebuffer(col, 10, 0) = 0;
    framebuffer(col, 10, 1) = 0;
    framebuffer(col, 10, 2) = 128;
    
    framebuffer(col, 15, 0) = 128;
    framebuffer(col, 15, 1) = 0;
    framebuffer(col, 15, 2) = 0;
    
    framebuffer(col, 20, 0) = 0;
    framebuffer(col, 20, 1) = 128;
    framebuffer(col, 20, 2) = 0;
    
    
    for (int i = 0; i < h; i++) {
      int new_col = (col + 5) % w;
      // green to red
      framebuffer(new_col, i, 0) = i*255/(h-1);
      framebuffer(new_col, i, 1) = (h-1-i)*255/(h-1);
      framebuffer(new_col, i, 2) = 0; 
      new_col = (new_col + 2) % w;
      // blue to green
      framebuffer(new_col, i, 0) = 0;
      framebuffer(new_col, i, 1) = i*255/(h-1);
      framebuffer(new_col, i, 2) = (h-1-i)*255/(h-1);
      new_col = (new_col + 2) % w;
      // red to blue  
      framebuffer(new_col, i, 0) = (h-1-i)*255/(h-1);
      framebuffer(new_col, i, 1) = 0;
      framebuffer(new_col, i, 2) = i*255/(h-1);
      new_col = (new_col + 2) % w;
      // white to black
      framebuffer(new_col, i, 0) = (h-1-i)*255/(h-1);
      framebuffer(new_col, i, 1) = (h-1-i)*255/(h-1);
      framebuffer(new_col, i, 2) = (h-1-i)*255/(h-1);
    }
}


bool ApplicationTest2::initialize(Globe& globe)
{
    return true;
}

void ApplicationTest2::process(Framebuffer& framebuffer, float)
{
    const int w = framebuffer.getWidth();
    const int w_2 = w / 2;
    const int h = framebuffer.getHeight();
    const int h_2 = h / 2;

    framebuffer.fill(0);

    for (int j = 0; j < w; j += 10) {
        for (int i = 0; i < h; i++) {
            framebuffer(j, i, 0) = (j==0)?0:255;
            framebuffer(j, i, 1) = 255;
            framebuffer(j, i, 2) = (j==0)?0:255;
        }
    }
}


ApplicationImageViewer::ApplicationImageViewer(const char* path,
    const ProjectionFunction& projection,
    const InterpolationFunction& pixelInterpolation
)
    : ApplicationBase()
    , m_path(path)
    , m_projection(projection)
    , m_pixelInterpolation(pixelInterpolation)
{}

bool ApplicationImageViewer::initialize(Globe& globe) {
    std::cout << "Opening image file " << m_path << std::endl;

    if (!std::ifstream(m_path.c_str()).good()) {
        std::cerr << "Image file " << m_path << " does not exist!" << std::endl;
        return false;
    }

    m_img = cimg_library::CImg<unsigned char>(m_path.c_str());
    std::cout << "Loaded image: "<< m_img.width() << "x" << m_img.height() << std::endl;
    
    // Lets assume, that image offsets are symmetrical along the vertical axis
    // If an image is e.g. too wide, we have to cut of the vertical range
    // A aspect ratio of 2.0 matches the surface of a sphere
    const float image_aspect = m_img.width() / m_img.height();
    const float max_img_lat = pi / 2.f * 2.0 / image_aspect;
    const float min_img_lat = -max_img_lat;

    std::cout << "Image Aspect ratio:" << image_aspect << std::endl;
    std::cout << "Resizing image to match the globe size: " << globe.getHorizontalNumPixels() << "x" << 2.0 / image_aspect * globe.getTotalVerticalNumPixels() << std::endl;

    m_img.resize(globe.getHorizontalNumPixels(), 2.0 / image_aspect * globe.getTotalVerticalNumPixels(), 1, 3);
    std::cout << "Building image projection lookup table" << std::endl;
    m_xy_img_for_lonlat = buildImageProjectionLUT(m_projection, globe, m_img.height(), m_img.width(), max_img_lat, min_img_lat);

    return true;
}

void ApplicationImageViewer::process(Framebuffer& framebuffer, float)
{
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        for (size_t i = 0; i < framebuffer.getHeight(); i++)
        {
            const auto& xy = m_xy_img_for_lonlat[j * framebuffer.getHeight() + i];

            m_pixelInterpolation(m_img, xy,
                framebuffer(j, i, 0),
                framebuffer(j, i, 1),
                framebuffer(j, i, 2));
        }
    }
}


ApplicationImageRotator::ApplicationImageRotator(const char* path,
    const ProjectionFunction& projection,
    const InterpolationFunction& pixelInterpolation)
    : ApplicationImageViewer(path, projection, pixelInterpolation)
    , m_offset_x(0)
{}

bool ApplicationImageRotator::initialize(Globe& globe) {
    m_offset_x = 0;
    return ApplicationImageViewer::initialize(globe);
}

void ApplicationImageRotator::process(Framebuffer& framebuffer, float time)
{
    m_offset_x = (m_offset_x + 1) % framebuffer.getWidth();
    
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        const size_t j_offsetted = (j + m_offset_x) % framebuffer.getWidth();  
        for (size_t i = 0; i < framebuffer.getHeight(); i++)
        {
            const auto& xy = m_xy_img_for_lonlat[j_offsetted * framebuffer.getHeight() + i];

            m_pixelInterpolation(m_img, xy,
                framebuffer(j, i, 0),
                framebuffer(j, i, 1),
                framebuffer(j, i, 2));
        }
    }
}
