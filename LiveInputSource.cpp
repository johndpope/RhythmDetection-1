

#include "LiveInputSource.h"
LiveInputSource::LiveInputSource(AudioDeviceManager& deviceManager_):deviceManager(deviceManager_), inputThread("real time I/O")
{
    AudioDeviceManager::AudioDeviceSetup config;
    deviceManager.getAudioDeviceSetup(config);
    //config.bufferSize = 1024;
    //config.sampleRate = FS;
    
    //deviceManager.setAudioDeviceSetup(config, true);
    deviceManager.addAudioCallback(this);
    bufferReady = false;
    
    // cresate onset detector here
    detectionTrial = new DetectionFunction(INPUTSIZE,FS);
    cr = new CaptureRythm();
}

LiveInputSource::~LiveInputSource()
{
    deviceManager.removeAudioCallback(this);
    isLiveOn = false;
    delete cr;
}

void LiveInputSource::audioDeviceAboutToStart(AudioIODevice* device)
{
    isLiveOn = true;
}

void LiveInputSource::audioDeviceStopped()
{
    isLiveOn = false;
}

void LiveInputSource::audioDeviceIOCallback( const float** inputChannelData,
                                            int totalNumInputChannels,
                                            float** outputChannelData,
                                            int totalNumOutputChannels,
                                            int numSamples)
{
    // remove the junk data=
    for (int i = 0; i < numSamples; ++i)
    {
        for (int j = totalNumOutputChannels; --j >= 0;)
        {
            if (outputChannelData[j] != 0){
                outputChannelData[j][i] = 0;
            }
        }
    }
    
    //get sample here
    if (bufferReady == true)
    {
        existOnset = 0;
        existOnset = detectionTrial->onsetMain(calculateBuffer.getSampleData(0)); //monophonic data
        if (existOnset == 1)
        {
            cr->getOnset();
        }
        bufferReady = false;
        
    }
    
    if (bufferReady == false)
    {
        sampleBuffer.copyFrom(0, 0, inputChannelData[0], numSamples);
        tempBuffer.copyFrom(0, 0, calculateBuffer, 0, numSamples, INPUTSIZE - numSamples);
        calculateBuffer.clear();
        tempBuffer.copyFrom(0, INPUTSIZE - numSamples, sampleBuffer, 0, 0, numSamples);
        calculateBuffer.copyFrom(0, 0, tempBuffer, 0, 0, INPUTSIZE);
        tempBuffer.clear();
        bufferReady = true;
        sampleBuffer.clear();
    }
    
//    int i = detectionTrial->processFrame(numSamples, (float *)inputChannelData[0]);
//    if (i>1) printf("bang!");
    
}
