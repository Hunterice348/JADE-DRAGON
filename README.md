# JADE DRAGON — Chinese Instrument Engine

**Hunterice Records | Version 1.0.0**

JADE DRAGON is a JUCE-based, MIDI-playable virtual instrument with six synthesized Chinese-inspired voices:
Guzheng, Erhu, Dizi, Pipa, Gong and Chinese Bells.

> Audio note: V1.0 uses procedural synthesis. It does not contain field recordings or multisampled acoustic instruments. The voices are sound-design approximations intended for composition and experimentation.

## Features

- VST3 and Standalone targets
- MIDI input and 16-voice polyphony
- Six selectable synthesized instrument models
- ADSR envelope, brightness, reverb and master controls
- Pitch bend and modulation-wheel controller input
- Plugin parameter/state serialization
- Dark jade / antique gold interface

## Requirements

- CMake 3.22+
- C++17 compiler
- Git
- JUCE 8.0.6 (downloaded by CMake FetchContent)
- Windows: Visual Studio 2022 with Desktop development with C++
- macOS: Xcode command line tools
- Linux: a recent GCC/Clang toolchain and JUCE audio/GUI dependencies

## Build

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target JADE_DRAGON_VST3 JADE_DRAGON_Standalone
```

If your generator does not expose those exact target names, list targets with:

```bash
cmake --build build --target help
```

The VST3 bundle is normally placed under:

- Windows: `build/JADE_DRAGON_artefacts/Release/VST3/`
- macOS: `build/JADE_DRAGON_artefacts/Release/VST3/`
- Linux: `build/JADE_DRAGON_artefacts/Release/VST3/`

The Standalone application is in the same `JADE_DRAGON_artefacts` directory.

## Install

Copy the VST3 bundle to the system VST3 folder:

- Windows: `C:\Program Files\Common Files\VST3`
- macOS: `/Library/Audio/Plug-Ins/VST3`
- Linux: `~/.vst3`

Restart or rescan plugins in your DAW.

## Project structure

```text
JADE_DRAGON/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
└── Source/
    ├── PluginProcessor.h
    ├── PluginProcessor.cpp
    ├── PluginEditor.h
    ├── PluginEditor.cpp
    └── Synth/
        ├── DragonSound.h
        ├── DragonVoice.h
        └── DragonVoice.cpp
```

## Controls

| Control | Function |
|---|---|
| Instrument | Select one of six sound models |
| Attack | Time for the note to reach peak level |
| Decay | Time from peak to sustain |
| Sustain | Held-note level |
| Release | Fade after note-off |
| Brightness | Harmonic amount |
| Reverb | Built-in room effect amount |
| Master | Output level |

## Current limitations / next engineering stage

- V1.0 uses oscillator-based synthesis rather than recorded multisamples.
- Chorus is reserved in the parameter plan and will be implemented in a later DSP pass.
- The initial UI is a functional compact interface, not yet a fully custom 3D interface.
- Validate builds on each target OS and test in multiple DAWs before distribution.
- For commercial release, add authentic licensed recordings, preset bank management, installer/signing, latency and CPU profiling, and automated audio tests.

## License

See `LICENSE`.
