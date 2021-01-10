
%module PovGlobe

%include <stl.i>

%{
#include "core/globe.hpp"
#include "core/application_base.hpp"

#ifdef BUILD_HW
 #include "hw/renderer_led_strip.hpp"
 #include "hw/rpm_mreasure_hall.hpp"
#endif
#ifdef BUILD_SIM
 #include "sim/renderer_sim.hpp"
 #include "sim/rpm_measure_sim.hpp"
#endif
#if !defined(BUILD_HW) && !defined(BUILD_SIM)
  #error "No build target specified"
#endif


#include "apps/application_examples.hpp"
%}



%include "core/helper.hpp"
%include "core/globe.hpp"
%include "core/application_base.hpp"

#ifdef BUILD_HW
  %include "hw/renderer_led_strip.hpp"
  %include "hw/rpm_mreasure_hall.hpp"
#endif
#ifdef BUILD_SIM
  %include "sim/renderer_sim.hpp"
  %include "sim/rpm_measure_sim.hpp"
#endif
#if !defined(BUILD_HW) && !defined(BUILD_SIM)
  #error "No build target specified"
#endif

%include "apps/application_examples.hpp"