#include "application_examples.hpp"
#include "globe.hpp"
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

	framebuffer( framebuffer.getWidth() / 2, 15, 0) = 255;
	framebuffer( framebuffer.getWidth() / 2, 15, 1) = 255;
	framebuffer( framebuffer.getWidth() / 2, 15, 2) = 255;

}


ApplicationImageViwewer::ApplicationImageViwewer(const char* path)
	: ApplicationBase()
	, m_path(path)
{

}

void ApplicationImageViwewer::initialize(Globe& globe) {
	std::cout << "Opening image file " << m_path << std::endl;

	if (!std::ifstream(m_path.c_str()).good()) {
		std::cerr << "Image file " << m_path  << " does not exist!" << std::endl;
		exit(4);
	}

	m_img = cimg_library::CImg<unsigned char>(m_path.c_str());

	const float top_pixel_skip_exact = globe.getSpacingTop() / globe.getHalfCircumference() * globe.getHeight();
	m_top_pixel_skip = 0;// static_cast<int>(round(top_pixel_skip_exact));
	const float bottom_pixel_skip_exact = globe.getSpacingBottom() / globe.getHalfCircumference() * globe.getHeight();
	m_bottom_pixel_skip = 0;//static_cast<int>(round(bottom_pixel_skip_exact));

	m_img.resize(globe.getWidth(), globe.getHeight() + m_top_pixel_skip + m_bottom_pixel_skip, 1, 3);

	std::vector<std::pair<float, float>> latLon;
	for (size_t i = 0; i < globe.getHeight(); i++)
	{
		const float lat = (i - (globe.getHeight() / 2.0)) / (globe.getHeight() / 2.0) * pi / 2.0;
		for (size_t j = 0; j < globe.getWidth(); j++)
		{
			const float lon = (i - (globe.getWidth() / 2.0)) / (globe.getWidth() / 2.0) * pi;
			latLon.push_back(std::make_pair(lat, lon));
		}
	}

	const auto xy_img_for_latlon = project(latLon, invCylindricalProjection);

}

void ApplicationImageViwewer::process(Framebuffer& framebuffer, float time)
{
	for (size_t i = 0; i < framebuffer.getHeight(); i++)
	{
		const float lat = (i - (framebuffer.getHeight() / 2.0)) / (framebuffer.getHeight() / 2.0) * pi / 2.0;
		for (size_t j = 0; j < framebuffer.getWidth(); j++)
		{
			const float lon = (i - (framebuffer.getWidth() / 2.0)) / (framebuffer.getWidth() / 2.0) * pi;
			for (size_t c = 0; c < framebuffer.getChannels(); c++)
			{
				float x, y;
				invCylindricalProjection(lon, lat, x, y);
				x += (framebuffer.getWidth() / 2.0);
				y ;
				// TODO 
				//std::cout << lat << ", " << lon << " ->  " << x << ", " << y << std::endl;
				framebuffer(j, i, c) = m_img(x, y + m_top_pixel_skip, 0, c);
			}
		}
	}
}


ApplicationImageRotator::ApplicationImageRotator(const char* path)
	: ApplicationImageViwewer(path)
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
			for (size_t c = 0; c < framebuffer.getChannels(); c++)
			{
				framebuffer(j, i, c) = m_img((j + m_offset_x) % framebuffer.getWidth(), i + m_top_pixel_skip, 0, c);
			}
		}
	}

	m_offset_x++;
}
