# ZyronixT (JUCE VST3 Synth)

ZyronixT is a full JUCE-based VST3/Standalone software synth by **BeatsPanda**.

## Included Features

- 3 wavetable oscillators + wavetable morphing controls.
- Visual waveform display for each oscillator.
- Noise layer blended into the oscillator engine.
- Filter section (LP/HP/BP) with envelope and LFO modulation.
- ADSR envelopes and 3 modulation LFO lanes.
- FX chain: distortion, delay, reverb + EQ parameters.
- Prompt-driven AI-style sound generation (rule-based mapper in this repo).
- Save/load custom presets and built-in factory presets.
- Modern dark UI layout:
  - Top bar: plugin branding + preset menu + load/save.
  - Left panel: oscillator controls + waveform visuals.
  - Center panel: macro knobs (Tone, Texture, AI Influence).
  - Right panel: effects + envelopes + LFO controls.
  - Bottom panel: prompt input + sound generation button.

## Project Structure

- `Source/PluginProcessor.*` – synth voice engine and audio processing.
- `Source/PluginEditor.*` – plugin UI.
- `Source/WavetableOscillator.h` – wavetable oscillator + morphing.
- `Source/AIParamMapper.*` – prompt → parameter mapping.
- `Source/PresetManager.*` – factory/user preset management.
- `ZyronixT.jucer` – Projucer project file (VS2022 export).

## Build in Visual Studio with Projucer

1. Open `ZyronixT.jucer` in Projucer.
2. Verify JUCE module paths in Projucer (`Global Paths`).
3. Click **Save Project and Open in IDE**.
4. In Visual Studio 2022, select `Release` + `x64` and build.
5. Use generated VST3 from `Builds/VisualStudio2022` output path.

## Notes

- The AI feature is currently local/rule-based and designed to be replaceable with an online or embedded model.
- The synthesizer architecture is intentionally readable and extensible for rapid iteration.
