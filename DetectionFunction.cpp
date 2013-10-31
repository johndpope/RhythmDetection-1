

#include "DetectionFunction.h"

DetectionFunction::DetectionFunction(int inputSize_, int sampleRate_)
{
    samplingRate = sampleRate_;
    inputSize = inputSize_;
    resampleRate = 4000;
    //frameSize = 512;
    //hopSize = 256;
    detectionFunctionID = SPECTRALDIFF;     //using spectral difference for code review
    
    realSize = ceil(inputSize*resampleRate/samplingRate);
    //fftSize = realSize * 2 ;
    //odfSize = ceil((realSize-frameSize)/hopSize);
    
    prevOnset = 0;                                         // no onset init
    prevEnergy = 0;
    binNum = 3;
    odfBufferSize = 10;                                 
    odfBuffer = new float[odfBufferSize];                  // save 10 previous odf value for peak picking threshold
    onsetTimer = 0;
    memset(odfBuffer, 0.f, sizeof(float) * odfBufferSize); // set zeros
    threshold = 0.1f;                                      // default threshold
    
    realData = new float[realSize];                        // the data after downsampleing
    //odf = new float[odfSize];
    prevBinValue = new float[binNum];                       
    memset(prevBinValue, 0.f, sizeof(float) * binNum);        // set zeros
    
    
    // fft init
    fftSize = 1;
    FFT_Order = 0;
    //FFT_Order = log2(realSize)+1;         //real and imaginary part difference
    while (fftSize < realSize)
	{
		fftSize = fftSize << 1;
		FFT_Order ++;
	}
    fft = new SplitRadixFFT(FFT_Order);
    fftout = new float[fftSize];
    hammingWin = new float[realSize];
	float hammingCoeff = (float) 2*PI / (realSize - 1);
	for (int i=0; i<realSize; i++) {
		hammingWin[i] = (float) (0.54 - 0.46 * cos(hammingCoeff * i));
	}
    
    //temp buffer init
    copyBuffer = new float[odfBufferSize];
    
    
    
}

DetectionFunction::~DetectionFunction()
{
    delete []hammingWin;
    delete fft;
    delete []fftout;
    delete []copyBuffer;
    delete odfBuffer;
    delete realData;
    delete prevBinValue;
    
}

bool DetectionFunction::onsetMain(float *data)
{
    
    memset(realData, 0, realSize*sizeof(float));
    resample(data, realData, samplingRate, resampleRate, inputSize);    //3200 -> 4000 now, 0.3 sec3
    return process(realData);
}

void DetectionFunction::resample(float* input, float* output, unsigned in_rate, unsigned out_rate, long in_length)
{
    double ratio         = in_rate / (double)out_rate;
    unsigned out_length  = ceil(in_length / ratio);
    const double support = 4.0;
    
    for(unsigned i=0; i<out_length; ++i)
    {
        double center = i * ratio;
        double min    = center-support;
        double max    = center+support;
        
        unsigned min_in = std::max(0, (int)(min + 0.5) );
        unsigned max_in = std::min( (int)in_length-1,  (int)(max + 0.5) );
        double sum    = 0.0;
        double result = 0.0;
        for(unsigned i=min_in; i<=max_in; ++i)
        {
            double factor = sinc(i-center);     // just for temp !!
            result += input[i] * factor;
            sum    += factor;
        }
        if(sum != 0.0) result /= sum;
        output[i] = result + 0.5;
    }
}



bool DetectionFunction::process(float* signal)
{
//    if(odfSize < (signalSize - frameSize) / hopSize) {
//        printf("can't save data like this");
//    }

    // initialize parameters
    if (detectionFunctionID == ENERGYDIFF)
    {
        odfValue = energydiff(realSize, realData);
    }
    else if (detectionFunctionID == SPECTRALDIFF)
    {
        odfValue = spectraldiff(realSize, realData);
    }
    bool isOnset = peakPicking(odfValue);
    bool allowOutput = false; //false means no onset
    if (--onsetTimer <0)
    {
        onsetTimer = 0;
        allowOutput = true;
    }
    if(isOnset && allowOutput)
    {
        //std::cout<<"bang at index "<<i<<std::endl;
        onsetTimer = 10;
        return 1;
    }
    return 0;
}
//********* detection function list ************//
float DetectionFunction::energydiff(int signalSize, float *signal)
{
    float energy = 0.0;
    float diff;
    for(int i = 0; i < signalSize; i++) {
        energy += signal[i] * signal[i];            // energy is the sum of total energy in this block
    }
    // get the energy difference between current and previous frame
    diff = fabs(energy - prevEnergy);
    prevEnergy = energy;
    return diff;
}


float DetectionFunction::spectraldiff(int signalSize, float *signal)
{
    double diff = 0.0;
    float currentBin = 0.0;
    
//    for (int i = 0; i<realSize; i++)
//        std::cout<<signal[i]<<" ";
    
    for (int i =0; i < realSize; i++)
    {
        fftout[i] = signal[i]*hammingWin[i];
    }
    for (int i = realSize; i<fftSize; i++)
    {
        fftout[i]=0;
    }
    
    fft->XForm(fftout);
//    for (int i = 0; i<fftSize; i++)
//        std::cout<<fftout[i]<<" ";

    // calculate the fft into every bin and get the difference
    int binBound1 = ceil(fftSize * 1.0 / 14.0);
    int binBound2 = ceil(fftSize * 3.0 / 14.0);
    for (int i=1; i<binBound1; i++)
    {
        currentBin += sqrt(fftout[i]*fftout[i]+fftout[fftSize-i]*fftout[fftSize-i]);
    }
    diff+=(currentBin - prevBinValue[0])>0 ? (currentBin-prevBinValue[0]):0;
    prevBinValue[0] = (prevBinValue[0]*4.0 + currentBin)/5.0;
    currentBin = 0.0;
    for (int i=binBound1; i<binBound2; i++)
    {
        currentBin += sqrt(fftout[i]*fftout[i]+fftout[fftSize-i]*fftout[fftSize-i]);
    }
    diff+=(currentBin - prevBinValue[1])>0 ? (currentBin-prevBinValue[1]):0 ;
    prevBinValue[1] = (prevBinValue[1]*4.0 + currentBin)/5.0;//currentBin;
    currentBin = 0.0;
    for (int i=binBound2; i<fftSize/2; i++)
    {
        currentBin += sqrt(fftout[i]*fftout[i]+fftout[fftSize-i]*fftout[fftSize-i]);
    }
    diff+=(currentBin - prevBinValue[2])>0 ? (currentBin-prevBinValue[2]):0 ;
    prevBinValue[2] = (prevBinValue[2]*4.0 + currentBin)/5.0;//currentBin;
    currentBin = 0.0;
    
    //std::cout<<diff<<std::endl;
    return diff;
    
}
//********************************************//

bool DetectionFunction::peakPicking(float currOdfValue)      // this method has one buffer delay
{
    if (odfBuffer[0]>currOdfValue && odfBuffer[0] > odfBuffer[1] && prevOnset == false)
    {
        if(odfBuffer[0] > onsetTh)
        {
            prevOnset = 1;
        }
    }
    else
    {
        prevOnset = false;
    }
    
    // update the threshold and the odfbuffer
    updateThreshold(currOdfValue);
    //std::cout<<threshold<<std::endl;
    return prevOnset;
}

float DetectionFunction::mean(float* buffer, int size)
{
    if(size <= 0) return 0.f;
    
    float sum = 0.0;
    for(int i = 0; i < size; i++)
    {
        sum +=buffer[i];
    }
    return sum / size;
}

float DetectionFunction::median(float *buffer, int size)
{
    float candidate = 0.0;
    float temp;
    //float* bufferCopy = new float[size]; //never new in process
    memcpy(copyBuffer, buffer, sizeof(float) * size);
    for (int i=size; i>0 ;i--)
    {
        for (int j=0; j<i-1; j++)
        {
            if (copyBuffer[j]>copyBuffer[j+1])
            {
                temp = copyBuffer[j];
                copyBuffer[j] = copyBuffer[j+1];
                copyBuffer[j+1] =temp;
            }
        }
    }
    if (size % 2 == 0)
    {
        candidate = copyBuffer[((size-1)/2)]+copyBuffer[(size+1)/2];
    }
    return candidate;
}

void DetectionFunction::updateThreshold(float currOdf)
{
    for (int i=odfBufferSize-1; i>=0; i--)
    {
        odfBuffer[i+1]=odfBuffer[i]; //pop out the final buffer
    }
    odfBuffer[0] = currOdf;
    threshold = 1.0*mean(odfBuffer, odfBufferSize);//+1.0*median(odfBuffer,odfBufferSize);    // threshold = medWeight * meadian + meanweight * mean
}


int DetectionFunction::getSampleRate() {
    return samplingRate;
}

void DetectionFunction::setSampleRate(int value) {
    samplingRate = value;
}

void DetectionFunction::setDetectionFunction(int dofID)
{
    detectionFunctionID = dofID;
    if (detectionFunctionID == ENERGYDIFF) std::cout<<"change dof to energy diff"<<std::endl;
    else if (detectionFunctionID == SPECTRALDIFF) std::cout<<"change dof to spectral diff"<<std::endl;
    else
    {
        std::cout<<"no such DOF! "<<std::endl;
        detectionFunctionID = SPECTRALDIFF;
    }
    
}
