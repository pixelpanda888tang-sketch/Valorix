#include "AIParamMapper.h"

PromptPatch AIParamMapper::mapPromptToPatch (const juce::String& prompt) const
{
    auto text = prompt.toLowerCase();
    PromptPatch patch;

    // Lightweight rule-based "AI" parser. Replace with a real service/API later.
    if (text.contains ("808") || text.contains ("sub"))
    {
        patch.oscMorph = 0.15f;
        patch.filterCutoff = 180.0f;
        patch.filterResonance = 0.1f;
        patch.ampAttack = 0.002f;
        patch.ampDecay = 0.2f;
        patch.ampSustain = 0.9f;
        patch.drive = 0.35f;
    }

    if (text.contains ("pluck"))
    {
        patch.ampAttack = 0.001f;
        patch.ampDecay = 0.07f;
        patch.ampSustain = 0.2f;
        patch.ampRelease = 0.12f;
        patch.filterCutoff = 2100.0f;
        patch.filterResonance = 0.55f;
    }

    if (text.contains ("keys") || text.contains ("piano"))
    {
        patch.oscMorph = 0.7f;
        patch.filterCutoff = 3000.0f;
        patch.reverbMix = 0.24f;
        patch.delayMix = 0.18f;
    }

    if (text.contains ("ambient") || text.contains ("pad"))
    {
        patch.ampAttack = 0.4f;
        patch.ampRelease = 0.9f;
        patch.reverbMix = 0.45f;
        patch.delayMix = 0.35f;
        patch.filterCutoff = 1200.0f;
    }

    if (text.contains ("bright"))
        patch.filterCutoff = juce::jmax (patch.filterCutoff, 4500.0f);

    if (text.contains ("dark"))
        patch.filterCutoff = juce::jmin (patch.filterCutoff, 650.0f);

    if (text.contains ("aggressive") || text.contains ("distorted"))
        patch.drive = 0.8f;

    return patch;
}

