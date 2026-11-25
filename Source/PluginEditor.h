#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class JadeLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    JadeLookAndFeel();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosition, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
};

class JADEDRAGONAudioProcessorEditor final
    : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit JADEDRAGONAudioProcessorEditor(JADEDRAGONAudioProcessor&);
    ~JADEDRAGONAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void setupSlider(juce::Slider& slider, const juce::String& name);
    void draw3DJadeCore(juce::Graphics& g, juce::Rectangle<float> area, float audioLevel);

    JADEDRAGONAudioProcessor& processor;
    JadeLookAndFeel jadeLook;

    juce::ComboBox instrumentSelector;
    juce::Slider attack, decay, sustain, release, brightness;
    juce::Slider drive, chorus, reverb, delay, master;
    juce::Label titleLabel, subtitleLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ComboAttachment> instrumentAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<SliderAttachment> brightnessAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> chorusAttachment;
    std::unique_ptr<SliderAttachment> reverbAttachment;
    std::unique_ptr<SliderAttachment> delayAttachment;
    std::unique_ptr<SliderAttachment> masterAttachment;

    float rotation = 0.0f;
    float smoothLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JADEDRAGONAudioProcessorEditor)
};
