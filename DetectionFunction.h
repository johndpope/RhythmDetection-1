

#ifndef __OnsetReal__DetectionFunction__
#define __OnsetReal__DetectionFunction__

#include <iostream>
#include <cmath>
#include "math.h"
#include "rsrfft.h"

#define PI 3.1415926535
#define FS 44100
#define INPUTSIZE  FS*0.256       // inputsize = 0.3 secs per onset

#define ENERGYDIFF 0
#define SPECTRALDIFF 1

class DetectionFunction
{

public:
    DetectionFunction(int inputSize_, int sampleRate_);
    ~DetectionFunction();
    
    void resample(float* input, float* output, unsigned in_rate, unsigned out_rate, long in_length);
    bool onsetMain(float* data);
    bool process(float* data);
    bool peakPicking(float currOdf);
    inline double sinc(double x)
    {
        if(x == 0.0) return 1.0;
        else x *= PI;
        return sin(x) / x;
    }
    float mean(float* buffer, int size);
    float median(float* buffer, int size);
    float energydiff(int signalSize, float *signal);
    float spectraldiff(int signalSize, float * signal);
    void updateThreshold(float currOdf);
    // basic parameters setter/getter
    int getSampleRate();
    void setSampleRate(int value);
    void setDetectionFunction(int dofID);
    void init();    // init everything that means I change to a new source, define a new scenario
    int onsetTh = 150;
    
protected:
    int samplingRate;
//    int frameSize;
//    int hopSize;
    int resampleRate;
    

private:
    // peak - picking variables
    bool prevOnset;      // determine previous frame is onset
    float threshold;     // detection framework
    int odfBufferSize;   // the number of odf
    float* odfBuffer;    // store the odfvalue
    float* copyBuffer;
    int onsetTimer;
    
    //data after pre-processing
    float* realData;
    //float* odf;
    float odfValue;
    int inputSize;
    int realSize;           // the real frame size we are going to handle
    int fftSize;            // so fft size is always twice the realsize
    //int odfSize;
    
    //detection function variables
    int detectionFunctionID;    
    int prevEnergy;
    float* fftout;
    int binNum;
    float* prevBinValue;
    int FFT_Order;
    SplitRadixFFT *fft;
    float* hammingWin;
    
};

#endif /* defined(__OnsetReal__DetectionFunction__) */
