#ifndef TINY_SYNTH_H
#define TINY_SYNTH_H
#include <cstdint> // for uint32_t type


class TinySynth {
public:
  TinySynth() {}
  ~TinySynth() { if (last_values) delete[] last_values; }

  void init();
  void process_audio_buffer(float *apf_buf, uint32_t an_count);
  void process_audio_buffer(float *apf_bufleft, float *apf_bufright, uint32_t an_count);
  

private:
    const float _base_rate = 0.005; // rate * base_rate = 220.5, nearly note A220.
    const float _scale =1.0;
    float *last_values = nullptr; // last_values[2]  = {}; // initialization all elements are zeros
    float _phase_inc =0.0;



};

#endif // TINY_SYNTH_H
