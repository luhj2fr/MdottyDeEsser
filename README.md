# MdottyDeEsser

Pro-grade VST3 de-esser plugin for sibilance reduction.

**Features:**
- Frequency detection (80–8000 Hz)
- Threshold & ratio control (1–10:1)
- Lookahead (0–20 ms)
- Auto makeup gain
- Modern animated UI with hover effects
- Standalone & DAW support

**Build:**
```bash
cmake -B build
cmake --build build --config Release
```

**Install:**
Copy `.vst3` bundle from `build/MdottyDeEsser_artefacts/Release/VST3/` to your DAW's VST3 folder.