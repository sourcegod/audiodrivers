#include <cstring> // memset
#include "audiomanager.h"
#include "rtaudiodriver.h"
//----------------------------------------------------------

uint32_t _buffer_size = 256;

/// Note: Initialized static reference of (singleton) klass.
AudioManager* AudioManager::_instance = NULL;
AudioManager* _audiom_instance = NULL;
float *_mainbuf_left = NULL;
float *_mainbuf_right = NULL;


int _process_callback(uint32_t frame_count, void* user_data) {
    (void)frame_count;
    (void)user_data;
    // std::cout << "Process Callback func, frame_count" << frame_count << "\n";
    // std::cout << "\a\n";

    return 0;
}
//----------------------------------------------------------

void init_buffers(uint32_t buf_size) {
    if ( _mainbuf_left ) {
        std::cout << "[Init buffers], mainbuf_left\n";
        memset(_mainbuf_left, 0, buf_size * sizeof(float));
    }
    if ( _mainbuf_right ) {
        std::cout << "[Init buffers], mainbuf_right\n";
        memset(_mainbuf_right, 0, buf_size * sizeof(float));
    }


}
//----------------------------------------------------------

void init_audio() {
      init_buffers(_buffer_size);


}
//----------------------------------------------------------

BaseAudioDriver* create_driver(const std::string& s_aud_name) {
    BaseAudioDriver *p_driver = NULL;   
    if (s_aud_name == "RtAudio") {
        p_driver = new RtAudioDriver(_process_callback);
        if (p_driver->get_driver_name()  == "NullAudioDriver") {
            delete p_driver;  
            p_driver = NULL;
        }
    
    } else {
        std::cerr << "[create_driver] Unknown driver" + s_aud_name + "\n";
    }

    if (p_driver != NULL) {
      _mainbuf_left = p_driver->get_outbuf_left();
      _mainbuf_right = p_driver->get_outbuf_right();
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
    _audiom_instance = NULL;
    _instance = NULL;

}
//----------------------------------------------------------


AudioManager*  AudioManager::get_instance() {
    if (_instance == NULL) {
        _instance = new AudioManager();
    }
    
    return _instance;
}
//----------------------------------------------------------


