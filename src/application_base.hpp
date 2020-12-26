#pragma once

#include "helper.hpp"

class Globe;

class ApplicationBase {

public:
	void initialize(Globe& globe);

	void process(Framebuffer& framebuffer, float time);
};
