
%module(directors="1", threads="1")  PovGlobe

%include <stl.i>

%{
#include "core/globe.hpp"
#include "core/application_base.hpp"

#ifdef HW_AVAILABLE
 #include "hw/renderer_led_strip.hpp"
 #include "hw/rpm_mreasure_hall.hpp"
#endif
#ifdef SIM_AVAILABLE
 #include "sim/renderer_sim.hpp"
 #include "sim/rpm_measure_sim.hpp"
#endif
#if !defined(HW_AVAILABLE) && !defined(SIM_AVAILABLE)
  #error "No build target specified"
#endif


#include "apps/application_examples.hpp"
%}

%feature("director") ApplicationBase;

%include "stdint.i"

%include "std_vector.i"
namespace std {
  %template(UInt8Vector) vector<uint8_t>;
}


%include "core/helper.hpp"
%include "core/globe.hpp"
%include "core/application_base.hpp"

%inline %{
struct ColumnProxy;
class Framebuffer;

struct RowProxy {
    ColumnProxy* colProxy;
    int row;

    uint8_t __getitem__(int i);
    void __setitem__(int i, uint8_t value);
};

struct ColumnProxy {
    Framebuffer* fb;
    int col;

    RowProxy __getitem__(int i) {
        RowProxy row;
        row.colProxy = this;
        row.row = i;
        return row;
    }

    void __setitem__(int i, std::vector<uint8_t> values){
        if(values.size() == 3){
            fb->operator()(col, i, 0) = values[0];
            fb->operator()(col, i, 1) = values[1];
            fb->operator()(col, i, 2) = values[2];
        }else {
            printf("Invalid number of colours!\n");
        }
    }
};  

uint8_t RowProxy::__getitem__(int i) {
    return colProxy->fb->operator()(colProxy->col, row, i);
}

void RowProxy::__setitem__(int i, uint8_t value) {
    colProxy->fb->operator()(colProxy->col, row, i) = value;
}
%}

%include "core/framebuffer.hpp"
%extend Framebuffer {
    ColumnProxy __getitem__(int i) {
        ColumnProxy col;
        col.fb = $self;
        col.col = i;
        return col;
    }
}

%include "core/interpolation.hpp"
%include "core/projection.hpp"

#ifdef HW_AVAILABLE
  %include "hw/renderer_led_strip.hpp"
  %include "hw/rpm_mreasure_hall.hpp"
#endif
#ifdef SIM_AVAILABLE
  %include "sim/renderer_sim.hpp"
  %include "sim/rpm_measure_sim.hpp"
#endif
#if !defined(HW_AVAILABLE) && !defined(SIM_AVAILABLE)
  #error "No build target specified"
#endif

%include "apps/application_examples.hpp"


%pythoncode %{
import tile_server_api


%}