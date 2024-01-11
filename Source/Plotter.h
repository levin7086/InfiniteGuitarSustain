/*
  ==============================================================================

    Plotter.h
    Created: 29 Nov 2023 12:42:15pm
    Author:  Levin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Plotter  : public juce::Component, juce::Timer
{
public:
    Plotter(int channelAmount)
    {
        averages = juce::Array<float>({0}, channelAmount);
        frameIdxs = juce::Array<int>({0}, channelAmount);
        plotChannels = juce::OwnedArray<juce::Array<float>>();
        for (int i = 0; i < channelAmount; ++i)
        {
            plotChannels.add(new juce::Array<float>({0}, bufferSize));
             plotChannels[i]->resize(bufferSize);
        }
        startTimerHz(40);
    }

    ~Plotter() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(bgColor);
        
        for (int ch = 0; ch < plotChannels.size(); ch++)
        {
            if(plotChannels[ch] == nullptr)
            {
                break;
            }
            juce::Array<float> plotData = *plotChannels[ch];
            g.setColour(colors[ch]);
            for (int i = 1; i < plotData.size(); ++i)
            {
                int idx = i;
                int lastIdx = i - 1;
 
                float x1 = lastIdx * getWidth() / (plotData.size() - 1);
                float y1 = getHeight() - Y_OFFSET - plotData[lastIdx] * getHeight();
                float x2 = idx * getWidth() / (plotData.size() - 1);
                float y2 = getHeight() - Y_OFFSET - plotData[idx] * getHeight() ;
                
                g.drawLine(x1, y1, x2, y2, 1.0f);
            }
        }
    }

    void resized() override
    {
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
    void pushBuffer(float * buffer, int bufferLength, int channel)
    {
        plotChannels[channel]->clearQuick();
        plotChannels[channel]->addArray(buffer, bufferLength);
    }
    
    void pushSample(float sample, int channel)
    {
        plotChannels[channel]->add(sample);
        if (plotChannels[channel]->size() > bufferSize)
        {
            plotChannels[channel]->remove(0);
        }
    }
    
    void pushSampleAndFrame(float sample, int channel)
    {
        float avg = averages[channel];
        avg += sample;
        averages.set(channel, avg);
        frameIdxs.set(channel, frameIdxs[channel] + 1);
        if (frameIdxs[channel] == frameSize)
        {

            plotChannels[channel]->add(avg / frameSize);

            if (plotChannels[channel]->size() > bufferSize)
            {
                plotChannels[channel]->remove(0);
            }
            averages.set(channel, 0);
            frameIdxs.set(channel, 0);
        }
    }
    
    int getFrameSize()
    {
        return frameSize;
    }
    
private:
    juce::Random random;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Plotter)
    juce::OwnedArray<juce::Array<float>> plotChannels;
    int bufferSize = 1024;
    juce::Array<int> frameIdxs = 0;
    const int frameSize = 128;
    const float Y_OFFSET = 100;
    juce::Array<float> averages;
    juce::Colour bgColor { 0xFFE8F1F5 };
    juce::Array<juce::Colour> colors = juce::Array<juce::Colour>({
        juce::Colour(0xFF3a434d),
        juce::Colour(0xFF957DAD),
        juce::Colour(0xFF3B9A9C),
        juce::Colour(0xFFFF6F61)
    });
};
