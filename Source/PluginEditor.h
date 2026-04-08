#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class WaveformView : public juce::Component, private juce::Timer
{
public:
    explicit WaveformView (juce::AudioProcessorValueTreeState& state, juce::String morphParam)
        : apvts (state), morphId (std::move (morphParam))
    {
        startTimerHz (30);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff10131b));
        g.setColour (juce::Colour (0xff57c7ff));
        g.strokePath (wavePath, juce::PathStrokeType (2.0f));

        g.setColour (juce::Colours::white.withAlpha (0.65f));
        g.drawRect (getLocalBounds(), 1);
    }

private:
    void timerCallback() override
    {
        auto* morphValue = apvts.getRawParameterValue (morphId);
        if (morphValue == nullptr)
            return;

        auto bounds = getLocalBounds().toFloat();
        wavePath.clear();

        for (int i = 0; i < bounds.getWidth(); ++i)
        {
            auto xNorm = (float) i / bounds.getWidth();
            auto sine = std::sin (juce::MathConstants<float>::twoPi * xNorm);
            auto saw = juce::jmap (xNorm, -1.0f, 1.0f);
            auto blend = juce::jmap (*morphValue, sine, saw);

            auto y = juce::jmap (blend, -1.0f, 1.0f, bounds.getBottom() - 5.0f, bounds.getY() + 5.0f);

            if (i == 0)
                wavePath.startNewSubPath ((float) i, y);
            else
                wavePath.lineTo ((float) i, y);
        }

        repaint();
    }

    juce::AudioProcessorValueTreeState& apvts;
    juce::String morphId;
    juce::Path wavePath;
};

class ZyronixTAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Button::Listener
{
public:
    explicit ZyronixTAudioProcessorEditor (ZyronixTAudioProcessor&);
    ~ZyronixTAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Slider makeKnob();
    juce::Slider makeVerticalSlider();
    juce::Label makeHeaderLabel (const juce::String& text);

    void buttonClicked (juce::Button* button) override;

    ZyronixTAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label authorLabel;
    juce::ComboBox presetBox;
    juce::TextButton loadButton { "Load" };
    juce::TextButton saveButton { "Save" };

    juce::Slider osc1Morph, osc2Morph, osc3Morph;
    WaveformView osc1View, osc2View, osc3View;

    juce::Slider macroTone, macroTexture, macroAI;

    juce::Slider fxDrive, fxDelay, fxReverb;
    juce::Slider ampAttack, ampDecay, ampSustain, ampRelease;
    juce::Slider lfoPitchRate, lfoPitchDepth, lfoFilterRate, lfoFilterDepth, lfoAmpRate, lfoAmpDepth;

    juce::TextEditor promptEditor;
    juce::TextButton generateButton { "Generate Sound" };

    std::unique_ptr<SliderAttachment> osc1Attach, osc2Attach, osc3Attach;
    std::unique_ptr<SliderAttachment> toneAttach, textureAttach, aiAttach;
    std::unique_ptr<SliderAttachment> driveAttach, delayAttach, reverbAttach;
    std::unique_ptr<SliderAttachment> atkAttach, decAttach, susAttach, relAttach;
    std::unique_ptr<SliderAttachment> lfoPRAttach, lfoPDAttach, lfoFRAttach, lfoFDAttach, lfoARAttach, lfoADAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZyronixTAudioProcessorEditor)
};

