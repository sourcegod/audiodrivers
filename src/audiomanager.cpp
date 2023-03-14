#include <cstring> // memset
#include "audiomanager.h"
#include "rtaudiodriver.h"
//----------------------------------------------------------

uint32_t _buffer_size = 256;

/// Note: Initialized static reference of (singleton) klass.
AudioManager* AudioManager::_instance = nullptr;
AudioManager* _audiom_instance = nullptr;
float *_outbuf_left = nullptr;
float *_outbuf_right = nullptr;


int _process_callback(void* input_buffer, void* output_buffer, uint32_t buf_size, void* user_data) {
    (void)input_buffer;
    (void)output_buffer;
    (void)buf_size;
    (void)user_data;
    // float *buffer = (float *) output_buffer;
    // double *lastValues = (double *) _user_data;
    // float *in = (float *)input_buffer;
    float *out = (float *)output_buffer;
    float *buf_data = new float[buf_size];


    /*
     // Copy buffer for duplex
    // Since the number of input and output channels is equal, we can do
    // a simple buffer copy operation here.
    // _buffer_bytes = nBufferFrames * _channels * sizeof(MY_TYPE);
    // std::cout << "bytes: " << *bytes << "\n";
    memcpy( outputBuffer, inputBuffer, _buffer_bytes);
    */
    
    // /*
    for (unsigned i =0; i < buf_size; i++ ) {
        *out++ = buf_data[ i ];
        *out++ = buf_data[ i ];
    }
    // */

 
    /*
    for ( uint32_t i=0; i < buf_size; i++ ) {
        for ( uint32_t j=0; j < 2; j++ ) {
            *buffer++ = (float) (lastValues[j] * SCALE * 0.5);
            // inc channel0 to base_rate: 0.005, 44100*0.005 = 220.5 HZ
            // and inc channel1 to the octave + 0.1 step
            lastValues[j] += BASE_RATE * (j+1 + (j*0.5) );
            if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
        }

    }
    */

    // std::cout << "Process Callback func, frame_count" << frame_count << "\n";
    // std::cout << "\a\n";

    return 0;
}
//----------------------------------------------------------

void init_buffers(uint32_t buf_size) {
    if ( _outbuf_left ) {
        std::cout << "[Init buffers], outbuf_left\n";
        memset(_outbuf_left, 0, buf_size * sizeof(float));
    }
    if ( _outbuf_right ) {
        std::cout << "[Init buffers], outbuf_right\n";
        memset(_outbuf_right, 0, buf_size * sizeof(float));
    }


}
//----------------------------------------------------------

void init_audio() {
      init_buffers(_buffer_size);


}
//----------------------------------------------------------

class Synth {
public:
    Synth() {}
    ~Synth() {}
    void init() { 
        _user_data = new double[2];

    }

    void process_audio_buffer(float **apf_buf, uint32_t an_count) {
        double *last_values = (double *) _user_data;
        for (uint32_t i=0; i < an_count; i++) {
            for (uint32_t j=0; j < 2; j++) {
                *apf_buf[i] +=  (float) (last_values[j] * _scale * 0.5);
                // inc channel0 to base_rate: 0.005, 44100*0.005 = 220.5 HZ
                // and inc channel1 to the octave + 0.1 step
                last_values[j] +=  _base_rate * (j+1 + (j*0.5) );
                if ( last_values[j] >= 1.0 ) last_values[j] -= 2.0;
            
            }
        
        }

    
    }
    //----------------------------------------------------------

private:
    const uint32_t _base_rate = 0.005; // rate * base_rate = 220.5, nearly note A220.
    const uint32_t _scale =1.0;
    double *_user_data = nullptr;

};
//----------------------------------------------------------

BaseAudioDriver* create_driver(const std::string& s_aud_name) {
    BaseAudioDriver *p_driver = nullptr;   
    if (s_aud_name == "RtAudio") {
        p_driver = new RtAudioDriver(_process_callback);
        if (p_driver->get_driver_name()  == "NullAudioDriver") {
            delete p_driver;  
            p_driver = nullptr;
        }
    
    } else {
        std::cerr << "[create_driver] Unknown driver" + s_aud_name + "\n";
    }

    if (p_driver != nullptr) {
      _outbuf_left = p_driver->get_outbuf_left();
      _outbuf_right = p_driver->get_outbuf_right();
      // Init buffers
      // init_audio();
    }
    
    return p_driver;
}
//----------------------------------------------------------

void AudioManager::init_drivers(const std::string& s_aud_name) {
    auto* p_driver = create_driver(s_aud_name);
    _audiod = (RtAudioDriver *)p_driver;
    init_audio();    

}
//----------------------------------------------------------

AudioManager::AudioManager() {
    // _audiod = new RtAudioDriver();
    if (_instance) {
        std::cerr << "Audio Manager is allready running.\n";
    }

    const std::string s_aud_name = "RtAudio";
    init_drivers(s_aud_name);
    _audiom_instance = this;
    _instance = this;

}
//----------------------------------------------------------

AudioManager::~AudioManager() {
    _audiom_instance = nullptr;
    _instance = nullptr;

}
//----------------------------------------------------------


AudioManager*  AudioManager::get_instance() {
    if (_instance == nullptr) {
        _instance = new AudioManager();
    }
    
    return _instance;
}
//----------------------------------------------------------


