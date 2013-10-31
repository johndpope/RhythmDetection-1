//
//  CaptureRythm.cpp
//  PitchTail_1
//
//  Created by annie zhang on 10/10/13.
//
//

#include "CaptureRythm.h"

CaptureRythm::CaptureRythm()
{
    outfile.open ("/Users/juziyt/Documents/MATLAB/robotics/out.txt");
    slice = 0;
}
CaptureRythm::~CaptureRythm()
{
    outfile.close();
}

void CaptureRythm::getOnset()
{
    match = false;
    if (!started){
        init = clock();
        started = true;
    }
    else{
        final = clock();
        time = (double)(final-init);
        if (tseq.size()>0 && tseq.back() > RESTNOTE)
            tseq.clear();
        if (time > MINDURATION)
            tseq.push_back(time);
        init = final;
    }
    cout<<time<<endl;
    if (!rhythmFound && tseq.size() >= MINNOTE && tseq.size()%2==0 && tseq.size() <= MAXNOTE){
        getTempo();
        checkRythm();
    }
    else if (rhythmFound && tseq.size() == preRhythm.size()){
        getTempo();
        matchRythm();
    }    
}

void CaptureRythm::checkRythm()
{
    int len = tseq.size()/2;
    diff.clear();
    for (int i=0; i<len; i++)
        diff.push_back(fabs(tseq[i] - tseq[i+len]));
    if (max(diff)/getMean()<THRESHOLD){
        rhythmFound = true;
        rhythmChanged = false;
        rhythmMean = getMean();
        cout<<"rythm Found"<<endl;
        preRhythm.clear();
        for (int i=0; i<len; i++){
            preRhythm.push_back(tseq[i]);
            outfile << preRhythm.back()/CLOCKS_PER_SEC*10 << " ";
            displayRhythm();
        }
        outfile << endl;
        tseq.clear();
    }
    else{
        rhythmFound = false;
        tseq.clear();
//        if (tseq.size() == MAXNOTE)
//            tseq.erase(tseq.begin(), tseq.begin()+len);
    }
}

void CaptureRythm::matchRythm()
{
    for (int i=0; i<preRhythm.size(); i++)
        diff[i] = fabs(tseq[i] - preRhythm[i]);
    if (max(diff)/rhythmMean<THRESHOLD){
        match = true;
        cout<<"match #################"<<endl;
        tseq.clear();
    }
    else {
        rhythmChanged = true;
        rhythmFound = false;
        tseq.clear();
        preRhythm.clear();
        displayRhythm();
    }
}

void CaptureRythm::reset()
{
    tseq.clear();
    rhythmChanged = false;
    rhythmFound = false;
    match = false;
    cout<<"reset ... "<<endl;
}

double CaptureRythm::getMean()
{
    double sum = 0;
    for (int i=0; i<tseq.size(); i++)
        sum += tseq[i];
    sum /= tseq.size();
    return sum;
}

double CaptureRythm::max(vector<double> &x)
{
    int idx = 0;
    double maxv = x[idx];
    for (int k=0; k<x.size(); k++)
        if (x[k]>maxv)
            maxv = x[k];
    return maxv;
}

void CaptureRythm::getTempo()
{
    tmp.clear();
    for (int i=0; i<tseq.size(); i++)
        tmp.push_back(tseq[i]);
    sort(tmp.begin(), tmp.end());
    
    period = tmp[(int)(tmp.size()/2)];
    tempo = (int)(60*CLOCKS_PER_SEC/period/10);
    if (tempo<60)
        tempo *= 2;
    else if (tempo > 240)
        tempo /= 2;
}

void CaptureRythm::getNote(double p)
{
    if (period>0){
        double ratio = period/p;
        if (ratio>3.5)
            noteSymbo = "#";
        else if(ratio>1.5)
            noteSymbo = "+";
        else
            noteSymbo = "|";
    }
}

void CaptureRythm::displayRhythm()
{
    rhythm = "";
    if (preRhythm.size() == 0)
        return;
    
    double b = max(preRhythm);
    for (int i=0; i<preRhythm.size(); i++){
        double ratio = preRhythm[i]/b;
        if (ratio <0.25)
            rhythm += "# ";
        else if (ratio<0.65)
            rhythm += "+ ";
        else
            rhythm += "| ";
    }
}
