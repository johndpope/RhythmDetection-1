

#include "MainComponent.h"


MainComponent::MainComponent()
{
    addAndMakeVisible (title = new Label (String::empty, "Rhythm Detector"));
    title->setFont (Font (20.00f, Font::plain));
    title->setJustificationType (Justification::topLeft);
    title->setEditable (false, false, false);
    title->setColour (TextEditor::textColourId, Colours::black);
    title->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    addAndMakeVisible (state = new Label (String::empty, "Input: audio File"));
    state->setFont (Font (20.00f, Font::plain));
    state->setJustificationType (Justification::topLeft);
    state->setEditable (false, false, false);
    state->setColour (TextEditor::textColourId, Colours::black);
    state->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    addAndMakeVisible(loadButton = new TextButton(String::empty));
    loadButton->setButtonText("Load File");
    loadButton->addListener(this);
    loadButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    loadButton->setColour (TextButton::textColourOnId, Colours::black);
    
    addAndMakeVisible(slider = new Slider());
    slider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 220, 30);
    slider->setColour(Slider::thumbColourId, Colours::blue);
    slider->setColour(Slider::trackColourId, Colours::lightblue);
    slider->setColour(Slider::textBoxTextColourId, Colours::black);
    slider->setColour(Slider::textBoxBackgroundColourId, Colours::lightgrey);
    slider->setRange(100, 300);
    slider->addListener(this);
    
    addAndMakeVisible(audioInputButton = new TextButton(String::empty));
    audioInputButton->setButtonText("switch input");
    audioInputButton->addListener(this);
    audioInputButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    audioInputButton->setColour (TextButton::textColourOnId, Colours::black);
    
    addAndMakeVisible(dofButton = new TextButton(String::empty));
    dofButton->addListener(this);
    dofButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    
    addAndMakeVisible(rythmButton = new TextButton(String::empty));
    rythmButton->addListener(this);
    rythmButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    
    addAndMakeVisible(beatButton = new TextButton(String::empty));
    beatButton->addListener(this);
    beatButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    
    addAndMakeVisible(resetButton = new TextButton(String::empty));
    resetButton -> setButtonText("Reset");
    resetButton->addListener(this);
    resetButton->setColour (TextButton::buttonColourId, Colours::lightblue);
    
    addAndMakeVisible(tempoValueLabel = new Label(String::empty, "0"));
    tempoValueLabel->setFont (Font (28.0000f, Font::bold));
    tempoValueLabel->setJustificationType (Justification::centred);
    tempoValueLabel->setEditable (false, false, false);
    tempoValueLabel->setColour (Label::textColourId, Colours::lightgrey);
    tempoValueLabel->setColour(Label::backgroundColourId, Colours::black);
    
    addAndMakeVisible(rhythmValueLabel = new Label(String::empty, ""));
    rhythmValueLabel->setFont (Font (28.0000f, Font::bold));
    rhythmValueLabel->setJustificationType (Justification::centred);
    rhythmValueLabel->setEditable (false, false, false);
    rhythmValueLabel->setColour (Label::textColourId, Colours::lightgrey);
    rhythmValueLabel->setColour(Label::backgroundColourId, Colours::black);
    
    setSize(400, 600);
    
    // audio stuff
    deviceManager.initialise(2, 3, 0, true,String::empty,0);
    currentFile = File(String::empty);
    
    startTimer(10);
 
}

MainComponent::~MainComponent()
{
    
    title = nullptr;
    loadButton = nullptr;
    audioInputButton = nullptr;
    dofButton = nullptr;
    rythmButton = nullptr;
    state = nullptr;
    slider = nullptr;
    tempoValueLabel = nullptr;
    rhythmValueLabel = nullptr;
    beatButton = nullptr;
    resetButton = nullptr;
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::lightgrey);
}

void MainComponent::resized()
{
    title->setBounds (getWidth()/3-10, 20, 400, 32);
    beatButton -> setBounds(getWidth()/4-60, getHeight()/4 - 90, 50, 50);
    state -> setBounds (getWidth()/4+20, getHeight()/4-80, 200, 32);
    rythmButton -> setBounds(getWidth()/4+200, getHeight()/4 - 90, 50, 50);
    tempoValueLabel -> setBounds(getWidth()/4-60, getHeight()/4-20, 60, 80);
    rhythmValueLabel -> setBounds(getWidth()/4+4, getHeight()/4-20, 260, 80);
    slider->setBounds(getWidth()/4, getHeight()/4+70, 200, 40);
    loadButton -> setBounds(getWidth()/3, getHeight()/3+90, 120, 32);
    audioInputButton -> setBounds(getWidth()/3, getHeight()/3+140, 120, 32);
    dofButton -> setBounds(getWidth()/2-40, getHeight()/2+120, 80, 80);
    resetButton -> setBounds(getWidth()-150, getHeight()-40, 100, 20);
    
    internalPath1.clear();
    internalPath1.startNewSubPath (136.0f, 80.0f);
    internalPath1.quadraticTo (176.0f, 24.0f, 328.0f, 32.0f);
    internalPath1.quadraticTo (472.0f, 40.0f, 472.0f, 104.0f);
    internalPath1.quadraticTo (472.0f, 192.0f, 232.0f, 176.0f);
    internalPath1.lineTo (184.0f, 216.0f);
    internalPath1.lineTo (200.0f, 168.0f);
    internalPath1.quadraticTo (96.0f, 136.0f, 136.0f, 80.0f);
    internalPath1.closeSubPath();
    
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainComponent::buttonClicked(Button* buttonthatWasClicked)
{
    if (buttonthatWasClicked == audioInputButton)
    {
        if(streaming)   //current iusing live input
        {
            streaming = nullptr;
            
            if(currentFile.exists())
            {
                player = new AudioFileSource(deviceManager);
                player->stopPlaying();
                player->setFile(currentFile);
                std::cout<< "load file: "<<currentFile.getFileName()<<std::endl;
            }
            state->setText("Input: Audio File", dontSendNotification);
            
        }
        else
        {
            player = nullptr;
            streaming = new LiveInputSource(deviceManager);
            state->setText("Input: live input", dontSendNotification);
        }
    }
    
    if(buttonthatWasClicked == resetButton)
    {
        if (streaming)
            streaming->cr->reset();
    }
    
    if (buttonthatWasClicked == loadButton)
    {
        FileChooser chooser (("Choose .eft file to open"),File::nonexistent,"*",true);
        chooser.browseForFileToOpen();
       // if (chooser.getResult().exists())
        
        currentFile = File(chooser.getResult());
        
        if (!player && !streaming)
        {
            player = new AudioFileSource(deviceManager);
            player->setFile(currentFile);
            std::cout<< "load file: "<<currentFile.getFileName()<<std::endl;
        }
        else if(player)
        {
            player->stopPlaying();
            player->setFile(currentFile);
            std::cout<< "load file: "<<currentFile.getFileName()<<std::endl;
        }
    }
}

void MainComponent::sliderValueChanged(Slider* sliderThatWasMoved)
{
    if (streaming)
        streaming->detectionTrial->onsetTh = (int)(sliderThatWasMoved->getValue());
    else if (player)
        player->detectionTrail->onsetTh = (int)(sliderThatWasMoved->getValue());
}

void MainComponent::timerCallback()
{
    if (streaming){
        if (streaming->existOnset == 1){
            if (colorchange){
                beatButton->setColour (TextButton::buttonColourId, Colours::orange);
                colorchange = false;
            }
            else{
                beatButton->setColour (TextButton::buttonColourId, Colours::blue);
                colorchange = true;
            }
        }
        
        if (streaming->cr->rhythmFound){
            state->setText("Rythm Found", dontSendNotification);
            state->setColour(Label::textColourId, Colours::darkblue);
            rythmButton->setColour(TextButton::buttonColourId, Colours::green);
        }
        else if (streaming->cr->rhythmChanged){
            state->setText("Rythm Changed", dontSendNotification);
            state->setColour(TextEditor::textColourId, Colours::darkblue);
            rythmButton->setColour(TextButton::buttonColourId, Colours::yellow);
        }
        else
            rythmButton->setColour(TextButton::buttonColourId, Colours::lightblue);
        
        if (streaming->cr->match)
            dofButton->setColour (TextButton::buttonColourId, Colours::red);
        else
            dofButton->setColour (TextButton::buttonColourId, Colours::lightblue);
        tempoValueLabel->setText((String)streaming->cr->tempo, NotificationType::sendNotification);
        rhythmValueLabel->setText(streaming->cr->rhythm, NotificationType::sendNotification);
    }
}

void MainComponent::visibilityChanged()
{
    
}
