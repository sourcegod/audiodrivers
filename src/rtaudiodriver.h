#ifndef RTAUDIO_DRIVER_H
#define RTAUDIO_DRIVER_H
#include "basedriver.h"
#include "RtAudio.h"
#include <string>
#include <iostream>

/*
 * Note: Deprecated type, just for memo
typedef int (*TStreamCallback )( 
        void *outputBuffer, void * inputBuffer, 
        unsigned int nBufferFrames, double streamTime, 
        RtAudioStreamStatus status, void *userData 
        );
*/

 
class RtAudioDriver : public BaseAudioDriver {
private:
    RtAudio* _dac =0;
    unsigned int _channels =2;
    unsigned int _rate =44100;
    unsigned int _bufferFrames =256;
    unsigned int _outputDevice =0;
    unsigned int _inputDevice =0;
    double* _userData = NULL;
    RtAudio::StreamOptions options;
    RtAudio::StreamParameters _out_params, _in_params;
    // Specify our own error callback function.
    std::vector<unsigned int> _deviceIds; 
    RtAudioCallback _stream_callback =0;
    void* _user_data = NULL;
    unsigned int _output_channels =2; // number of channels for input
    unsigned int _input_channels =0; // number of channels for input
    
    static int stream_callback_func( 
        void *outputBuffer, void *inputBuffer, 
        unsigned int nBufferFrames, double streamTime, 
        RtAudioStreamStatus status, void *userData ) {
    
        /*
        double *lastValues = (double *) userData;
        */

        RtAudioDriver *p_obj = (RtAudioDriver *) userData;
        p_obj->next_audio_block(inputBuffer, outputBuffer, 
                nBufferFrames, streamTime,
                status, userData);

      std::cout << "\a\n";
        if ( status ) {
            std::cout << "Stream over/underflow detected." << std::endl;
        }
        
        
        /*
        frameCounter += nBufferFrames;
        if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
        */

      

        return 0;
    }
    //----------------------------------------------------------

public:
    RtAudioDriver() : BaseAudioDriver("RtAudioDriver") {
        // Note: the error callback function must static  
        // Default RtAudio constructor
        _dac = new RtAudio( RtAudio::UNSPECIFIED, &errorCallback );
        // _dac.setErrorCallback( &errorCallback ); // could use if not set via constructor

        // Tell RtAudio to output all messages, even warnings.
        _dac->showWarnings( true );

    }
    // */
    
    ~RtAudioDriver() { delete _dac; }
    
    static void errorCallback( RtAudioErrorType /*type*/, const std::string &errorText );
    void check_devices() override;
    void print_devices() override;
    unsigned int getDeviceIndex( std::vector<std::string> deviceNames ) override;
    int init_params(unsigned int channels, unsigned int rate, 
            unsigned int bufferFrames, 
            unsigned int outputDevice) override;
    int open() override;
    void close() override;
    void start_driver() override;
    void stop_driver() override;
    bool is_running() override { return _dac->isStreamRunning(); }
    void set_stream_callback(RtAudioCallback stream_callback) { _stream_callback = stream_callback; } 
    void next_audio_block(void* /*input_buffer*/, void* /*output_buffer*/,
          unsigned int /*buffer_frames*/, double /*stream_time*/,
          unsigned int /*status*/, void* /*user_data*/) 
          override {}
    
    void set_user_data(void* user_data) 
        override { _user_data = user_data; }
    void set_audio_channels(unsigned int output_channels, unsigned int input_channels) override; 

};

#endif
