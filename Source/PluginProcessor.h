#pragma once

#include <JuceHeader.h>
#include "AIParamMapper.h"
#include "PresetManager.h"
#include "WavetableOscillator.h"

class ZyronixTAudioProcessor final : public juce::AudioProcessor
{
public:
    ZyronixTAudioProcessor();
    ~ZyronixTAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #if ! JucePlugin_IsMidiEffect
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }
    PresetManager& getPresetManager() { return presetManager; }

    void applyPromptToSynth (const juce::String& prompt);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    struct Voice
    {
        std::array<WavetableOscillator, 3> oscillators;
        WavetableOscillator noiseOsc;

        juce::ADSR ampEnv;
        juce::ADSR filterEnv;

        juce::dsp::StateVariableTPTFilter<float> svf;

        float level = 0.0f;
        float noteHz = 440.0f;
        float phase = 0.0f;
        bool active = false;
        int midiNote = -1;
    };

    void initialiseVoice (Voice& voice);
    void handleMidi (const juce::MidiBuffer& midiMessages);
    float processVoiceSample (Voice& voice);

    std::array<Voice, 8> voices;

    juce::AudioProcessorValueTreeState apvts;
    PresetManager presetManager;
    AIParamMapper aiMapper;

    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay { 48000 };

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> eqLow;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> eqMid;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> eqHigh;

    juce::dsp::Oscillator<float> lfoPitch { [] (float x) { return std::sin (x); } };
    juce::dsp::Oscillator<float> lfoFilter { [] (float x) { return std::sin (x); } };
    juce::dsp::Oscillator<float> lfoAmp { [] (float x) { return std::sin (x); } };

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZyronixTAudioProcessor)
};

