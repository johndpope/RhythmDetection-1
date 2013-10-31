//
//  CaptureRythm.h
//  PitchTail_1
//
//  Created by annie zhang on 10/10/13.
//
//

#ifndef __PitchTail_1__CaptureRythm__
#define __PitchTail_1__CaptureRythm__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>
#include <vector>

#define MINNOTE 10
#define MAXNOTE 10
#define MINDURATION 10000
#define RESTNOTE 120000
#define THRESHOLD 0.7
using namespace std;

class CaptureRythm
{
    
public:
    CaptureRythm();
    ~CaptureRythm();
    void getOnset();
    void checkRythm();
    void matchRythm();
    double max(vector<double> &x);
    double getMean();
    void getTempo();
    void getNote(double p);
    void reset();
    void displayRhythm();

    clock_t init, final;
    double time = 0;
    bool started = false;
    vector<double> tseq;
    vector<double> preRhythm;
    vector<double> diff;
    vector<double> tmp;
    bool rhythmFound = false;
    bool rhythmChanged = false;
    bool match = false;
    double rhythmMean = 1;
    int tempo = 0;
    double period = 0;
    int note = 0;
    string noteSymbo = "";
    string rhythm = "";
    ofstream outfile;
    int slice;
};

#endif /* defined(__PitchTail_1__CaptureRythm__) */
