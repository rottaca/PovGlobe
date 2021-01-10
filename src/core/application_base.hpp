#pragma once

#include "helper.hpp"

#include <chrono>

class Globe;

class ApplicationBase {


public:
    virtual void initialize(Globe& globe);

    virtual void process(Framebuffer& framebuffer, float time) = 0;

    virtual std::chrono::duration<float, std::milli> getTargetCycleTime() const;
};
