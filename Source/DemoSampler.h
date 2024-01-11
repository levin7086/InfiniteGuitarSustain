/*
  ==============================================================================

    DemoSampler.h
    Created: 11 Jan 2024 12:42:42pm
    Author:  Levin
    Notes: Heavily based on https://docs.juce.com/master/tutorial_playing_sound_files.html
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

enum TransportState
{
    STOPPED,
    STARTING,
    PLAYING,
    STOPPING
};

class DemoSampler  : public juce::Component, juce::ChangeListener
{
public:
    DemoSampler() : state(STOPPED)
    {
        addAndMakeVisible(playDemoSampleLabel);
        playDemoSampleLabel.setText("No guitar at hand? Choose a file instead!", juce::NotificationType::dontSendNotification);
        playDemoSampleLabel.setColour(juce::Label::textColourId, textColor);
        addAndMakeVisible(chooseFileBtn);
        chooseFileBtn.setButtonText("Open file");
        chooseFileBtn.onClick = [this] { chooseFileBtnClicked(); };
        chooseFileBtn.setColour(juce::TextButton::buttonColourId, btnBg);
        chooseFileBtn.setColour(juce::TextButton::textColourOnId, textColor);
        chooseFileBtn.setColour(juce::TextButton::buttonOnColourId, btnPressed);
        addAndMakeVisible(playDemoSampleBtn);
        playDemoSampleBtn.setButtonText("Play the guitar for me");
        playDemoSampleBtn.onClick = [this] { playButtonClicked(); };
        playDemoSampleBtn.setEnabled(false);
        playDemoSampleBtn.setColour(juce::TextButton::buttonColourId, btnBg);
        playDemoSampleBtn.setColour(juce::TextButton::textColourOnId, textColor);
        playDemoSampleBtn.setColour(juce::TextButton::buttonOnColourId, btnPressed);
        
        formatManager.registerBasicFormats();
        transportSource.addChangeListener(this);
    }

    ~DemoSampler() override
    {
        transportSource.releaseResources();
    }

    void paint (juce::Graphics& g) override
    {
        
    }

    void resized() override
    {
        auto area = getLocalBounds();
        playDemoSampleLabel.setBounds(area.removeFromTop(20));
        chooseFileBtn.setBounds(area.removeFromTop(30));
        playDemoSampleBtn.setBounds(area.removeFromTop(30));
    }
    
    void chooseFileBtnClicked()
    {
        chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                               juce::File{},
                                                               "*.wav");
                auto chooserFlags = juce::FileBrowserComponent::openMode
                                  | juce::FileBrowserComponent::canSelectFiles;
                chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
                {
                    auto file = fc.getResult();
                    if (file != juce::File{})
                    {
                        auto* reader = formatManager.createReaderFor (file);
                        if (reader != nullptr)
                        {
                            auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                            playDemoSampleBtn.setEnabled (true);
                            readerSource.reset (newSource.release());
                        }
                    }
                });
    }

    void playButtonClicked()
    {
        if (state == PLAYING)
        {
            changeState(STOPPING);
            return;
        }
        if (state == STOPPED)
        {
            changeState(STARTING);
            return;
        }
    }
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override
        {
            if (source == &transportSource)
            {
                if (transportSource.isPlaying())
                    changeState (PLAYING);
                else
                    changeState (STOPPED);
            }
        }
    
    void changeState (TransportState newState)
    {
        if (state != newState)
        {
            state = newState;
            switch (state)
            {
                case STOPPED:
                    transportSource.setPosition (0.0);
                    playDemoSampleBtn.setButtonText("Play the guitar for me");
                    break;
                case STARTING:
                    playDemoSampleBtn.setEnabled (false);
                    transportSource.start();
                    break;
                case PLAYING:
                    playDemoSampleBtn.setEnabled(true);
                    playDemoSampleBtn.setButtonText("Stop the guitar for me");
                    break;
                case STOPPING:
                    transportSource.stop();
                    break;
            }
        }
    }
    
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    TransportState state;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoSampler)
    
    juce::Label playDemoSampleLabel;
    juce::TextButton chooseFileBtn;
    juce::TextButton playDemoSampleBtn;
    
    juce::Colour textColor { 0xFFFFFFF2 };
    juce::Colour btnPressed { 0xFF2F353B};
    juce::Colour btnBg { 0xFF5A626D};
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::FileChooser> chooser;
};

