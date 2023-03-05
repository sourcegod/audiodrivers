#ifndef BASE_DRIVER_h
#define BASE_DRIVER_h
#include <vector>
#include <string>

/*
 * Note: Deprecated type, just for memo
typedef int (*TStreamCallback )( 
        void *inputBuffer, void * outputBuffer, 
        unsigned int nbuffer_frames, double streamTime, 
        unsigned int status, void *userData 
        );
*/

 
class BaseDriver {
private:
    unsigned int _channels =2;
    unsigned int _rate =44100;
    unsigned int _buffer_frames =256;
    unsigned int _input_device =0;
    unsigned int _output_device =0;
    void* _user_data = NULL;
    unsigned int _input_channels =0; // number of channels for input
    unsigned int _output_channels =2; // number of channels for input
    
public:
    BaseDriver() { }
    ~BaseDriver() { }
    
    virtual void check_devices() {}
    virtual void print_devices() {}
    virtual unsigned int getDeviceIndex( std::vector<std::string> /*deviceNames*/ ) { return 0; }
    virtual int init_params(unsigned int /*channels*/, unsigned int /*rate*/, 
            unsigned int /*buffer_frames*/, 
            unsigned int /*output_device*/) { return 0; }
    
    virtual int open() { return 0; }
    virtual void close() {}
    virtual void start_driver() {}
    virtual void stop_driver() {}
    virtual bool is_running() { return false; }
    virtual void next_audio_block(void* /*input_buffer*/, void* /*output_buffer*/,
        unsigned int /*buffer_frames, double stream_time*/,
        unsigned int /*status*/, void* /*user_data*/) {}
    
    virtual void set_user_data(void* user_data) { _user_data = user_data; }
    virtual void set_audio_channels(unsigned int input_channels, unsigned int output_channels) {
        _input_channels = input_channels;
        _output_channels = output_channels;
    }

};
//----------------------------------------------------------

#endif
