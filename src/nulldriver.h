#ifndef NULL_AUDIO_DRIVER_H
#define NULL_AUDIO_DRIVER_H

#include "basedriver.h"
#include <iostream>

class NullAudioDriver : public BaseAudioDriver {
public:
    NullAudioDriver() : BaseAudioDriver("NullAudioDriver") { }
    ~NullAudioDriver() { }
    
    virtual void check_devices() override { };
    virtual void print_devices() override { }
    virtual unsigned int getDeviceIndex( std::vector<std::string> /*deviceNames*/ ) override { return 0; }
    virtual int init_params(unsigned int /*channels*/, unsigned int /*rate*/, 
            unsigned int /*bufferFrames*/, 
            unsigned int /*outputDevice*/) override { return 0; }
    virtual int open() override { return 0; }
    virtual void close() override { }
    virtual void start_driver() override { }
    virtual void stop_driver() override { }
    virtual bool is_running() override { return 0; }
    virtual void next_audio_block(void* /*input_buffer*/, void* /*output_buffer*/,
          unsigned int /*buffer_frames*/, double /*stream_time*/,
          unsigned int /*status*/, void* /*user_data*/) 
          override { }
    
    virtual void set_user_data(void* /*user_data*/) override { }
    virtual void set_audio_channels(unsigned int /*output_channels*/, unsigned int /*output_channels*/) override { }

};

#endif // NULL_AUDIO_DRIVER_H
