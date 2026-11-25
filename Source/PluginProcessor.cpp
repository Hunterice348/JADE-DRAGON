#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr double twoPi = juce::MathConstants<double>::twoPi;
    constexpr float pi = juce::MathConstants<float>::pi;
}

// ===================================================================
// VOICE
// ===================================================================

JadeDragonVoice::JadeDragonVoice(std::atomic<float>* inst, std::atomic<float>* a, std::atomic<float>* d,
                                 std::atomic<float>* s, std::atomic<float>* r, std::atomic<float>* b,
                                 std::atomic<float>* drv, std::atomic<float>* cho)
    : instrument(inst), attack(a), decay(d), sustain(s), release(r),
      brightness(b), drive(drv), chorusMix(cho) {}

bool JadeDragonVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<JadeDragonSound*>(sound) != nullptr;
}

void JadeDragonVoice::setCurrentPlaybackSampleRate(double rate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(rate);
    sampleRate = rate;
    envelope.setSampleRate(rate);
}

void JadeDragonVoice::startNote(int midiNote, float velocityValue, juce::SynthesiserSound*, int pitchWheel)
{
    juce::ADSR::Parameters env;
    env.attack = attack->load();
    env.decay = decay->load();
    env.sustain = sustain->load();
    env.release = release->load();
    envelope.setParameters(env);

    frequency = juce::MidiMessage::getMidiNoteInHertz(midiNote);
    velocity = velocityValue;
    phase = 0.0;
    previousSample = 0.0f;
    pluckAmount = 1.0f;
    filterState1 = 0.0f;
    filterState2 = 0.0f;
    lfoPhase = 0.0f;

    pitchWheelMoved(pitchWheel);
    envelope.noteOn();
}

void JadeDragonVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff) envelope.noteOff();
    else { envelope.reset(); clearCurrentNote(); }
}

void JadeDragonVoice::pitchWheelMoved(int value)
{
    pitchBend = ((static_cast<float>(value) - 8192.0f) / 8192.0f) * 2.0f;
}

void JadeDragonVoice::controllerMoved(int number, int value)
{
    juce::ignoreUnused(number, value);
}

float JadeDragonVoice::nextNoise()
{
    noiseState ^= noiseState << 13;
    noiseState ^= noiseState >> 17;
    noiseState ^= noiseState << 5;
    return static_cast<float>(noiseState & 0x00ffffffu) / 8388607.5f - 1.0f;
}

float JadeDragonVoice::generateTone(int type, double p, float noise)
{
    const float f = static_cast<float>(p);
    switch (type)
    {
        case 0: // Guzheng
            return std::sin(f) + 0.55f * std::sin(f * 2.0f) + 0.30f * std::sin(f * 3.0f) +
                   0.17f * std::sin(f * 4.0f) + 0.08f * std::sin(f * 6.0f);
        case 1: // Erhu
            return std::sin(f) + 0.42f * std::sin(f * 2.0f) + 0.20f * std::sin(f * 3.0f) +
                   0.10f * std::sin(f * 4.0f) + noise * 0.03f;
        case 2: // Dizi
            return std::sin(f) + 0.17f * std::sin(f * 2.0f) + 0.05f * std::sin(f * 3.0f) + noise * 0.15f;
        case 3: // Pipa
            return std::sin(f) + 0.68f * std::sin(f * 2.0f) + 0.42f * std::sin(f * 3.0f) +
                   0.24f * std::sin(f * 4.0f) + 0.13f * std::sin(f * 7.0f);
        case 4: // Gong
            return std::sin(f) + 0.72f * std::sin(f * 2.17f) + 0.47f * std::sin(f * 3.42f) +
                   0.27f * std::sin(f * 4.71f) + 0.15f * std::sin(f * 6.83f);
        case 5: // Jade Bells
            return std::sin(f) + 0.70f * std::sin(f * 2.76f) + 0.34f * std::sin(f * 5.40f) +
                   0.19f * std::sin(f * 8.93f) + 0.08f * std::sin(f * 12.2f);
        default: return std::sin(f);
    }
}

void JadeDragonVoice::renderNextBlock(juce::AudioBuffer<float>& buffer, int start, int samples)
{
    if (!isVoiceActive() || sampleRate <= 0.0) return;

    const int type = juce::jlimit(0, 5, static_cast<int>(instrument->load()));
    const float bright = juce::jlimit(0.02f, 1.0f, brightness->load());
    const float driveAmount = juce::jlimit(0.0f, 1.0f, drive->load());
    const float chorusAmt = chorusMix->load();

    const double bentFrequency = frequency * std::pow(2.0, pitchBend / 12.0);
    const double increment = twoPi * bentFrequency / sampleRate;

    const float cutoff = 350.0f + bright * 15000.0f;
    const float filterCoef = juce::jlimit(0.001f, 0.99f,
        1.0f - std::exp(-2.0f * pi * cutoff / static_cast<float>(sampleRate)));

    const float resonance = 0.4f + bright * 0.5f;
    const float saturation = 1.0f + driveAmount * 12.0f;

    for (int i = 0; i < samples; ++i)
    {
        const float noise = nextNoise();
        float tone = generateTone(type, phase, noise);
        phase += increment;
        if (phase >= twoPi) phase -= twoPi;

        const float transient = pluckAmount;
        pluckAmount *= (type == 0 || type == 3) ? 0.9990f : 0.9980f;

        if (type == 0 || type == 3) tone += noise * transient * 0.12f;
        if (type == 2) tone += noise * 0.15f;

        // 2-pole resonant lowpass filter
        filterState1 += filterCoef * (tone - filterState1);
        filterState2 += filterCoef * (filterState1 - filterState2 +
                       resonance * (filterState1 - filterState2) * 0.3f);

        float sample = filterState2;
        sample = softClip(sample * saturation);

        // Chorus for Erhu
        if (type == 1 && chorusAmt > 0.01f)
        {
            lfoPhase += 2.0f * pi * 5.0f / static_cast<float>(sampleRate);
            sample += sample * std::sin(lfoPhase) * chorusAmt * 0.04f;
        }

        const float amplitude = envelope.getNextSample() * velocity * 0.23f;
        sample *= amplitude;
        sample = sample * 0.92f + previousSample * 0.08f;
        previousSample = sample;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample(channel, start + i, sample);

        if (!envelope.isActive()) { clearCurrentNote(); break; }
    }
}

// ===================================================================
// PROCESSOR
// ===================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
JADEDRAGONAudioProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "instrument", 1 }, "Instrument",
        juce::StringArray { "Guzheng", "Erhu", "Dizi", "Pipa", "Gong", "Jade Bells" }, 0));

    layout.add(std::make_unique<Float>(juce::ParameterID { "attack", 1 }, "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.025f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "decay", 1 }, "Decay",
        juce::NormalisableRange<float>(0.01f, 5.0f, 0.01f, 0.4f), 0.45f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "sustain", 1 }, "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.72f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "release", 1 }, "Release",
        juce::NormalisableRange<float>(0.01f, 10.0f, 0.01f, 0.5f), 1.2f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "brightness", 1 }, "Brightness",
        juce::NormalisableRange<float>(0.02f, 1.0f, 0.01f), 0.72f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "drive", 1 }, "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.08f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "chorus", 1 }, "Chorus",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "reverb", 1 }, "Reverb",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.32f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "delay", 1 }, "Delay",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.12f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "master", 1 }, "Master",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    return layout;
}

JADEDRAGONAudioProcessor::JADEDRAGONAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "JADE_DRAGON_STATE", createParameterLayout())
{
    for (int i = 0; i < 16; ++i)
    {
        synth.addVoice(new JadeDragonVoice(
            parameters.getRawParameterValue("instrument"),
            parameters.getRawParameterValue("attack"),
            parameters.getRawParameterValue("decay"),
            parameters.getRawParameterValue("sustain"),
            parameters.getRawParameterValue("release"),
            parameters.getRawParameterValue("brightness"),
            parameters.getRawParameterValue("drive"),
            parameters.getRawParameterValue("chorus")));
    }
    synth.addSound(new JadeDragonSound());
}

void JADEDRAGONAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    synth.setCurrentPlaybackSampleRate(sampleRate);

    const size_t delaySize = static_cast<size_t>(sampleRate * 2.0);
    for (auto& channel : delayBuffer) channel.assign(delaySize, 0.0f);
    delayPosition = 0;

    reverb.setSampleRate(sampleRate);
    juce::Reverb::Parameters p;
    p.roomSize = 0.75f; p.damping = 0.42f; p.width = 1.0f;
    p.wetLevel = 0.3f; p.dryLevel = 0.8f; p.freezeMode = 0.0f;
    reverb.setParameters(p);
}

void JADEDRAGONAudioProcessor::releaseResources() { synth.allNotesOff(0, false); }

bool JADEDRAGONAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}

void JADEDRAGONAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    const int samples = buffer.getNumSamples();
    const int channels = buffer.getNumChannels();
    const float delayAmt = parameters.getRawParameterValue("delay")->load();
    const float reverbAmt = parameters.getRawParameterValue("reverb")->load();
    const float master = parameters.getRawParameterValue("master")->load();

    // Delay (только для stereo)
    if (channels == 2 && !delayBuffer[0].empty())
    {
        float* left = buffer.getWritePointer(0);
        float* right = buffer.getWritePointer(1);
        const int size = static_cast<int>(delayBuffer[0].size());
        const int delaySamples = juce::jlimit(1, size - 1, static_cast<int>(currentSampleRate * 0.34));

        for (int i = 0; i < samples; ++i)
        {
            const int readPos = (delayPosition - delaySamples + size) % size;
            const float echoL = delayBuffer[0][static_cast<size_t>(readPos)];
            const float echoR = delayBuffer[1][static_cast<size_t>(readPos)];

            delayBuffer[0][static_cast<size_t>(delayPosition)] = left[i] + echoL * 0.36f;
            delayBuffer[1][static_cast<size_t>(delayPosition)] = right[i] + echoR * 0.36f;

            left[i] += echoL * delayAmt * 0.55f;
            right[i] += echoR * delayAmt * 0.55f;

            delayPosition = (delayPosition + 1) % size;
        }
    }

    // Reverb
    if (reverbAmt > 0.001f)
    {
        auto rp = reverb.getParameters();
        rp.wetLevel = reverbAmt * 0.55f;
        rp.dryLevel = 1.0f - reverbAmt * 0.25f;
        reverb.setParameters(rp);

        if (channels == 2)
        {
            reverb.processStereo(buffer.getWritePointer(0),
                                 buffer.getWritePointer(1), samples);
        }
        else if (channels == 1)
        {
            reverb.processMono(buffer.getWritePointer(0), samples);
        }
    }

    buffer.applyGain(master);

    // RMS для визуализатора
    float rms = 0.0f;
    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getReadPointer(ch);
        for (int i = 0; i < samples; ++i) rms += data[i] * data[i];
    }
    currentLevel = std::sqrt(rms / (samples * std::max(1, channels))) * 4.0f;
}

const juce::String JADEDRAGONAudioProcessor::getName() const { return JucePlugin_Name; }

void JADEDRAGONAudioProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destination);
}

void JADEDRAGONAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* JADEDRAGONAudioProcessor::createEditor()
{
    return new JADEDRAGONAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JADEDRAGONAudioProcessor();
}
