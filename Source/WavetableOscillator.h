#pragma once

#include <JuceHeader.h>

class WavetableOscillator
{
public:
    void prepare (double sampleRate)
    {
        sr = sampleRate;
        setFrequency (frequencyHz);
    }

    void setFrequency (float hz)
    {
        frequencyHz = hz;
        delta = static_cast<float> (frequencyHz / sr);
    }

    void setLevel (float newLevel) { level = newLevel; }

    // Supports oscillator morphing across two wavetable frames.
    void setMorph (float newMorph)
    {
        morph = juce::jlimit (0.0f, 1.0f, newMorph);
    }

    void setCurrentWavetable (const juce::AudioBuffer<float>& tableA, const juce::AudioBuffer<float>& tableB)
    {
        wavetableA = tableA;
        wavetableB = tableB;
    }

    float processSample()
    {
        const auto tableSize = wavetableA.getNumSamples();
        if (tableSize <= 1)
            return 0.0f;

        auto pos = phase * static_cast<float> (tableSize - 1);
        auto idx = static_cast<int> (pos);
        auto frac = pos - static_cast<float> (idx);

        auto a0 = wavetableA.getSample (0, idx);
        auto a1 = wavetableA.getSample (0, juce::jmin (idx + 1, tableSize - 1));

        auto b0 = wavetableB.getSample (0, idx);
        auto b1 = wavetableB.getSample (0, juce::jmin (idx + 1, tableSize - 1));

        const auto sampleA = juce::jmap (frac, a0, a1);
        const auto sampleB = juce::jmap (frac, b0, b1);

        auto out = juce::jmap (morph, sampleA, sampleB) * level;

        phase += delta;
        if (phase >= 1.0f)
            phase -= 1.0f;

        return out;
    }

    static juce::AudioBuffer<float> createSineTable (int size)
    {
        juce::AudioBuffer<float> table (1, size);
        for (int i = 0; i < size; ++i)
            table.setSample (0, i, std::sin (juce::MathConstants<float>::twoPi * (float) i / (float) (size - 1)));
        return table;
    }

    static juce::AudioBuffer<float> createSawTable (int size)
    {
        juce::AudioBuffer<float> table (1, size);
        for (int i = 0; i < size; ++i)
            table.setSample (0, i, juce::jmap ((float) i / (float) (size - 1), -1.0f, 1.0f));
        return table;
    }

private:
    juce::AudioBuffer<float> wavetableA { createSineTable (2048) };
    juce::AudioBuffer<float> wavetableB { createSawTable (2048) };

    double sr = 44100.0;
    float frequencyHz = 220.0f;
    float delta = 0.0f;
    float phase = 0.0f;
    float level = 0.5f;
    float morph = 0.0f;
};

