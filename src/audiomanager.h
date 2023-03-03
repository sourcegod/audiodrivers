#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include "rtaudiodriver.h"

class AudioManager {
private:
    bool _running;

public:
    AudioManager();
    void start_driver();
    void stop_driver();

};

#endif
