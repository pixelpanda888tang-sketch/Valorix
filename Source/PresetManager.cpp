#include "PresetManager.h"

PresetManager::PresetManager (juce::AudioProcessorValueTreeState& state) : apvts (state)
{
    installFactoryPresets();
}

void PresetManager::installFactoryPresets()
{
    factoryBank.removeAllChildren (nullptr);

    // 5+ built-ins across requested sound families.
    factoryBank.appendChild (createPreset ("808 Punch", 0.12f, 220.0f, 0.18f, 0.001f, 0.15f, 0.8f, 0.2f, 0.45f, 0.08f, 0.04f), nullptr);
    factoryBank.appendChild (createPreset ("808 Long Sub", 0.04f, 130.0f, 0.05f, 0.002f, 0.2f, 0.95f, 0.5f, 0.3f, 0.05f, 0.03f), nullptr);

    factoryBank.appendChild (createPreset ("Pluck Glass", 0.5f, 2600.0f, 0.6f, 0.001f, 0.08f, 0.22f, 0.14f, 0.12f, 0.16f, 0.12f), nullptr);
    factoryBank.appendChild (createPreset ("Pluck Nylon", 0.62f, 2200.0f, 0.45f, 0.002f, 0.1f, 0.24f, 0.18f, 0.08f, 0.12f, 0.1f), nullptr);

    factoryBank.appendChild (createPreset ("Keys Electric", 0.7f, 3200.0f, 0.3f, 0.008f, 0.12f, 0.72f, 0.35f, 0.15f, 0.22f, 0.16f), nullptr);
    factoryBank.appendChild (createPreset ("Keys Bell", 0.9f, 4500.0f, 0.2f, 0.005f, 0.16f, 0.55f, 0.5f, 0.05f, 0.3f, 0.25f), nullptr);

    factoryBank.appendChild (createPreset ("Pad Neon", 0.35f, 1500.0f, 0.4f, 0.35f, 0.6f, 0.7f, 0.9f, 0.18f, 0.48f, 0.36f), nullptr);
    factoryBank.appendChild (createPreset ("Pad Vapor", 0.4f, 1800.0f, 0.36f, 0.4f, 0.7f, 0.74f, 1.1f, 0.14f, 0.52f, 0.42f), nullptr);

    factoryBank.appendChild (createPreset ("Lead Razor", 0.78f, 3800.0f, 0.22f, 0.003f, 0.1f, 0.68f, 0.15f, 0.62f, 0.14f, 0.09f), nullptr);
    factoryBank.appendChild (createPreset ("Lead Air", 0.56f, 3400.0f, 0.28f, 0.004f, 0.12f, 0.7f, 0.24f, 0.28f, 0.18f, 0.12f), nullptr);
}

juce::StringArray PresetManager::getFactoryPresetNames() const
{
    juce::StringArray names;

    for (int i = 0; i < factoryBank.getNumChildren(); ++i)
        names.add (factoryBank.getChild (i).getProperty ("name").toString());

    return names;
}

void PresetManager::loadFactoryPreset (const juce::String& name)
{
    for (int i = 0; i < factoryBank.getNumChildren(); ++i)
    {
        auto child = factoryBank.getChild (i);
        if (child.getProperty ("name") == name)
        {
            for (int p = 0; p < apvts.state.getNumChildren(); ++p)
            {
                auto param = apvts.state.getChild (p);
                auto id = param.getType().toString();
                if (child.hasProperty (id))
                    if (auto* target = apvts.getParameter (id))
                        target->setValueNotifyingHost (target->convertTo0to1 ((float) child[id]));
            }
            return;
        }
    }
}

bool PresetManager::saveUserPresetToFile (const juce::File& file, const juce::String& name) const
{
    juce::ValueTree tree { "preset" };
    tree.setProperty ("name", name, nullptr);

    for (int p = 0; p < apvts.state.getNumChildren(); ++p)
    {
        auto param = apvts.state.getChild (p);
        auto id = param.getType().toString();
        if (auto* src = apvts.getParameter (id))
            tree.setProperty (id, src->getValue(), nullptr);
    }

    auto xml = tree.createXml();
    return xml != nullptr && xml->writeTo (file);
}

bool PresetManager::loadUserPresetFromFile (const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (file));
    if (xml == nullptr)
        return false;

    auto tree = juce::ValueTree::fromXml (*xml);
    if (! tree.isValid())
        return false;

    for (int p = 0; p < apvts.state.getNumChildren(); ++p)
    {
        auto param = apvts.state.getChild (p);
        auto id = param.getType().toString();
        if (tree.hasProperty (id))
            if (auto* target = apvts.getParameter (id))
                target->setValueNotifyingHost (tree.getProperty (id));
    }

    return true;
}

juce::ValueTree PresetManager::createPreset (const juce::String& name,
                                             float morph,
                                             float cutoff,
                                             float resonance,
                                             float attack,
                                             float decay,
                                             float sustain,
                                             float release,
                                             float drive,
                                             float reverbMix,
                                             float delayMix) const
{
    juce::ValueTree preset { "preset" };
    preset.setProperty ("name", name, nullptr);
    preset.setProperty ("osc1Morph", morph, nullptr);
    preset.setProperty ("filterCutoff", cutoff, nullptr);
    preset.setProperty ("filterResonance", resonance, nullptr);
    preset.setProperty ("ampAttack", attack, nullptr);
    preset.setProperty ("ampDecay", decay, nullptr);
    preset.setProperty ("ampSustain", sustain, nullptr);
    preset.setProperty ("ampRelease", release, nullptr);
    preset.setProperty ("fxDrive", drive, nullptr);
    preset.setProperty ("fxReverb", reverbMix, nullptr);
    preset.setProperty ("fxDelay", delayMix, nullptr);

    return preset;
}

