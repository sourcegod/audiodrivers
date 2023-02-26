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

#define FORMAT RTAUDIO_SINT16
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


