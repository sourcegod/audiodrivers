#ifndef RTAUDIO_DRIVER_H
#define RTAUDIO_DRIVER_H
#include "../config.h"
#include "basedriver.h"
#include "nulldriver.h"
#include <string>
#include <iostream>

#ifdef RTAUDIO_SUPPORT
    #include "RtAudio.h"

typedef int (*TAudioProcessCallback )(uint32_t, void*); 
/*
 * Note: Deprecated type, just for memo
typedef int (*TStreamCallback )( 
        void *outputBuffer, void * inputBuffer, 
        unsigned int nBufferFrames, double streamTime, 
        RtAudioStreamStatus status, void *userData 
        );
*/

 
class RtAudioDriver : public BaseAudioDriver {
public:
    TAudioProcessCallback _process_callback;
    float *_out_left = NULL;
    float *_out_right = NULL;
 
    RtAudioDriver(TAudioProcessCallback process_callback) 
      : BaseAudioDriver("RtAudioDriver"), 
      _process_callback(process_callback) {
        // Note: the error callback function must static  
        // Default RtAudio constructor
        _dac = new RtAudio( RtAudio::UNSPECIFIED, &errorCallback );
        // _dac.setErrorCallback( &errorCallback ); // could use if not set via constructor

        // Tell RtAudio to output all messages, even warnings.
        _dac->showWarnings( true );

    }
    
    ~RtAudioDriver() { delete _dac; }
    
    static void errorCallback( RtAudioErrorType /*type*/, const std::string &errorText );
    virtual void check_devices() override;
    virtual void print_devices() override;
    virtual unsigned int getDeviceIndex( std::vector<std::string> deviceNames ) override;
    virtual int init_params(unsigned int channels, unsigned int rate, 
            unsigned int bufferFrames, 
            unsigned int outputDevice) override;
    virtual int open() override;
    virtual void close() override;
    virtual void start_driver() override;
    virtual void stop_driver() override;
    virtual bool is_running() override { return _dac->isStreamRunning(); }
    void set_stream_callback(RtAudioCallback stream_callback) { _stream_callback = stream_callback; } 
    virtual void next_audio_block(void* /*input_buffer*/, void* /*output_buffer*/,
          unsigned int /*buffer_frames*/, double /*stream_time*/,
          unsigned int /*status*/, void* /*user_data*/) 
          override {}
    
    virtual void set_user_data(void* user_data) 
        override { _user_data = user_data; }
    virtual void set_audio_channels(unsigned int output_channels, unsigned int input_channels) override; 

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
    
        (void)outputBuffer;

        (void)inputBuffer;
        (void)streamTime;
        (void)status;

        /*
            double *lastValues = (double *) userData;
        */

        RtAudioDriver *p_driver = (RtAudioDriver *) userData;
        float *out = (float *)outputBuffer;
        // float *in = (float *)inputBuffer;


        
        /*
        p_obj->next_audio_block(inputBuffer, outputBuffer, 
                nBufferFrames, streamTime,
                status, userData);
        */
        
        p_driver->_process_callback(nBufferFrames, NULL);
	      for (unsigned i =0; i < nBufferFrames; i++ ) {
		        *out++ = p_driver->_out_left[ i ];
		        *out++ = p_driver->_out_right[ i ];
	      }

        // std::cout << "\a\n";
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

};

#else

class RtAudioDriver : public NullAudioDriver {
public:
    RtAudioDriver() : NullDriver() {}

};

#endif // RTAUDIO_SUPPORT
#endif // RTAUDIO_DRIVER_H
