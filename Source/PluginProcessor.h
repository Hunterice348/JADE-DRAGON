#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <atomic>
#include <cmath>

class JadeDragonVoice final : public juce::SynthesiserVoice
{
public:
    JadeDragonVoice(std::atomic<float>* inst, std::atomic<float>* a, std::atomic<float>* d,
                    std::atomic<float>* s, std::atomic<float>* r, std::atomic<float>* b,
                    std::atomic<float>* drv, std::atomic<float>* cho);

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNote, float velocity, juce::SynthesiserSound* sound, int pitchWheel) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int value) override;
    void controllerMoved(int number, int value) override;
    void setCurrentPlaybackSampleRate(double rate) override;
    void renderNextBlock(juce::AudioBuffer<float>& buffer, int start, int samples) override;

private:
    float generateTone(int type, double p, float noise);
    float nextNoise();
    static float softClip(float x) { return std::tanh(x); }

    std::atomic<float>* instrument = nullptr;
    std::atomic<float>* attack = nullptr;
    std::atomic<float>* decay = nullptr;
    std::atomic<float>* sustain = nullptr;
    std::atomic<float>* release = nullptr;
    std::atomic<float>* brightness = nullptr;
    std::atomic<float>* drive = nullptr;
    std::atomic<float>* chorusMix = nullptr;

    juce::ADSR envelope;

    double phase = 0.0;
    double frequency = 440.0;
    double sampleRate = 44100.0;
    float velocity = 0.0f;
    float pitchBend = 0.0f;
    float previousSample = 0.0f;
    float pluckAmount = 0.0f;
    float filterState1 = 0.0f;
    float filterState2 = 0.0f;
    float lfoPhase = 0.0f;
    uint32_t noiseState = 0x12345678u;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JadeDragonVoice)
};

class JadeDragonSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class JADEDRAGONAudioProcessor final : public juce::AudioProcessor
{
public:
    JADEDRAGONAudioProcessor();
    ~JADEDRAGONAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int blockSize) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destination) override;
    void setStateInformation(const void* data, int size) override;

    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> currentLevel { 0.0f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::Synthesiser synth;
    juce::Reverb reverb;

    std::array<std::vector<float>, 2> delayBuffer;
    int delayPosition = 0;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JADEDRAGONAudioProcessor)
};
