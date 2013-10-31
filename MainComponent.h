

#ifndef __OnsetReal__MainComponent__
#define __OnsetReal__MainComponent__

#include <iostream>
#include "JuceHeader.h"
#include "LiveInputSource.h"
#include "AudioFileSource.h"

class MainComponent  : public Component, public ButtonListener, public SliderListener, public Timer
{
public:
    //==============================================================================
    MainComponent ();
    ~MainComponent();
    
    //==============================================================================s]
    
    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void visibilityChanged();
    void timerCallback();
    void sliderValueChanged(Slider* sliderThatWasMoved);
    int th = 100;
    
private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager deviceManager;
    ScopedPointer<LiveInputSource> streaming;
    ScopedPointer<AudioFileSource> player;
    File currentFile;
    //[/UserVariables]
    
    //==============================================================================
    //    Label* helloWorldLabel;
    Path internalPath1;
    ScopedPointer<Label> title;
    ScopedPointer<Label> state;
    ScopedPointer<Label> fileSelected;
    ScopedPointer<Label> tempoValueLabel;
    ScopedPointer<Label> rhythmValueLabel;
    
    ScopedPointer<ComboBox> userCase;
    ScopedPointer<TextButton> loadButton;
    ScopedPointer<TextButton> audioInputButton;
    ScopedPointer<TextButton> dofButton;
    ScopedPointer<TextButton> rythmButton;
    ScopedPointer<TextButton> beatButton;
    ScopedPointer<TextButton> resetButton;
    ScopedPointer<Slider> slider;
    int fftSizer;
    int order;
    bool colorchange = true;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};



#endif /* defined(__OnsetReal__MainComponent__) */
