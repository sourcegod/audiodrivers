/******************************************/
/*
  File: main.cpp
  RTAudio driver manager.

  Compile:
  $ g++ -Wall -D__LINUX_ALSA__ -o myfile myfile.cpp RtAudio.cpp -lasound -lpthread
    Date: Sun, 26/02/2023
    Author: Coolbrother

*/
/******************************************/

#include "RtAudio.h"
#include "rtaudiodriver.h"
#include "audiomanager.h"
#include <cstring> // for memcpy



#include <signal.h>
// Unix variants
#include <unistd.h>

#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
// typedef signed short MY_TYPE;
typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32 // RTAUDIO_SINT16
#define SCALE  1.0 // 32767.0

// Platform-dependent sleep routines.
// Interrupt handler function
bool _done;
static void finish( int /*ignore*/ ){ _done = true; }

#define BASE_RATE 0.005 // rate * base_rate = 220.5, nearly note A220.
#define TIME   1.0
unsigned int _channels =2; 
unsigned int _rate =44100;
unsigned int _buffer_frames =256;
unsigned int _buffer_bytes = _buffer_frames * _channels * sizeof(MY_TYPE);
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

void beep() {
    std::cout << "\a";
}
//----------------------------------------------------------


int _audio_callback( 
        void *outputBuffer, void *inputBuffer, 
        unsigned int nBufferFrames, double streamTime, 
        RtAudioStreamStatus status, void *userData ) {
    
    // /*
    unsigned int i, j;
    extern unsigned int _channels;
    MY_TYPE *buffer = (MY_TYPE *) outputBuffer;
    double *lastValues = (double *) userData;
    // */


       if ( status ) {
        std::cout << "Stream over/underflow detected." << std::endl;
        beep();
    }
    if ( streamTime >= streamTimePrintTime ) {
        std::cout << "streamTime = " << streamTime << std::endl;
        streamTimePrintTime += streamTimePrintIncrement;
    }


    /*
     // Copy buffer for duplex
    // Since the number of input and output channels is equal, we can do
    // a simple buffer copy operation here.
    // _buffer_bytes = nBufferFrames * _channels * sizeof(MY_TYPE);
    // std::cout << "bytes: " << *bytes << "\n";
    memcpy( outputBuffer, inputBuffer, _buffer_bytes);
    */
    
    /*
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
    */
      
    // /*
    for ( i=0; i<nBufferFrames; i++ ) {
        for ( j=0; j<_channels; j++ ) {
            *buffer++ = (MY_TYPE) (lastValues[j] * SCALE * 0.5);
            // inc channel0 to base_rate: 0.005, 44100*0.005 = 220.5 HZ
            // and inc channel1 to the octave + 0.1 step
            lastValues[j] += BASE_RATE * (j+1 + (j*0.5) );
            if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
        }

    }
    // */

    frameCounter += nBufferFrames;
    if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
  

    return 0;
}
//----------------------------------------------------------


int main( int argc, char *argv[] ) {
  AudioManager *audiom = new AudioManager();
  unsigned int device =0;
  // auto* audiom = new RtAudioDriver();
  audiom->check_devices();
  audiom->print_devices();

  if (argc > 1)
    device = (unsigned int) atoi( argv[1] );
  
  if ( nFrames > 0 ) checkCount = true;

  _userData = (double *) calloc( _channels, sizeof( double ) );

    // /*
    audiom->init_params(_channels, _rate, _buffer_frames, device);
    audiom->set_stream_callback(_audio_callback);
    audiom->set_user_data(_userData);
    audiom->set_audio_channels(0, 2); // No inputs, 2 outputs
    audiom->open();

    // Stream is open ... now start it.
    audiom->start_driver();

    if ( checkCount ) {
      while ( audiom->is_running() == true ) SLEEP( 100 );
    } else {
      std::cout << "\nPlaying ... quit with Ctrl-C (buffer size = " << _buffer_frames << ").\n";
    }

    // Install an interrupt handler function.
    _done = false;
    (void) signal(SIGINT, finish);

    while ( !_done && audiom->is_running() ) SLEEP( 100 );

    // Block released ... stop the stream
    audiom->stop_driver();
    audiom->close();
    // */



    return 0;
}
