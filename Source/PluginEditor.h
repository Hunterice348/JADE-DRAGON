#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct Point3D { float x, y, z; };

class JadeLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    JadeLookAndFeel();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosition, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
};

class JADEDRAGONAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
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
    juce::Slider attack, decay, sustain, release, brightness, drive, chorus, reverb, delay, master;
    juce::Label titleLabel, subtitleLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ComboAttachment> instrumentAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;
    std::unique_ptr<SliderAttachment> brightnessAttachment, driveAttachment, chorusAttachment, reverbAttachment, delayAttachment, masterAttachment;

    float rotation = 0.0f;
    float smoothLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JADEDRAGONAudioProcessorEditor)
};
