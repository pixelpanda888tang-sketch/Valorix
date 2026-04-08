#include "PluginProcessor.h"
#include "PluginEditor.h"

ZyronixTAudioProcessor::ZyronixTAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout()),
      presetManager (apvts)
{
}

void ZyronixTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 2 };

    reverb.reset();
    delay.reset();
    delay.prepare (spec);

    lfoPitch.prepare (spec);
    lfoFilter.prepare (spec);
    lfoAmp.prepare (spec);

    lfoPitch.initialise ([] (float x) { return std::sin (x); });
    lfoFilter.initialise ([] (float x) { return std::sin (x); });
    lfoAmp.initialise ([] (float x) { return std::sin (x); });

    lfoPitch.setFrequency (*apvts.getRawParameterValue ("lfoPitchRate"));
    lfoFilter.setFrequency (*apvts.getRawParameterValue ("lfoFilterRate"));
    lfoAmp.setFrequency (*apvts.getRawParameterValue ("lfoAmpRate"));

    for (auto& voice : voices)
        initialiseVoice (voice);
}

void ZyronixTAudioProcessor::releaseResources() {}

#if ! JucePlugin_IsMidiEffect
bool ZyronixTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void ZyronixTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    handleMidi (midiMessages);

    auto* cutoff = apvts.getRawParameterValue ("filterCutoff");
    auto* res = apvts.getRawParameterValue ("filterResonance");
    auto* drive = apvts.getRawParameterValue ("fxDrive");
    auto* reverbMix = apvts.getRawParameterValue ("fxReverb");
    auto* delayMix = apvts.getRawParameterValue ("fxDelay");

    lfoPitch.setFrequency (*apvts.getRawParameterValue ("lfoPitchRate"));
    lfoFilter.setFrequency (*apvts.getRawParameterValue ("lfoFilterRate"));
    lfoAmp.setFrequency (*apvts.getRawParameterValue ("lfoAmpRate"));

    juce::dsp::Reverb::Parameters rvb;
    rvb.wetLevel = *reverbMix;
    rvb.roomSize = 0.6f;
    rvb.width = 0.8f;
    rvb.dryLevel = 1.0f - *reverbMix;
    reverb.setParameters (rvb);

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        float mixed = 0.0f;

        const auto pitchMod = lfoPitch.processSample (0.0f) * *apvts.getRawParameterValue ("lfoPitchDepth");
        const auto filterMod = lfoFilter.processSample (0.0f) * *apvts.getRawParameterValue ("lfoFilterDepth");
        const auto ampMod = lfoAmp.processSample (0.0f) * *apvts.getRawParameterValue ("lfoAmpDepth");

        for (auto& voice : voices)
        {
            if (! voice.active)
                continue;

            const auto currentCutoff = juce::jlimit (20.0f, 20000.0f, *cutoff + filterMod * 4000.0f);
            voice.svf.setCutoffFrequency (currentCutoff);
            voice.svf.setResonance (*res);

            for (auto& osc : voice.oscillators)
                osc.setFrequency (voice.noteHz * (1.0f + pitchMod * 0.08f));

            mixed += processVoiceSample (voice) * (1.0f + ampMod * 0.4f);
        }

        auto distorted = std::tanh (mixed * (1.0f + *drive * 9.0f));
        auto delayed = delay.popSample (0);
        delay.pushSample (0, distorted + delayed * 0.35f);

        auto sampleOut = juce::jmap (*delayMix, distorted, distorted + delayed * 0.5f);

        buffer.setSample (0, s, sampleOut);
        buffer.setSample (1, s, sampleOut);
    }

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> ctx (block);
    reverb.process (ctx);
}

void ZyronixTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void ZyronixTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout ZyronixTAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto addFloat = [&params] (const juce::String& id, const juce::String& name, float min, float max, float def)
    {
        params.push_back (std::make_unique<juce::AudioParameterFloat> (id, name, juce::NormalisableRange<float> (min, max), def));
    };

    addFloat ("osc1Morph", "Osc 1 Morph", 0.0f, 1.0f, 0.3f);
    addFloat ("osc2Morph", "Osc 2 Morph", 0.0f, 1.0f, 0.4f);
    addFloat ("osc3Morph", "Osc 3 Morph", 0.0f, 1.0f, 0.55f);
    addFloat ("oscMix", "Osc Mix", 0.0f, 1.0f, 0.8f);

    addFloat ("filterType", "Filter Type", 0.0f, 2.0f, 0.0f);
    addFloat ("filterCutoff", "Filter Cutoff", 20.0f, 20000.0f, 1200.0f);
    addFloat ("filterResonance", "Filter Resonance", 0.1f, 2.0f, 0.4f);

    addFloat ("ampAttack", "Amp Attack", 0.001f, 3.0f, 0.01f);
    addFloat ("ampDecay", "Amp Decay", 0.001f, 3.0f, 0.12f);
    addFloat ("ampSustain", "Amp Sustain", 0.0f, 1.0f, 0.7f);
    addFloat ("ampRelease", "Amp Release", 0.001f, 5.0f, 0.35f);

    addFloat ("lfoPitchRate", "LFO Pitch Rate", 0.05f, 16.0f, 4.0f);
    addFloat ("lfoPitchDepth", "LFO Pitch Depth", 0.0f, 1.0f, 0.1f);
    addFloat ("lfoFilterRate", "LFO Filter Rate", 0.05f, 16.0f, 3.0f);
    addFloat ("lfoFilterDepth", "LFO Filter Depth", 0.0f, 1.0f, 0.2f);
    addFloat ("lfoAmpRate", "LFO Amp Rate", 0.05f, 16.0f, 2.0f);
    addFloat ("lfoAmpDepth", "LFO Amp Depth", 0.0f, 1.0f, 0.15f);

    addFloat ("fxDrive", "Distortion", 0.0f, 1.0f, 0.15f);
    addFloat ("fxDelay", "Delay", 0.0f, 1.0f, 0.1f);
    addFloat ("fxReverb", "Reverb", 0.0f, 1.0f, 0.15f);
    addFloat ("eqLow", "EQ Low", -18.0f, 18.0f, 0.0f);
    addFloat ("eqMid", "EQ Mid", -18.0f, 18.0f, 0.0f);
    addFloat ("eqHigh", "EQ High", -18.0f, 18.0f, 0.0f);

    addFloat ("macroTone", "Macro Tone", 0.0f, 1.0f, 0.5f);
    addFloat ("macroTexture", "Macro Texture", 0.0f, 1.0f, 0.5f);
    addFloat ("macroAI", "Macro AI Influence", 0.0f, 1.0f, 0.7f);

    return { params.begin(), params.end() };
}

void ZyronixTAudioProcessor::applyPromptToSynth (const juce::String& prompt)
{
    auto patch = aiMapper.mapPromptToPatch (prompt);

    auto setIfExists = [this] (const juce::String& id, float value)
    {
        if (auto* param = apvts.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (value));
    };

    setIfExists ("osc1Morph", patch.oscMorph);
    setIfExists ("osc2Morph", juce::jlimit (0.0f, 1.0f, patch.oscMorph + 0.1f));
    setIfExists ("osc3Morph", juce::jlimit (0.0f, 1.0f, patch.oscMorph + 0.2f));
    setIfExists ("filterCutoff", patch.filterCutoff);
    setIfExists ("filterResonance", patch.filterResonance);
    setIfExists ("ampAttack", patch.ampAttack);
    setIfExists ("ampDecay", patch.ampDecay);
    setIfExists ("ampSustain", patch.ampSustain);
    setIfExists ("ampRelease", patch.ampRelease);
    setIfExists ("fxDrive", patch.drive);
    setIfExists ("fxReverb", patch.reverbMix);
    setIfExists ("fxDelay", patch.delayMix);
}

void ZyronixTAudioProcessor::initialiseVoice (Voice& voice)
{
    juce::ADSR::Parameters amp;
    amp.attack = *apvts.getRawParameterValue ("ampAttack");
    amp.decay = *apvts.getRawParameterValue ("ampDecay");
    amp.sustain = *apvts.getRawParameterValue ("ampSustain");
    amp.release = *apvts.getRawParameterValue ("ampRelease");

    voice.ampEnv.setSampleRate (currentSampleRate);
    voice.ampEnv.setParameters (amp);

    voice.filterEnv.setSampleRate (currentSampleRate);
    voice.filterEnv.setParameters (amp);

    voice.svf.reset();
    voice.svf.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

    for (auto& osc : voice.oscillators)
    {
        osc.prepare (currentSampleRate);
        osc.setLevel (0.33f);
    }

    voice.noiseOsc.prepare (currentSampleRate);
    voice.noiseOsc.setLevel (0.1f);
}

void ZyronixTAudioProcessor::handleMidi (const juce::MidiBuffer& midiMessages)
{
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            for (auto& voice : voices)
            {
                if (! voice.active)
                {
                    voice.active = true;
                    voice.midiNote = msg.getNoteNumber();
                    voice.noteHz = (float) juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber());
                    voice.ampEnv.noteOn();
                    voice.filterEnv.noteOn();
                    break;
                }
            }
        }
        else if (msg.isNoteOff())
        {
            for (auto& voice : voices)
            {
                if (voice.active && voice.midiNote == msg.getNoteNumber())
                {
                    voice.ampEnv.noteOff();
                    voice.filterEnv.noteOff();
                }
            }
        }
    }
}

float ZyronixTAudioProcessor::processVoiceSample (Voice& voice)
{
    const auto morph1 = *apvts.getRawParameterValue ("osc1Morph");
    const auto morph2 = *apvts.getRawParameterValue ("osc2Morph");
    const auto morph3 = *apvts.getRawParameterValue ("osc3Morph");

    voice.oscillators[0].setMorph (morph1);
    voice.oscillators[1].setMorph (morph2);
    voice.oscillators[2].setMorph (morph3);

    float sample = 0.0f;
    for (auto& osc : voice.oscillators)
        sample += osc.processSample();

    sample *= *apvts.getRawParameterValue ("oscMix");

    sample += juce::Random::getSystemRandom().nextFloat() * 0.08f - 0.04f;

    const auto filterMode = (int) std::round (*apvts.getRawParameterValue ("filterType"));
    voice.svf.setType (filterMode == 0 ? juce::dsp::StateVariableTPTFilterType::lowpass
                                        : (filterMode == 1 ? juce::dsp::StateVariableTPTFilterType::highpass
                                                           : juce::dsp::StateVariableTPTFilterType::bandpass));

    sample = voice.svf.processSample (0, sample);

    auto envValue = voice.ampEnv.getNextSample();
    if (! voice.ampEnv.isActive())
        voice.active = false;

    return sample * envValue;
}

juce::AudioProcessorEditor* ZyronixTAudioProcessor::createEditor()
{
    return new ZyronixTAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZyronixTAudioProcessor();
}

