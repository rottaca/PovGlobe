#include "application_examples.hpp"
#include "globe.hpp"

ApplicationImageViwewer::ApplicationImageViwewer(const std::string& path)
	: ApplicationBase()
	, m_path(path)
{

}

void ApplicationImageViwewer::initialize(Globe& globe) {
	m_img = cimg_library::CImg<unsigned char>(m_path.c_str());


	m_img.resize(globe.getWidth(), globe.getHeight(), 1, 3);
	m_img.permute_axes("yxzc");


}

void ApplicationImageViwewer::process(Framebuffer& framebuffer, float time)
{
	const unsigned char* p = m_img.data();

	for (size_t i = 0; i < framebuffer.getSize(); i++) {
		framebuffer[i] = *p++;
	}
}
