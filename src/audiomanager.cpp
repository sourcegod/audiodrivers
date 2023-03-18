#include <cstring> // memset
#include "audiomanager.h"
#include "rtaudiodriver.h"
#include "tiny_synth.h"

//----------------------------------------------------------

uint32_t _buffer_size = 256;

/// Note: Initialized static reference of (singleton) klass.
AudioManager* AudioManager::_instance = nullptr;
AudioManager* _audiom_instance = nullptr;
float *_outbuf_left = nullptr;
float *_outbuf_right = nullptr;
TinySynth *_synth = nullptr;

// prototypes
void init_buffers(uint32_t);
//----------------------------------------------------------


int _process_callback(void* input_buffer, void* output_buffer, uint32_t buf_size, void* user_data) {
    (void)input_buffer;
    (void)output_buffer;
    (void)buf_size;
    (void)user_data;
    // float *buffer = (float *) output_buffer;
    // double *lastValues = (double *) _user_data;
    // float *in = (float *)input_buffer;
    // float *out = (float *)output_buffer;
    // float *buf_data = new float[buf_size];
    init_buffers(buf_size);

    _synth->process_audio_buffer(_outbuf_left, _outbuf_right, buf_size);

    /*
     // Copy buffer for duplex
    // Since the number of input and output channels is equal, we can do
    // a simple buffer copy operation here.
    // _buffer_bytes = nBufferFrames * _channels * sizeof(MY_TYPE);
    // std::cout << "bytes: " << *bytes << "\n";
    memcpy( outputBuffer, inputBuffer, _buffer_bytes);
    */
    
    // std::cout << "\a\n";

    return 0;
}
//----------------------------------------------------------

void init_buffers(uint32_t buf_size) {
    if ( _outbuf_left ) {
        memset(_outbuf_left, 0, buf_size * sizeof(float));
    }
    if ( _outbuf_right ) {
        memset(_outbuf_right, 0, buf_size * sizeof(float));
    }


}
//----------------------------------------------------------

void init_audio() {
      
    init_buffers(_buffer_size);
    _synth = new TinySynth();
    _synth->init();


}
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
      std::cout << "[create_driver]: with " << s_aud_name << " driver.\n";
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

    // const std::string s_aud_name = "RtAudio";
    // init_drivers(s_aud_name);
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


