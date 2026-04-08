#pragma once

#include <JuceHeader.h>

struct PromptPatch
{
    float oscMorph = 0.5f;
    float filterCutoff = 1200.0f;
    float filterResonance = 0.3f;
    float ampAttack = 0.01f;
    float ampDecay = 0.12f;
    float ampSustain = 0.7f;
    float ampRelease = 0.35f;
    float drive = 0.15f;
    float reverbMix = 0.15f;
    float delayMix = 0.1f;
};

class AIParamMapper
{
public:
    PromptPatch mapPromptToPatch (const juce::String& prompt) const;
};

