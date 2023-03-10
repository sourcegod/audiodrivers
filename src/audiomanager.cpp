#include "audiomanager.h"
#include "rtaudiodriver.h"

/// Note: Initialized static reference of (singleton) klass.
AudioManager* AudioManager::_instance = NULL;
AudioManager* _audiom_instance = NULL;


int _process_callback(uint32_t, void*) {
    std::cout << "Process Callback func\n";
    std::cout << "\a\n";

    return 0;
}
//----------------------------------------------------------

// /*
AudioManager::AudioManager() {
    // _audiod = new RtAudioDriver();
    if (_instance) {
        std::cerr << "Audio Manager is allready running.\n";
    }

    const std::string s_aud_name = "RtAudio";
    create_driver(s_aud_name);
    _audiom_instance = this;
    _instance = this;

}
//----------------------------------------------------------
// */

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

BaseAudioDriver*  AudioManager::create_driver(const std::string& s_aud_name) {
    BaseAudioDriver *p_driver = NULL;   
    if (s_aud_name == "RtAudio") {
        p_driver = new RtAudioDriver(_process_callback);
        _audiod = (RtAudioDriver *)p_driver;
        if (p_driver->get_driver_name()  == "NullAudioDriver") {
            delete p_driver;  
            p_driver = NULL;
        }
    
    } else {
        std::cerr << "[create_driver] Unknown driver" + s_aud_name + "\n";
    }

    if (p_driver != NULL) {
      //
        // Init driver
    }
    
    return p_driver;
}
//----------------------------------------------------------

