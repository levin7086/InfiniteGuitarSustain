/*
  ==============================================================================

    Looper.h
    Created: 4 Dec 2023 2:10:17pm
    Author:  Levin

  ==============================================================================
*/

#pragma once

class Looper
{
public:
    Looper (int channelAmount)
    {
        channels = juce::OwnedArray<juce::Array<float>>();
        this->channelAmount = channelAmount;
    }
    
    void initialize(float samplerate)
    {
        fadeInDuration = fadeInDurationSec * samplerate;
        fadeOutDuration = fadeOutDurationSec * samplerate;
    }
    
    void process(float sample, int loopLength, bool loopEnabled)
    {
        pastSamples.add(sample);
        if (pastSamples.size() == lookback)
        {
            pastSamples.remove(0);
        }
        if (loopLength > 0 && pastSamples.size() >= loopLength && loopEnabled)
        {
            float * loop = new float[loopLength];
            int iZeroCross = findZeroCrossing(loopLength);
            std::copy(pastSamples.begin() + iZeroCross, pastSamples.begin() + iZeroCross + loopLength, loop);
            loopChannel(loop, loopLength);
        }
    }
    
    void loopChannel(float * data, int size)
    {
        channels.add(new juce::Array<float>(data, size));
        channelPositions.add(0);
        fadeInPositions.add(0);
        if (channels.size() > channelAmount)
        {
            channels.remove(0);
            channelPositions.remove(0);
            fadeInPositions.remove(0);
        }
    }
    
    float nextSample(bool loopEnabled)
    {
        float sample = 0;
        if (!channels.isEmpty())
        {
            for (int ch = 0; ch < channels.size(); ch++)
            {
                int i = channelPositions[ch];
                int fadeInPos = fadeInPositions[ch];
                float fadeInAmp = ((float) fadeInPos / (float) fadeInDuration);
                int fadeOut = !loopEnabled * fadeOutPosition;
                float fadeOutAmp = ((float) fadeOutDuration - fadeOut) / (float) fadeOutDuration;
                sample += fadeOutAmp * fadeInAmp * (*channels[ch])[i];
                channelPositions.set(ch, ++i);
                if (i == channels[ch]->size())
                {
                    channelPositions.set(ch, 0);
                }
                if (fadeInPos < fadeInDuration)
                {
                    fadeInPositions.set(ch, fadeInPos + 1);
                }
            }
            if (fadeOutPosition == fadeOutDuration && !loopEnabled)
            {
                channels.clear();
                fadeOutPosition = 0;
            } else if (!loopEnabled)
            {
                fadeOutPosition++;
            }
            return sample;
        }
        return sample;
    }
    
private:
    juce::OwnedArray<juce::Array<float>> channels;
    int channelAmount;
    juce::Array<int> channelPositions;
    juce::Array<float> pastSamples;
    int lookback = 1024;
    int nextChannel = 0;
    float fadeInDurationSec = 0.4;
    int fadeInDuration;
    juce::Array<int> fadeInPositions;
    float fadeOutDurationSec = 0.7;
    int fadeOutDuration;
    int fadeOutPosition = 0;
    static constexpr float ZERO_DEVIATION = 0.0005;
    
    int findZeroCrossing (int loopLength)
    {
        for (int i = pastSamples.size() - loopLength; i >= 0; i--)
        {
            if (pastSamples[i] >= 0 - ZERO_DEVIATION && pastSamples[i] <= 0 + ZERO_DEVIATION)
            {
                return i;
            }
        }
        return pastSamples.size() - loopLength;
    }
};
