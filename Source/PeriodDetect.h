/*
  ==============================================================================

    PeriodDetect.h
    Created: 4 Dec 2023 8:01:57pm
    Author:  Levin

  ==============================================================================
*/

#pragma once

class PeriodDetector
{
public:
    void initialize(float samplerate)
    {
        windowSize = 1024;
        sustainLength = sustainLengthSec * samplerate;
        sustainOffset = sustainOffsetSec * samplerate;
        lpFilter = juce::dsp::FIR::Filter<float>(juce::dsp::FilterDesign<float>::designFIRLowpassKaiserMethod(1400, samplerate, 0.1, -10));
        this->samplerate = samplerate;
    }
    
    float processSample(float sample, bool onset)
    {
        if (onset)
        {
            lastOnsetCounter = 0;
        }
        if (lastOnsetCounter > sustainOffset && lastOnsetCounter <= sustainLength + sustainOffset)
        {
            window.set(windowIdx, sample);
            windowIdx++;
            if (windowIdx == windowSize)
            {
                windowIdx = 0;
                juce::Array<float> Dt;
                for (int t = 0; t < tMax - tMin; t++)
                {
                    float dt = 0;
                    for (int i = 1; i < windowSize; i++)
                    {
                        int lag = i - t + tMin;
                        if (lag < 0 || lag >= windowSize)
                        {
                            continue;
                        }
                        dt += abs(window[i] - window[lag]);
                    }
                    dt /= windowSize;
                    Dt.add(dt);
                }
                
                window.clearQuick();
                int tMin1 = findLocalMin(tMin + 1, Dt.begin(), Dt.size(), 0.005);
                if (tMin1 == -1)
                {
                    return -1;
                }
                return pushPeriodEstimate(tMin1 - tMin);
            }
        }
        lastOnsetCounter = (lastOnsetCounter <= sustainLength && lastOnsetCounter != -1) ? (lastOnsetCounter + 1) : -1;
        return -1;
    }
    
    void setSustainOffset (float seconds)
    {
        sustainOffset = seconds * samplerate;
    }
    
    void setSustainLength (float seconds)
    {
        sustainLength = seconds * samplerate;
    }
    
private:
    juce::Array<float> window;
    int windowIdx = 0;
    int windowSize = 0;
    static constexpr int PERIOD_LOOKBACK = 4;
    juce::Array<float> pastPeriods;

    int tMin = 32;
    int tMax = 700;
    
    int lastOnsetCounter = -1;
    float sustainLengthSec = 1.3;
    int sustainLength = 0;
    float sustainOffsetSec = 0.1;
    int sustainOffset;
    float samplerate;
    
    juce::dsp::FIR::Filter<float> lpFilter;
    
    float pushPeriodEstimate(float p)
    {
        pastPeriods.add(p);
        if (pastPeriods.size() == PERIOD_LOOKBACK)
        {
            float mean = 0;
            for (int i = 0; i < pastPeriods.size(); i++)
            {
                mean += pastPeriods[i];
            }
            mean /= 4;
            float totalAbsDeviation = 0;
            for (int i = 0; i < pastPeriods.size(); i++)
            {
                totalAbsDeviation += abs(pastPeriods[i] - mean);
            }
            totalAbsDeviation /= 4;
            pastPeriods.clearQuick();
            if (totalAbsDeviation < 0.1)
            {
                lastOnsetCounter = -1;
                return mean;
            }
        }
        return -1;
    }
    
    int findLocalMin (int start, float * data, int dataSize, float tresh)
    {
        for (int i = start; i < dataSize - 2; i++)
        {
            if (data[i] < data[i - 1] && data[i] < data[i + 1] && data[i] < tresh) {
                return i;
            }
        }
        return -1;
    }
};
