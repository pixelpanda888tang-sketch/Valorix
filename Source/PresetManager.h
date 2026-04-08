#pragma once

#include <JuceHeader.h>

class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState& state);

    void installFactoryPresets();
    juce::StringArray getFactoryPresetNames() const;
    void loadFactoryPreset (const juce::String& name);

    bool saveUserPresetToFile (const juce::File& file, const juce::String& name) const;
    bool loadUserPresetFromFile (const juce::File& file);

private:
    juce::ValueTree createPreset (const juce::String& name,
                                  float morph,
                                  float cutoff,
                                  float resonance,
                                  float attack,
                                  float decay,
                                  float sustain,
                                  float release,
                                  float drive,
                                  float reverbMix,
                                  float delayMix) const;

    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree factoryBank { "factoryPresets" };
};

