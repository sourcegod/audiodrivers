#include "audiomanager.h"
#include "rtaudiodriver.h"

/// Note: Initialized static reference of (singleton) klass.
AudioManager* AudioManager::_instance = NULL;
AudioManager* _audiom_instance = NULL;
// /*
AudioManager::AudioManager() {
    // _audiod = new RtAudioDriver();
    if (_instance) {
        std::cerr << "Audio Manager is allready running.\n";
    }

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

BaseAudioDriver*  AudioManager::create_driver(std::string& s_aud_name) {
    BaseAudioDriver *p_driver = NULL;   
    if (s_aud_name == "RtAudio") {
        p_driver = new RtAudioDriver();
    
    } else {
        p_driver = NULL; // new NullDriver();
    }
    
    return p_driver;
}
//----------------------------------------------------------

