#pragma once

#include "framebuffer.hpp"

class Globe;

class ApplicationBase {
public:
    virtual ~ApplicationBase() = default;

    virtual bool initialize(Globe& globe);
    virtual float getTargetCycleTimeMs() const;

    virtual void process(Framebuffer& framebuffer, float timeS) = 0;
};
