/*
  ==============================================================================

    GUI.h
    Created: 6 Jan 2024 7:28:46pm
    Author:  Levin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DemoSampler.h"

#define CORNER_RADIUS 10.0f

static const juce::Colour BASE_COLOR { 0xFF3a434d };

class GUISeperator : public juce::Component
{
public:
    GUISeperator()
    {
    }
    
    ~GUISeperator() override
    {}

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BASE_COLOR);
    }
    
    void resized() override
    {
        
    }
private:
    
};

class GUIHeader : public juce::Component
{
public:
    GUIHeader()
    {
        addAndMakeVisible(sustainSwitch);
        sustainSwitch.setButtonText("Toggle sustain");
        sustainSwitch.setColour(juce::ToggleButton::textColourId, textColor);
        sustainSwitch.setColour(juce::ToggleButton::tickColourId, btnTickColor);
        sustainSwitch.setColour(juce::ToggleButton::tickDisabledColourId, btnTickColor);
        addAndMakeVisible(demoSampler);
    }
    
    ~GUIHeader() override
    {}

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BASE_COLOR);
        g.setColour(textColor);
        g.setFont(juce::Font("Roboto", 32.0f, juce::Font::bold));
        auto textRect = getLocalBounds().withLeft(10);
        g.drawText("infinite sustain . . . . . . . . . .", textRect, juce::Justification::left, true);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        area.removeFromRight(100);
        demoSampler.setBounds(area.removeFromRight(200));
        area.removeFromRight(100);
        sustainSwitch.setBounds(area.removeFromRight(100));
    }
    
    juce::ToggleButton sustainSwitch;
    DemoSampler demoSampler;
private:
    juce::Colour textColor { 0xFFFFFFF2 };
    juce::Colour btnTickColor { 0xFF7A828D };
};

class GUIInosSection : public juce::Component
{
public:
    GUIInosSection()
    {
        addAndMakeVisible(inos2SectionLabel);
        inos2SectionLabel.setText("INOS2 Config", juce::NotificationType::dontSendNotification);
        inos2SectionLabel.setColour(juce::Label::textColourId, titleColor);
        inos2SectionLabel.setFont(juce::Font("Lato", 21.0f, juce::Font::bold));
        
        addAndMakeVisible(jSlider);
        jSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        jSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        jSlider.setRange(0.01, 0.99);
        jSlider.setValue(0.955);
        addAndMakeVisible(jLabel);
        jLabel.setText(" J value", juce::NotificationType::dontSendNotification);
        jLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::plain));
        jLabel.setColour(juce::Label::textColourId, textColor);
        
        addAndMakeVisible(magCompSlider);
        magCompSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        magCompSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        magCompSlider.setRange(0.2, 2);
        magCompSlider.setValue(1);
        addAndMakeVisible(magCompLabel);
        magCompLabel.setText("Maginute compression", juce::NotificationType::dontSendNotification);
        magCompLabel.setColour(juce::Label::textColourId, textColor);
        magCompLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::bold));
    }
    
    ~GUIInosSection() override
    {}

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BASE_COLOR);
        g.setColour(bgColor);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), CORNER_RADIUS);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        area.removeFromLeft(10);
        inos2SectionLabel.setBounds(area.removeFromTop(40));
        auto jArea = area.removeFromLeft(200);
        jLabel.setBounds(jArea.removeFromTop(20));
        jSlider.setBounds(jArea.removeFromTop(20));
        area.removeFromLeft(10);
        auto magCompArea = area.removeFromLeft(200);
        magCompLabel.setBounds(magCompArea.removeFromTop(20));
        magCompSlider.setBounds(magCompArea.removeFromTop(20));
    }

    juce::Colour bgColor { 0xFFE6E6FA };
    juce::Colour titleColor { 0xFFB9A3EB };
    juce::Colour sliderColor { 0xFF967BB6 };
    juce::Colour textColor { 0xFF404040};
    juce::Label inos2SectionLabel;
    juce::Slider jSlider;
    juce::Label jLabel;
    juce::Slider magCompSlider;
    juce::Label magCompLabel;
};

class GUIPeakPickingSection : public juce::Component
{
public:
    GUIPeakPickingSection()
    {
        addAndMakeVisible(peakPickSectionLabel);
        peakPickSectionLabel.setText("Peak-picking Config", juce::NotificationType::dontSendNotification);
        peakPickSectionLabel.setColour(juce::Label::textColourId, titleColor);
        peakPickSectionLabel.setFont(juce::Font("Lato", 21.0f, juce::Font::bold));
        
        addAndMakeVisible(minMeanOffsetSlider);
        minMeanOffsetSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        minMeanOffsetSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        minMeanOffsetSlider.setRange(0.01, 1.5);
        minMeanOffsetSlider.setValue(0.1);
        addAndMakeVisible(minMeanOffsetLabel);
        minMeanOffsetLabel.setText("Min. offset from mean", juce::NotificationType::dontSendNotification);
        minMeanOffsetLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::plain));
        minMeanOffsetLabel.setColour(juce::Label::textColourId, textColor);
        
        addAndMakeVisible(maxInosLookbackSlider);
        maxInosLookbackSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        maxInosLookbackSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        maxInosLookbackSlider.setRange(0.01, 2);
        maxInosLookbackSlider.setValue(0.03);
        addAndMakeVisible(maxInosLookbackLabel);
        maxInosLookbackLabel.setText("Lookback (Max) [seconds]", juce::NotificationType::dontSendNotification);
        maxInosLookbackLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::plain));
        maxInosLookbackLabel.setColour(juce::Label::textColourId, textColor);
        
        addAndMakeVisible(meanInosLookbackSlider);
        meanInosLookbackSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        meanInosLookbackSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        meanInosLookbackSlider.setRange(0.01, 2);
        meanInosLookbackSlider.setValue(0.1);
        addAndMakeVisible(meanInosLookbackLabel);
        meanInosLookbackLabel.setText("Lookback (Mean) [seconds]", juce::NotificationType::dontSendNotification);
        meanInosLookbackLabel.setColour(juce::Label::textColourId, textColor);
        meanInosLookbackLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::bold));
        
        addAndMakeVisible(minOnsetDistSlider);
        minOnsetDistSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        minOnsetDistSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        minOnsetDistSlider.setRange(0.1, 2);
        minOnsetDistSlider.setValue(0.2);
        addAndMakeVisible(minOnsetDistLabel);
        minOnsetDistLabel.setText("Min. onset distance [seconds]", juce::NotificationType::dontSendNotification);
        minOnsetDistLabel.setColour(juce::Label::textColourId, textColor);
        minOnsetDistLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::bold));
    }
    
    ~GUIPeakPickingSection() override
    {}

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BASE_COLOR);
        g.setColour(bgColor);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), CORNER_RADIUS);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        area.removeFromLeft(10);
        peakPickSectionLabel.setBounds(area.removeFromTop(40));
        auto minOffsetArea = area.removeFromLeft(200);
        minMeanOffsetLabel.setBounds(minOffsetArea.removeFromTop(20));
        minMeanOffsetSlider.setBounds(minOffsetArea.removeFromTop(20));
        area.removeFromLeft(10);
        auto maxInosArea = area.removeFromLeft(200);
        maxInosLookbackLabel.setBounds(maxInosArea.removeFromTop(20));
        maxInosLookbackSlider.setBounds(maxInosArea.removeFromTop(20));
        area.removeFromLeft(10);
        auto meanInosArea = area.removeFromLeft(200);
        meanInosLookbackLabel.setBounds(meanInosArea.removeFromTop(20));
        meanInosLookbackSlider.setBounds(meanInosArea.removeFromTop(20));
        area.removeFromLeft(10);
        auto minDistArea = area.removeFromLeft(200);
        minOnsetDistLabel.setBounds(minDistArea.removeFromTop(20));
        minOnsetDistSlider.setBounds(minDistArea.removeFromTop(20));
    }

    juce::Colour bgColor { 0xFFA0D8C9 };
    juce::Colour titleColor { 0xFF5DA0A2 };
    juce::Colour sliderColor { 0xFF77B1A9 };
    juce::Colour textColor { 0xFF394D59};
    juce::Label peakPickSectionLabel;
    juce::Slider minMeanOffsetSlider;
    juce::Label minMeanOffsetLabel;
    juce::Slider maxInosLookbackSlider;
    juce::Label maxInosLookbackLabel;
    juce::Slider meanInosLookbackSlider;
    juce::Label meanInosLookbackLabel;
    juce::Slider minOnsetDistSlider;
    juce::Label minOnsetDistLabel;
};

class GUIPeriodDetectionSection : public juce::Component
{
public:
    GUIPeriodDetectionSection()
    {
        addAndMakeVisible(periodDtctSectionLabel);
        periodDtctSectionLabel.setText("Period detection config", juce::NotificationType::dontSendNotification);
        periodDtctSectionLabel.setColour(juce::Label::textColourId, titleColor);
        periodDtctSectionLabel.setFont(juce::Font("Lato", 21.0f, juce::Font::bold));
        
        addAndMakeVisible(sustainOffsetSlider);
        sustainOffsetSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        sustainOffsetSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        sustainOffsetSlider.setRange(0, 1);
        sustainOffsetSlider.setValue(0.1);
        addAndMakeVisible(sustainOffsetLabel);
        sustainOffsetLabel.setText("Sustain offset [seconds]", juce::NotificationType::dontSendNotification);
        sustainOffsetLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::plain));
        sustainOffsetLabel.setColour(juce::Label::textColourId, textColor);
        
        addAndMakeVisible(sustainLengthSlider);
        sustainLengthSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
        sustainLengthSlider.setColour(juce::Slider::thumbColourId, sliderColor);
        sustainLengthSlider.setRange(0.5, 2);
        sustainLengthSlider.setValue(1.3);
        addAndMakeVisible(sustainLengthLabel);
        sustainLengthLabel.setText("Sustain length [seconds]", juce::NotificationType::dontSendNotification);
        sustainLengthLabel.setFont(juce::Font("Lato", 16.0f, juce::Font::plain));
        sustainLengthLabel.setColour(juce::Label::textColourId, textColor);
    }
    
    ~GUIPeriodDetectionSection() override
    {}

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BASE_COLOR);
        g.setColour(bgColor);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), CORNER_RADIUS);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        area.removeFromLeft(10);
        periodDtctSectionLabel.setBounds(area.removeFromTop(40));
        auto sustainOffsetArea = area.removeFromLeft(200);
        sustainOffsetLabel.setBounds(sustainOffsetArea.removeFromTop(20));
        sustainOffsetSlider.setBounds(sustainOffsetArea.removeFromTop(20));
        area.removeFromLeft(10);
        auto sustainLengthArea = area.removeFromLeft(200);
        sustainLengthLabel.setBounds(sustainLengthArea.removeFromTop(20));
        sustainLengthSlider.setBounds(sustainLengthArea.removeFromTop(20));
        area.removeFromLeft(10);
        
    }

    juce::Colour bgColor { 0xFFFFCBA4 };
    juce::Colour titleColor { 0xFFFF7F50 };
    juce::Colour sliderColor { 0xFFFFA07A };
    juce::Colour textColor { 0xFF333333};
    juce::Label periodDtctSectionLabel;
    juce::Slider sustainOffsetSlider;
    juce::Label sustainOffsetLabel;
    juce::Slider sustainLengthSlider;
    juce::Label sustainLengthLabel;
};
