#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr float pi = juce::MathConstants<float>::pi;
    constexpr double twoPi = juce::MathConstants<double>::twoPi;

    float softClip(float x) { return std::tanh(x); }
}

JadeDragonVoice::JadeDragonVoice(std::atomic<float>* inst, std::atomic<float>* a, std::atomic<float>* d,
                                 std::atomic<float>* s, std::atomic<float>* r, std::atomic<float>* b,
                                 std::atomic<float>* drv, std::atomic<float>* cho)
    : instrument(inst), attack(a), decay(d), sustain(s), release(r),
      brightness(b), drive(drv), chorusMix(cho)
{
    lfo.initialise([](float x) { return std::sin(x); }, 128);
}

bool JadeDragonVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<JadeDragonSound*>(sound) != nullptr;
}

void JadeDragonVoice::setCurrentPlaybackSampleRate(double rate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(rate);
    sampleRate = rate;
    envelope.setSampleRate(rate);
    lowpassFilter.setSampleRate(rate);
    lfo.setSampleRate(rate);
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
    lfoPhase = 0.0f;

    float cutoff = 200.0f + (brightness->load() * 18000.0f);
    lowpassFilter.setCutoffFrequency(cutoff);
    lowpassFilter.setResonance(0.7f);

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
    const float fundamental = std::sin(static_cast<float>(p));
    switch (type)
    {
        case 0: return fundamental + 0.55f * std::sin(static_cast<float>(p * 2.0)) + 0.30f * std::sin(static_cast<float>(p * 3.0)) + 0.17f * std::sin(static_cast<float>(p * 4.0)); // Guzheng
        case 1: return fundamental + 0.42f * std::sin(static_cast<float>(p * 2.0)) + 0.20f * std::sin(static_cast<float>(p * 3.0)) + noise * 0.03f; // Erhu
        case 2: return fundamental + 0.17f * std::sin(static_cast<float>(p * 2.0)) + 0.05f * std::sin(static_cast<float>(p * 3.0)) + noise * 0.15f; // Dizi
        case 3: return fundamental + 0.68f * std::sin(static_cast<float>(p * 2.0)) + 0.42f * std::sin(static_cast<float>(p * 3.0)) + 0.24f * std::sin(static_cast<float>(p * 4.0)); // Pipa
        case 4: return fundamental + 0.72f * std::sin(static_cast<float>(p * 2.17)) + 0.47f * std::sin(static_cast<float>(p * 3.42)) + 0.27f * std::sin(static_cast<float>(p * 4.71)); // Gong
        case 5: return fundamental + 0.70f * std::sin(static_cast<float>(p * 2.76)) + 0.34f * std::sin(static_cast<float>(p * 5.40)) + 0.19f * std::sin(static_cast<float>(p * 8.93)); // Jade Bells
        default: return fundamental;
    }
}

void JadeDragonVoice::renderNextBlock(juce::AudioBuffer<float>& buffer, int start, int samples)
{
    if (!isVoiceActive() || sampleRate <= 0.0) return;

    const int type = juce::jlimit(0, 5, static_cast<int>(instrument->load()));
    const float driveAmount = juce::jlimit(0.0f, 1.0f, drive->load());
    const float chorusAmt = chorusMix->load();
    const double bentFrequency = frequency * std::pow(2.0, pitchBend / 12.0);
    const double increment = twoPi * bentFrequency / sampleRate;
    const float saturation = 1.0f + driveAmount * 8.0f;

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

        tone = lowpassFilter.processSample(tone);
        tone = softClip(tone * saturation);

        if (type == 1 && chorusAmt > 0.01f)
        {
            lfoPhase += 2.0f * pi * 5.0f / static_cast<float>(sampleRate);
            tone += tone * std::sin(lfoPhase) * chorusAmt * 0.03f;
        }

        const float amplitude = envelope.getNextSample() * velocity * 0.25f;
        float sample = tone * amplitude;
        sample = sample * 0.95f + previousSample * 0.05f;
        previousSample = sample;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample(channel, start + i, sample);

        if (!envelope.isActive()) { clearCurrentNote(); break; }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout JADEDRAGONAudioProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "instrument", 1 }, "Instrument",
        juce::StringArray { "Guzheng", "Erhu", "Dizi", "Pipa", "Gong", "Jade Bells" }, 0));
    layout.add(std::make_unique<Float>(juce::ParameterID { "attack", 1 }, "Attack", juce::NormalisableRange<float>(0.001f, 5.0f), 0.025f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "decay", 1 }, "Decay", juce::NormalisableRange<float>(0.01f, 5.0f), 0.45f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "sustain", 1 }, "Sustain", 0.0f, 1.0f, 0.72f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "release", 1 }, "Release", juce::NormalisableRange<float>(0.01f, 10.0f), 1.2f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "brightness", 1 }, "Brightness", 0.02f, 1.0f, 0.72f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "drive", 1 }, "Drive", 0.0f, 1.0f, 0.08f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "chorus", 1 }, "Chorus", 0.0f, 1.0f, 0.3f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "reverb", 1 }, "Reverb", 0.0f, 1.0f, 0.32f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "delay", 1 }, "Delay", 0.0f, 1.0f, 0.12f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "master", 1 }, "Master", 0.0f, 1.0f, 0.8f));

    return layout;
}

JADEDRAGONAudioProcessor::JADEDRAGONAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "JADE_DRAGON_STATE", createParameterLayout()),
      delayLine(44100 * 2)
{
    for (int i = 0; i < 16; ++i)
    {
        synth.addVoice(new JadeDragonVoice(
            parameters.getRawParameterValue("instrument"), parameters.getRawParameterValue("attack"),
            parameters.getRawParameterValue("decay"), parameters.getRawParameterValue("sustain"),
            parameters.getRawParameterValue("release"), parameters.getRawParameterValue("brightness"),
            parameters.getRawParameterValue("drive"), parameters.getRawParameterValue("chorus")
        ));
    }
    synth.addSound(new JadeDragonSound());
}

void JADEDRAGONAudioProcessor::prepareToPlay(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    synth.setCurrentPlaybackSampleRate(sampleRate);
    delayLine.prepare({ sampleRate, static_cast<juce::uint32>(blockSize), 2 });
    delayLine.setMaximumDelayInSamples(static_cast<juce::uint32>(sampleRate * 1.5));
    delayLine.setDelay(static_cast<juce::uint32>(sampleRate * 0.35));
    
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(blockSize), 2 };
    reverb.prepare(spec);
    
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = 0.8f; rp.damping = 0.5f; rp.width = 1.0f; rp.wetLevel = 0.3f; rp.dryLevel = 0.8f;
    reverb.setParameters(rp);
}

void JADEDRAGONAudioProcessor::releaseResources() { synth.allNotesOff(0, false); }

bool JADEDRAGONAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void JADEDRAGONAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    const int samples = buffer.getNumSamples();
    const float delayAmt = parameters.getRawParameterValue("delay")->load();
    const float reverbAmt = parameters.getRawParameterValue("reverb")->load();
    const float master = parameters.getRawParameterValue("master")->load();

    if (delayAmt > 0.01f)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < samples; ++i)
            {
                float delayed = delayLine.popSample(ch);
                delayLine.pushSample(ch, data[i] + delayed * 0.4f);
                data[i] += delayed * delayAmt * 0.6f;
            }
        }
    }

    if (reverbAmt > 0.01f)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        auto rp = reverb.getParameters();
        rp.wetLevel = reverbAmt * 0.6f;
        rp.dryLevel = 1.0f - reverbAmt * 0.3f;
        reverb.setParameters(rp);
        reverb.process(ctx);
    }

    buffer.applyGain(master);

    float rms = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getReadPointer(ch);
        for (int i = 0; i < samples; ++i) rms += data[i] * data[i];
    }
    currentLevel = std::sqrt(rms / (samples * buffer.getNumChannels())) * 4.0f;
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

juce::AudioProcessorEditor* JADEDRAGONAudioProcessor::createEditor() { return new JADEDRAGONAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new JADEDRAGONAudioProcessor(); }
