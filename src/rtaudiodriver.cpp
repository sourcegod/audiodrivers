/******************************************/
/*
  File: rtaudiodriver.cpp
  RTAudio driver manager.

  Compile:
  $ g++ -Wall -D__LINUX_ALSA__ -o myfile myfile.cpp RtAudio.cpp -lasound -lpthread

  Date: Tue, 23/02/2023
  Author: Coolbrother

*/
/******************************************/

#include "RtAudio.h"
#include "rtaudiodriver.h"

#include <iostream>
#include <cstdlib>
#include <signal.h>
// Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )


typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE  32767.0

// Platform-dependent sleep routines.
// Interrupt handler function
bool done;
static void finish( int /*ignore*/ ){ done = true; }

#define BASE_RATE 0.005 // rate * base_rate = 220.5, nearly note A220.
#define TIME   1.0
unsigned int _channels =2;
unsigned int _rate =44100;
unsigned int _bufferFrames =256;
double* _userData = NULL;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
const unsigned int callbackReturnValue = 1; // 1 = stop and drain, 2 = abort
double streamTimePrintIncrement = 1.0; // seconds
double streamTimePrintTime = 1.0; // seconds

int _saw_callback( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ) {
    // Deprecated function
    unsigned int i, j;
    extern unsigned int _channels;
    MY_TYPE *buffer = (MY_TYPE *) outputBuffer;
    double *lastValues = (double *) userData;

    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;

    if ( streamTime >= streamTimePrintTime ) {
        std::cout << "streamTime = " << streamTime << std::endl;
        streamTimePrintTime += streamTimePrintIncrement;
    }

    for ( i=0; i<nBufferFrames; i++ ) {
        for ( j=0; j<_channels; j++ ) {
            *buffer++ = (MY_TYPE) (lastValues[j] * SCALE * 0.5);
            lastValues[j] += BASE_RATE * (j+1+(j*0.1));
            if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
        }
    }

    frameCounter += nBufferFrames;
    if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
    
    return 0;
}
//----------------------------------------------------------


int _audio_callback( 
        void *outputBuffer, void * /*inputBuffer*/, 
        unsigned int nBufferFrames, double streamTime, 
        RtAudioStreamStatus status, void *userData ) {
    
    unsigned int i, j;
    extern unsigned int _channels;
    MY_TYPE *buffer = (MY_TYPE *) outputBuffer;
    double *lastValues = (double *) userData;

    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;

    if ( streamTime >= streamTimePrintTime ) {
        std::cout << "streamTime = " << streamTime << std::endl;
        streamTimePrintTime += streamTimePrintIncrement;
    }

    for ( i=0; i<nBufferFrames; i++ ) {
        for ( j=0; j<_channels; j++ ) {
            *buffer++ = (MY_TYPE) (lastValues[j] * SCALE * 0.5);
            // inc channel0 to base_rate: 0.005, 44100*0.005 = 220.5 HZ
            // and inc channel1 to the octave + 0.1 step
            lastValues[j] += BASE_RATE * (j+1 + (j*0.5) );
            if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
        }

    }

    frameCounter += nBufferFrames;
    if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
  

    return 0;
}
//----------------------------------------------------------


/*
RtAudioDriver::RtAudioDriver() {
    // _dac = RtAudio( RtAudio::UNSPECIFIED, &errorCallback );
}
//----------------------------------------------------------
*/

void RtAudioDriver::errorCallback( RtAudioErrorType /*type*/, const std::string &errorText ) {
  // This example error handling function simply outputs the error message to stderr.
  std::cerr << "\nerrorCallback: " << errorText << "\n\n";
}
//----------------------------------------------------------

unsigned int RtAudioDriver::getDeviceIndex( std::vector<std::string> deviceNames ) {
  unsigned int i;
  std::string keyHit;
  std::cout << '\n';
  for ( i=0; i<deviceNames.size(); i++ )
    std::cout << "  Device #" << i << ": " << deviceNames[i] << '\n';
  
  do {
    std::cout << "\nChoose a device #: ";
    std::cin >> i;
    // std::cout << "voici i: " << i << "\n";
  } while ( i >= deviceNames.size() );
  std::getline( std::cin, keyHit );  // used to clear out stdin
  return i;
}
//----------------------------------------------------------

void RtAudioDriver::print_devices() {
    auto deviceNames = _dac->getDeviceNames();
    std::cout << "\nList of audio Devices\n";
    for ( unsigned int i=0; i<deviceNames.size(); i++ )
      std::cout << "  Device #" << i << ": " << deviceNames[i] << '\n';

}
//----------------------------------------------------------


void RtAudioDriver::check_devices() {
    _deviceIds = _dac->getDeviceIds();
    if ( _deviceIds.size() < 1 ) {
      std::cout << "\nNo audio devices found!\n";
      exit( 1 );
    }

}
//----------------------------------------------------------


int RtAudioDriver::init_params(unsigned int channels, unsigned int rate, unsigned int bufferFrames, 
        unsigned int outputDevice) {
    _channels = channels;
    _rate = rate;
    _bufferFrames = bufferFrames;
    unsigned int deviceId =0;
    oParams.nChannels = _channels;
    if ( outputDevice == 0 ) {
        deviceId = _dac->getDefaultOutputDevice();
        oParams.deviceId  = deviceId;
    } else {
        if ( outputDevice >= _deviceIds.size() ) {
            outputDevice = getDeviceIndex( _dac->getDeviceNames() );
        }
        deviceId = _deviceIds[outputDevice];
        oParams.deviceId  = deviceId;
      
    }

    return 0;
}
//----------------------------------------------------------

int RtAudioDriver::open() {
    // opening devices in exclusive mode  
    // options.flags = RTAUDIO_HOG_DEVICE;
    options.flags |= RTAUDIO_SCHEDULE_REALTIME;
    // An error in the openStream() function can be detected either by
    // checking for a non-zero return value OR by a subsequent call to
    // isStreamOpen().
    if ( _dac->openStream( 
          &oParams, // output params
          NULL, // input params
          FORMAT, _rate, 
          &_bufferFrames, 
          _stream_callback, 
          _user_data, 
          &options ) ) {
      std::cout << _dac->getErrorText() << std::endl;
      close();
    }
    if ( _dac->isStreamOpen() == false ) close();

    std::cout << "Stream latency = " << _dac->getStreamLatency() << "\n" << std::endl;

    return 0;
}
//----------------------------------------------------------

void RtAudioDriver::close() {
    if (_dac->isStreamOpen() ) _dac->closeStream();
    free( _userData );
    std::cout << "Closing the Audio Driver...\n";

}
//----------------------------------------------------------


void RtAudioDriver::start_driver() {
    if ( _dac->startStream() ) {
        std::cout << _dac->getErrorText() << std::endl;
        close();
    } else {
        std::cout << "Starting the Stream...\n";
    }

}
//----------------------------------------------------------

void RtAudioDriver::stop_driver() {
    if ( _dac->isStreamRunning() )
      _dac->stopStream();  // or could call _dac->abortStream();
    std::cout << "Stopping the Stream...\n";

}
//----------------------------------------------------------

int main( int argc, char *argv[] ) {
  unsigned int bufferFrames, device =0;
  auto* audiom = new RtAudioDriver();
  audiom->check_devices();
  audiom->print_devices();

  if (argc > 1)
    device = (unsigned int) atoi( argv[1] );
  
  if ( nFrames > 0 ) checkCount = true;

  _userData = (double *) calloc( _channels, sizeof( double ) );

  bufferFrames = 256;
  // /*
  audiom->init_params(_channels, _rate, bufferFrames, device);
  audiom->set_stream_callback(_audio_callback);
  audiom->set_user_data(_userData);
  audiom->open();

  // Stream is open ... now start it.
  audiom->start_driver();

  if ( checkCount ) {
    while ( audiom->is_running() == true ) SLEEP( 100 );
  } else {
    std::cout << "\nPlaying ... quit with Ctrl-C (buffer size = " << bufferFrames << ").\n";
  }

    // Install an interrupt handler function.
    done = false;
    (void) signal(SIGINT, finish);

    while ( !done && audiom->is_running() ) SLEEP( 100 );

    // Block released ... stop the stream
    audiom->stop_driver();
    audiom->close();


    return 0;
}
