#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>

namespace JadeDragonPresets
{
    struct Preset
    {
        const char* name;
        int instrument;

        float attack;
        float decay;
        float sustain;
        float release;

        float brightness;
        float drive;
        float reverb;
        float delay;
        float master;
    };

    inline constexpr std::array<Preset, 50> bank =
    {{
        // 01–10: Traditional Chinese
        { "01 - Jade Guzheng",       0, 0.005f, 1.20f, 0.25f, 2.50f, 0.78f, 0.08f, 0.22f, 0.12f, 0.85f },
        { "02 - Silk Strings",       0, 0.030f, 0.85f, 0.55f, 2.80f, 0.62f, 0.04f, 0.35f, 0.18f, 0.82f },
        { "03 - Imperial Erhu",      1, 0.080f, 1.50f, 0.72f, 3.20f, 0.58f, 0.06f, 0.42f, 0.10f, 0.82f },
        { "04 - Bamboo Dizi",        2, 0.010f, 0.60f, 0.75f, 1.60f, 0.90f, 0.03f, 0.18f, 0.08f, 0.84f },
        { "05 - Ancient Pipa",       3, 0.003f, 0.75f, 0.18f, 1.40f, 0.82f, 0.12f, 0.16f, 0.10f, 0.86f },
        { "06 - Temple Gong",        4, 0.002f, 3.80f, 0.00f, 5.50f, 0.42f, 0.10f, 0.82f, 0.28f, 0.80f },
        { "07 - Jade Bells",         5, 0.002f, 1.80f, 0.12f, 4.00f, 0.88f, 0.04f, 0.58f, 0.32f, 0.82f },
        { "08 - Moonlit Guzheng",    0, 0.020f, 2.00f, 0.42f, 4.20f, 0.55f, 0.02f, 0.72f, 0.38f, 0.80f },
        { "09 - Wuxia Flute",        2, 0.060f, 1.00f, 0.80f, 2.50f, 0.72f, 0.02f, 0.50f, 0.22f, 0.82f },
        { "10 - Dynasty Pluck",      3, 0.001f, 0.48f, 0.10f, 0.90f, 0.95f, 0.18f, 0.12f, 0.06f, 0.88f },

        // 11–20: Cinematic and atmospheric
        { "11 - Dragon Awakening",   4, 0.150f, 3.50f, 0.65f, 7.00f, 0.48f, 0.12f, 0.90f, 0.42f, 0.78f },
        { "12 - Celestial Garden",   5, 0.450f, 2.80f, 0.72f, 6.50f, 0.65f, 0.02f, 0.88f, 0.48f, 0.78f },
        { "13 - Frozen Palace",      0, 0.350f, 2.40f, 0.62f, 5.80f, 0.72f, 0.03f, 0.82f, 0.52f, 0.78f },
        { "14 - Red Moon",           1, 0.250f, 2.20f, 0.78f, 5.00f, 0.38f, 0.10f, 0.76f, 0.30f, 0.80f },
        { "15 - Spirit Temple",      2, 0.500f, 2.60f, 0.70f, 6.00f, 0.60f, 0.02f, 0.92f, 0.40f, 0.76f },
        { "16 - Golden Horizon",     5, 0.180f, 1.90f, 0.58f, 4.80f, 0.82f, 0.04f, 0.68f, 0.34f, 0.80f },
        { "17 - Silent Mountains",   2, 0.800f, 3.00f, 0.82f, 7.50f, 0.42f, 0.00f, 0.95f, 0.24f, 0.76f },
        { "18 - Jade Nebula",        5, 0.700f, 3.50f, 0.76f, 8.00f, 0.52f, 0.08f, 0.95f, 0.65f, 0.74f },
        { "19 - Imperial Hall",      4, 0.030f, 4.00f, 0.35f, 6.00f, 0.55f, 0.16f, 0.88f, 0.20f, 0.78f },
        { "20 - Eternal Silk",       1, 0.400f, 2.50f, 0.85f, 6.80f, 0.50f, 0.02f, 0.84f, 0.44f, 0.78f },

        // 21–30: Synths and pads
        { "21 - Neon Pagoda",        5, 0.080f, 1.40f, 0.82f, 3.80f, 0.92f, 0.10f, 0.58f, 0.62f, 0.82f },
        { "22 - Cyber Wuxia",        3, 0.010f, 0.70f, 0.55f, 2.20f, 0.96f, 0.32f, 0.38f, 0.46f, 0.80f },
        { "23 - Glass Dragon",       2, 0.120f, 1.80f, 0.68f, 4.20f, 0.98f, 0.04f, 0.74f, 0.55f, 0.78f },
        { "24 - Amber Pad",          1, 0.650f, 2.80f, 0.90f, 7.00f, 0.45f, 0.02f, 0.88f, 0.32f, 0.76f },
        { "25 - Digital Dynasty",    0, 0.015f, 0.90f, 0.48f, 2.60f, 0.88f, 0.22f, 0.42f, 0.50f, 0.80f },
        { "26 - Crystal Rain",       5, 0.010f, 2.20f, 0.22f, 5.00f, 1.00f, 0.02f, 0.82f, 0.70f, 0.78f },
        { "27 - Dark Jade Pad",      4, 0.900f, 3.80f, 0.85f, 8.00f, 0.22f, 0.18f, 0.95f, 0.48f, 0.74f },
        { "28 - Lotus Frequency",    2, 0.250f, 1.60f, 0.72f, 4.80f, 0.68f, 0.05f, 0.76f, 0.60f, 0.78f },
        { "29 - Silk Machine",       3, 0.002f, 0.55f, 0.32f, 1.80f, 0.90f, 0.28f, 0.30f, 0.35f, 0.82f },
        { "30 - Dragon Breath",      1, 0.120f, 1.80f, 0.70f, 3.50f, 0.72f, 0.38f, 0.55f, 0.25f, 0.80f },

        // 31–40: Bass, dark and industrial
        { "31 - Black Jade Bass",    3, 0.001f, 0.35f, 0.00f, 0.45f, 0.32f, 0.55f, 0.08f, 0.04f, 0.90f },
        { "32 - Temple Sub",         4, 0.002f, 0.70f, 0.00f, 0.80f, 0.18f, 0.42f, 0.12f, 0.02f, 0.88f },
        { "33 - Industrial Pipa",    3, 0.001f, 0.28f, 0.12f, 0.60f, 0.95f, 0.72f, 0.10f, 0.08f, 0.86f },
        { "34 - Rusted Gong",        4, 0.001f, 1.80f, 0.00f, 2.50f, 0.28f, 0.82f, 0.40f, 0.18f, 0.80f },
        { "35 - Shadow Erhu",        1, 0.025f, 0.90f, 0.35f, 2.00f, 0.30f, 0.62f, 0.28f, 0.12f, 0.82f },
        { "36 - War Drums",          4, 0.001f, 0.65f, 0.00f, 1.20f, 0.75f, 0.88f, 0.22f, 0.06f, 0.84f },
        { "37 - Concrete Dragon",    3, 0.001f, 0.42f, 0.08f, 0.90f, 0.82f, 0.95f, 0.18f, 0.10f, 0.82f },
        { "38 - Void Gong",          4, 0.010f, 3.20f, 0.00f, 5.50f, 0.16f, 0.68f, 0.92f, 0.34f, 0.76f },
        { "39 - Toxic Bamboo",       2, 0.002f, 0.30f, 0.10f, 0.80f, 1.00f, 0.72f, 0.20f, 0.14f, 0.82f },
        { "40 - Crimson Machine",    0, 0.001f, 0.50f, 0.18f, 1.10f, 0.88f, 0.90f, 0.24f, 0.22f, 0.80f },

        // 41–50: Experimental and special
        { "41 - Quantum Lotus",      5, 0.020f, 2.60f, 0.40f, 5.50f, 1.00f, 0.25f, 0.90f, 0.85f, 0.76f },
        { "42 - Broken Porcelain",   0, 0.001f, 0.18f, 0.00f, 0.35f, 1.00f, 0.78f, 0.42f, 0.66f, 0.80f },
        { "43 - Ghost Opera",        1, 0.900f, 3.00f, 0.92f, 8.00f, 0.36f, 0.08f, 0.98f, 0.72f, 0.74f },
        { "44 - Shattered Moon",     5, 0.005f, 2.40f, 0.18f, 6.00f, 0.98f, 0.45f, 0.86f, 0.92f, 0.76f },
        { "45 - Metallic Phoenix",   4, 0.002f, 2.00f, 0.20f, 4.50f, 0.94f, 0.72f, 0.78f, 0.40f, 0.78f },
        { "46 - Dreaming Dizi",      2, 0.300f, 1.80f, 0.88f, 5.20f, 0.76f, 0.02f, 0.90f, 0.78f, 0.76f },
        { "47 - Acid Dynasty",       3, 0.001f, 0.32f, 0.24f, 0.75f, 1.00f, 0.98f, 0.32f, 0.58f, 0.78f },
        { "48 - Abyssal Temple",     4, 0.600f, 4.00f, 0.55f, 8.00f, 0.12f, 0.52f, 1.00f, 0.82f, 0.72f },
        { "49 - Last Emperor",       1, 0.180f, 2.20f, 0.78f, 5.80f, 0.62f, 0.16f, 0.86f, 0.42f, 0.78f },
        { "50 - JADE DRAGON",        5, 0.080f, 2.40f, 0.68f, 6.00f, 0.88f, 0.35f, 0.82f, 0.72f, 0.80f }
    }};

    inline int getCount()
    {
        return static_cast<int>(bank.size());
    }

    inline const Preset& get(int index)
    {
        return bank[juce::jlimit(0, getCount() - 1, index)];
    }

    inline void setParameter(
        juce::AudioProcessorValueTreeState& state,
        const char* parameterID,
        float value)
    {
        if (auto* parameter = state.getParameter(parameterID))
        {
            parameter->setValueNotifyingHost(
                parameter->convertTo0to1(value));
        }
    }

    inline void apply(
        juce::AudioProcessorValueTreeState& state,
        int index,
        std::function<void(int)> setInstrument = {})
    {
        const auto& p = get(index);

        setParameter(state, "attack",     p.attack);
        setParameter(state, "decay",      p.decay);
        setParameter(state, "sustain",    p.sustain);
        setParameter(state, "release",    p.release);
        setParameter(state, "brightness", p.brightness);
        setParameter(state, "drive",      p.drive);
        setParameter(state, "reverb",     p.reverb);
        setParameter(state, "delay",      p.delay);
        setParameter(state, "master",     p.master);

        if (setInstrument)
            setInstrument(p.instrument);
    }
}