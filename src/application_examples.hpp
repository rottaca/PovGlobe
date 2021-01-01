#pragma once

#include "helper.hpp"
#include "application_base.hpp"
#include <CImg.h>

class ApplicationTest1 : public ApplicationBase {

public:
	virtual void initialize(Globe& globe);
	virtual void process(Framebuffer& framebuffer, float time);

private:
	int m_last_pixel;
};

class ApplicationImageViwewer : public ApplicationBase {

public:
	ApplicationImageViwewer(const char* path);

	virtual void initialize(Globe& globe);
	virtual void process(Framebuffer& framebuffer, float time);

protected:
	std::string m_path;
	cimg_library::CImg<unsigned char> m_img;

	size_t m_top_pixel_skip;
	size_t m_bottom_pixel_skip;
};


class ApplicationImageRotator : public ApplicationImageViwewer {

public:
	ApplicationImageRotator(const char* path);

	void initialize(Globe& globe);
	void process(Framebuffer& framebuffer, float time);

private:
	int m_offset_x;
};
