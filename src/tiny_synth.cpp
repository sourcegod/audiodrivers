#include "tiny_synth.h"
#include <iostream>

TinySynth::TinySynth() {}
TinySynth::~TinySynth() { if (_user_data) delete[] _user_data; }
void TinySynth::init() { 
    _user_data = new double[2];

}
//----------------------------------------------------------

void TinySynth::process_audio_buffer(float *apf_buf, uint32_t an_count) {
    double *last_values = (double *) _user_data;
    for (uint32_t i=0; i < an_count; i++) {
        for (uint32_t j=0; j < 2; j++) {
            apf_buf[i] +=  (float) (last_values[j] * _scale * 0.5);
            // inc channel0 to base_rate: 0.005, 44100*0.005 = 220.5 HZ
            // and inc channel1 to the octave + 0.1 step
            last_values[j] +=  _base_rate * (j+1 + (j*0.5) );
            if ( last_values[j] >= 1.0 ) last_values[j] -= 2.0;
        
        }
    
    }


}
//----------------------------------------------------------

void TinySynth::process_audio_buffer(float *apf_bufleft, float *apf_bufright, uint32_t an_count) {
    double *last_values = (double *) _user_data;
    float val;
    if (!apf_bufleft) return;
    if (!apf_bufright) return;
    // std::cout << "[process_data]: count: " << an_count << " \n";
    for (uint32_t i=0; i < an_count; i++) {
        for (uint32_t j=0; j < 2; j++) {
            val = (float) (last_values[j] * _scale * 0.5);
            if (j == 0) apf_bufleft[i] += val;
            else apf_bufright[i] +=  val; 
            
            // inc channel0 to base_rate: 0.005, 44100*0.005 = 220.5 HZ
            // and inc channel1 to the octave + 0.1 step
            last_values[j] +=  _base_rate * (j+1 + (j*0.5) );
            if ( last_values[j] >= 1.0 ) last_values[j] -= 2.0;
        }
        
    }


}
//----------------------------------------------------------

