
#ifndef __OnsetReal__LiveInputSource__
#define __OnsetReal__LiveInputSource__

#include <iostream>
#include "JuceHeader.h"
#include "DetectionFunction.h"
#include "CaptureRythm.h"


class LiveInputSource : public AudioIODeviceCallback
{
public:
    LiveInputSource(AudioDeviceManager& deviceManager);
    ~LiveInputSource();
    void audioDeviceIOCallback(const float** inputChannelData,
							   int totalNumInputChannels,
							   float** outputChannelData,
							   int totalNumOutputChannels,
							   int numSamples);
	
	void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    ScopedPointer<DetectionFunction> detectionTrial;
    CaptureRythm *cr;
    bool existOnset;
    
private:
    AudioDeviceManager& deviceManager;
    TimeSliceThread inputThread;
    AudioSampleBuffer sampleBuffer = AudioSampleBuffer(1,INPUTSIZE); //the buffer is for store;
    AudioSampleBuffer calculateBuffer = AudioSampleBuffer(1,INPUTSIZE); //the buffer is throwing to the pitchtail
    AudioSampleBuffer tempBuffer = AudioSampleBuffer(1,INPUTSIZE); // this buffer is for sliding buffer window
    bool isLiveOn;
    bool bufferReady;
};

#endif /* defined(__OnsetReal__LiveInputSource__) */
