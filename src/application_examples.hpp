#pragma once

#include "helper.hpp"
#include "application_base.hpp"
#include <CImg.h>

class ApplicationImageViwewer: public ApplicationBase {


public:

	ApplicationImageViwewer(const std::string& path);

	void initialize(Globe& globe);

	void process(Framebuffer& framebuffer, float time);

private:
	const std::string& m_path;
	cimg_library::CImg<unsigned char> m_img;
};
