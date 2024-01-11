#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : looper(30)
{
    addAndMakeVisible(header);
    addAndMakeVisible(inosSection);
    addAndMakeVisible(peakPickingSection);
    addAndMakeVisible(periodDetectionSection);
    
    addAndMakeVisible(leftSeperator);
    addAndMakeVisible(rightSeperator);
    addAndMakeVisible(bottomSeperator);
    addAndMakeVisible(seperator1);
    addAndMakeVisible(seperator2);
    addAndMakeVisible(seperator3);

    addAndMakeVisible(plot);
    
    setSize (1024, 800);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (1, 1);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

    inos2.initialize(sampleRate);
    periodDtct.initialize(sampleRate);
    looper.initialize(sampleRate);
    header.demoSampler.transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels  = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels  = activeInputChannels .getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
    
    updateParameters();
    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if ((! activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels;
            
            if (! activeInputChannels[channel])
            {
                bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else // [3]
            {
                
                auto* inBuffer = bufferToFill.buffer->getReadPointer (actualInputChannel,
                                                                      bufferToFill.startSample);
                
                if (header.demoSampler.state == PLAYING && header.demoSampler.readerSource.get() != nullptr)
                {
                    juce::AudioSourceChannelInfo tempInfo;
                    tempInfo.buffer = bufferToFill.buffer;
                    tempInfo.startSample = bufferToFill.startSample;
                    tempInfo.numSamples = bufferToFill.numSamples;

                    header.demoSampler.readerSource->getNextAudioBlock(tempInfo);
                    inBuffer = tempInfo.buffer->getReadPointer(0);
                }
                
                auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
                
                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    plot.pushSampleAndFrame(inBuffer[sample], 0);
                    bool onset = inos2.processSample(inBuffer[sample]);
                    plot.pushSampleAndFrame(inos2.getLatesInos2Val(), 1);
                    plot.pushSampleAndFrame(onset * plot.getFrameSize(), 2);
                    
                    float period = periodDtct.processSample(inBuffer[sample], onset);
                    if (period != -1)
                    {
                        lastDetectedPeriod = period;
                    }
                    looper.process(inBuffer[sample], period, header.sustainSwitch.getToggleState());
                    outBuffer[sample] = inBuffer[sample] + looper.nextSample(header.sustainSwitch.getToggleState());
                }
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    header.setBounds (area.removeFromTop(80));
    bottomSeperator.setBounds(area.removeFromBottom(50));
    leftSeperator.setBounds(area.removeFromLeft(7));
    rightSeperator.setBounds(area.removeFromRight(7));
    inosSection.setBounds(area.removeFromTop(100));
    seperator1.setBounds(area.removeFromTop(10));
    peakPickingSection.setBounds(area.removeFromTop(100));
    seperator2.setBounds(area.removeFromTop(10));
    periodDetectionSection.setBounds(area.removeFromTop(100));
    seperator3.setBounds(area.removeFromTop(10));
    plot.setBounds(area.removeFromTop(400));
}

void MainComponent::updateParameters()
{
    inos2.setJValue(inosSection.jSlider.getValue());
    inos2.setCompression(inosSection.magCompSlider.getValue());
    inos2.setRequiredOffset(peakPickingSection.minMeanOffsetSlider.getValue());
    inos2.setMaxInosLookback(peakPickingSection.maxInosLookbackSlider.getValue());
    inos2.setMeanInosLookback(peakPickingSection.meanInosLookbackSlider.getValue());
    inos2.setMinOnsetDist(peakPickingSection.minOnsetDistSlider.getValue());
    
    periodDtct.setSustainOffset(periodDetectionSection.sustainOffsetSlider.getValue());
    periodDtct.setSustainLength(periodDetectionSection.sustainLengthSlider.getValue());
}
