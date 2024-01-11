#pragma once

#include <JuceHeader.h>
#include "Plotter.h"
#include "Inos2Odf.h"
#include "PeriodDetect.h"
#include "Looper.h"
#include "GUILayout.h"
#include "PostProcessor.h"
#include "DemoSampler.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    //==============================================================================
    // Your private member variables go here...
    
    // Algorithms
    Inos2 inos2;
    PeriodDetector periodDtct;
    float lastDetectedPeriod = -1;
    Looper looper;
    
    // UI Components
    GUIHeader header;
    GUIInosSection inosSection;
    GUIPeakPickingSection peakPickingSection;
    GUIPeriodDetectionSection periodDetectionSection;
    
    GUISeperator leftSeperator;
    GUISeperator rightSeperator;
    GUISeperator bottomSeperator;
    GUISeperator seperator1;
    GUISeperator seperator2;
    GUISeperator seperator3;
    
    Plotter plot = Plotter(4);
    
    void updateParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
