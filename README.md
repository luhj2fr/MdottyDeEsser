# MdottyDeEsser

A split-band de-esser VST3 plugin (JUCE + CMake), in the spirit of Waves
Sibilance / DeEsser: it isolates the sibilant "S"/"T" energy in a separate
high band, ducks only that band when it crosses a threshold, and leaves the
rest of the signal untouched.

## Features

- **Split-band de-essing**: sibilant band is detected *and* processed
  separately from the rest of the signal, so gain reduction never dulls
  the low end of the source.
- **Wideband mode**: same sibilance detector instead drives full-signal
  gain reduction (compressor-like behavior triggered by sibilance).
- **Controls**: Threshold, Frequency (center of the detection band),
  Range (max reduction in dB), Mode switch, Listen (solo the detected band).
- **Real-time gain-reduction meter** in the GUI.
- **Mono and stereo** supported (matched in/out channel layouts).
- Custom dark/futuristic GUI — no default JUCE grey knobs.

## Project layout

```
MdottyDeEsser/
├── CMakeLists.txt              # Fetches JUCE, defines the VST3 target
├── Source/
│   ├── PluginProcessor.h/.cpp  # AudioProcessor, parameters, processBlock
│   ├── PluginEditor.h/.cpp     # GUI layout and wiring
│   ├── DeEsserDSP.h            # Split-band detector/gain-reduction engine
│   ├── LookAndFeel.h/.cpp      # Dark theme, custom knob drawing
│   └── GainReductionMeter.h/.cpp
└── .github/workflows/build.yml # CI: builds VST3 on Windows, uploads artifact
```

## Building locally in Visual Studio 2022

**Prerequisites:** CMake ≥ 3.22, Visual Studio 2022 with the "Desktop
development with C++" workload, and an internet connection (CMake's
`FetchContent` pulls JUCE 7.0.12 automatically — no manual JUCE setup
needed).

1. Open a terminal (Developer PowerShell for VS 2022, or a plain
   PowerShell/cmd prompt) in the project root.

2. Generate the Visual Studio solution:

   ```bat
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
   ```

   This creates `build\MdottyDeEsser.sln`.

3. Either:
   - Open `build\MdottyDeEsser.sln` in Visual Studio 2022, select the
     `Release` configuration, set `MdottyDeEsser_VST3` as the startup
     project (or just Build → Build Solution), and build; **or**
   - Build from the command line:

     ```bat
     cmake --build build --config Release --target MdottyDeEsser_VST3
     ```

4. Because `COPY_PLUGIN_AFTER_BUILD` is enabled and no custom
   `VST3_COPY_DIR` is set, JUCE's default post-build step copies the
   plugin straight to the standard system location:

   ```
   C:\Program Files\Common Files\VST3\MdottyDeEsser.vst3
   ```

   (Visual Studio may need to run elevated the first time, since that
   folder requires admin rights to write to.)

5. Rescan plugins in your DAW — MdottyDeEsser should appear under VST3.

## Building via GitHub Actions

Push to `main` (or trigger the workflow manually) and the
`Build MdottyDeEsser VST3` workflow will configure with
`-G "Visual Studio 17 2022"`, build the `MdottyDeEsser_VST3` target in
Release, and upload the resulting `MdottyDeEsser.vst3` bundle as a
downloadable build artifact named `MdottyDeEsser-VST3-Windows`.

## Tuning notes

- **Frequency** sets the center of a bandpass filter used both for
  detection and for the band that gets ducked; 3–10 kHz covers most
  vocal/instrument sibilance, but the control range extends to 12 kHz.
- **Listen** solos that band post-filter so you can dial in Frequency
  and Threshold by ear before trusting the meter.
- **Range** caps the maximum reduction so the de-esser can't over-tighten
  on a single very hot "S".
