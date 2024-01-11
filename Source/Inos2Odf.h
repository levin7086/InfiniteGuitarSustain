/*
  ==============================================================================

    Inos2Odf.h
    Created: 5 Dec 2023 5:31:43pm
    Author:  Levin

  ==============================================================================
*/

#pragma once

class Inos2
{
public:
    Inos2() : hann(WINDOW_SIZE, juce::dsp::WindowingFunction<float>::WindowingMethod::hann), fft (FFT_ORDER)
    {
    }
    
    void initialize(float samplerate)
    {
        minOnsetDist = minOnsetDistSec * samplerate;
        lookbackMax = lookbackMaxSec * samplerate;
        lookbackMean = lookbackMeanSec * samplerate;
        lookbackSize = std::max(lookbackMax, lookbackMean);
        lookbackInos.resize(lookbackSize);
        this->samplerate = samplerate;
    }
    
    bool processSample(float sample)
    {
        bool peak = false;
        if (pushSampleToWindow(sample))
        {
            performFFT();
            float inos2 = calcInos2();
            // Instead of adding hop amount of samples, I could also create a smaller buffer / lower samplig rate so that 1 sample in this buffer equals 128 samples in the normal 
            peak = isPeak(inos2);
            for (int h = 0; h < hop; h++)
            {
                pushInosVal(inos2);
            }
        }
        samplesSinceOnset++;
        return peak;
    }
    
    bool pushSampleToWindow(float sample)
    {
        window.set(windowIdx, sample);
        windowIdx++;
        if (windowIdx == WINDOW_SIZE)
        {
            std::fill (fftData.begin(), fftData.end(), 0.0f);
            std::copy (window.begin(), window.end(), fftData.begin());
            hann.multiplyWithWindowingTable(fftData.begin(), WINDOW_SIZE);
            window.removeRange(0, hop);
            windowIdx = overlap - 1;
            return true;
        }
        return false;
    }
    
    float getLatesInos2Val()
    {
        return lookbackInos.getLast();
    }
    
    void setRequiredOffset(float offset)
    {
        meanOffset = offset;
    }
    
    void setJValue (float val)
    {
        j = (WINDOW_SIZE - 1) * val;
        normCoef = 1 / pow(j, 1/4);
    }
    
    void setCompression (float val)
    {
        compression = val;
    }
    
    void setMaxInosLookback (float seconds)
    {
        lookbackMax = seconds * samplerate;
        lookbackSize = std::max(lookbackMax, lookbackSize);
    }
    
    void setMeanInosLookback (float seconds)
    {
        lookbackMean = seconds * samplerate;
        lookbackSize = std::max(lookbackMean, lookbackSize);
    }
    
    void setMinOnsetDist (float seconds)
    {
        minOnsetDist = seconds * samplerate;
    }
    
private:
    static constexpr int FFT_ORDER = 10;
    static constexpr int WINDOW_SIZE = 1 << FFT_ORDER; // = 2^fftOrder samples
    static constexpr int FFT_SIZE = WINDOW_SIZE * 2;
    int j = (WINDOW_SIZE - 1) * 0.955;
    
    // Windowing
    juce::Array<float> window;
    int windowIdx = 0;
    const int hop = 64;
    const int overlap = WINDOW_SIZE - hop;
    juce::dsp::WindowingFunction<float> hann;
    
    //FFT
    juce::dsp::FFT fft;
    std::array<float, FFT_SIZE> fftData;
    float compression = 1;
    
    //INOS2
    float normCoef = 1 / pow(j, 1/4); // ~= 1 / 4th root of J
    // - Peak picking
    juce::Array<float> lookbackInos;
    float lookbackMaxSec = 0.03;
    float lookbackMeanSec = 0.1;
    int lookbackMax = 0;
    int lookbackMean = 0;
    float meanOffset = 0.1;
    int lookbackSize = 0;
    float minOnsetDistSec = 0.2;
    int minOnsetDist = 0;
    int samplesSinceOnset = 0;
    
    float samplerate;
    
    void performFFT()
    {
        fft.performFrequencyOnlyForwardTransform(fftData.data());
        
        for (int i = 0; i < WINDOW_SIZE; i++)
        {
            std::complex<float> c;
            c.real(fftData[i * 2]);
            c.imag(fftData[i * 2 + 1]);
            float mag = std::abs(c);
            mag = std::log(compression * mag + 1);
            fftData[i] = mag;
        }
    }
    
    float calcInos2()
    {
        std::sort(fftData.begin(), fftData.begin() + WINDOW_SIZE);
        float norm2Squ = 0;
        float norm4 = 0;
        
        for (int i = 0; i < j; i++)
        {
            norm2Squ += fftData[i] * fftData[i];
            norm4 += pow(fftData[i], 4);
        }
        norm4 = pow(norm4, 1/4);
        return norm2Squ / norm4 * normCoef;
    }
   
    bool isPeak(float inos2)
    {
        if (isMax(inos2) && isAboveMean(inos2) && isAboveMinOnsetDist())
        {
            samplesSinceOnset = 0;
            return true;
        }
        return false;
    }
    
    bool isMax(float inos2)
    {
        float lastMax = 0;
        for (int i = 0; i < lookbackMax; i++)
        {
            lastMax = (lookbackInos[lookbackSize - i] > lastMax) ? lookbackInos[lookbackSize - i] : lastMax;
        }
        return std::max(inos2, lastMax) == inos2;
    }
    
    bool isAboveMean(float inos2)
    {
        float mean = 0;
        for (int i = 0; i < lookbackMean; i++)
        {
            mean += lookbackInos[lookbackSize - i];
        }
        mean /= lookbackMean;
        return inos2 >= mean + meanOffset;
    }
    
    bool isAboveMinOnsetDist()
    {
        return samplesSinceOnset >= minOnsetDist;
    }
    
    void pushInosVal(float inos2)
    {
        lookbackInos.add(inos2);
        if (lookbackInos.size() >= lookbackSize)
        {
            lookbackInos.remove(0);
        }
    }
    
};
