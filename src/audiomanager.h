#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include "rtaudiodriver.h"

class AudioManager {
private:
    RtAudioDriver *_audiod;

public:
    AudioManager();
    void check_devices() { _audiod->check_devices(); }
    void print_devices() { _audiod->print_devices(); }
    
    int init_params(unsigned int channels, unsigned int rate, 
            unsigned int buffer_frames, 
            unsigned int output_device) {
        return _audiod->init_params(channels, rate, 
                buffer_frames, output_device);
    }

    int open() { return _audiod->open(); }
    void close() { _audiod->close(); }
    void start_driver() { _audiod->start_driver(); }
    void stop_driver() { _audiod->stop_driver(); }
    bool is_running() { return _audiod->is_running(); }
    void set_stream_callback(RtAudioCallback stream_callback) { _audiod->set_stream_callback(stream_callback); } 
    void set_user_data(void* user_data) { _audiod->set_user_data(user_data); }
    void set_audio_channels(unsigned int input_channels, unsigned int output_channels) { 
        _audiod->set_audio_channels(input_channels, output_channels); 
    }

};

#endif
