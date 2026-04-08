#include "PluginEditor.h"

ZyronixTAudioProcessorEditor::ZyronixTAudioProcessorEditor (ZyronixTAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      osc1View (audioProcessor.getValueTreeState(), "osc1Morph"),
      osc2View (audioProcessor.getValueTreeState(), "osc2Morph"),
      osc3View (audioProcessor.getValueTreeState(), "osc3Morph")
{
    setSize (1280, 760);

    titleLabel = makeHeaderLabel ("ZyronixT");
    authorLabel = makeHeaderLabel ("by BeatsPanda");
    authorLabel.setJustificationType (juce::Justification::centredRight);

    addAndMakeVisible (titleLabel);
    addAndMakeVisible (authorLabel);

    auto factoryNames = audioProcessor.getPresetManager().getFactoryPresetNames();
    for (int i = 0; i < factoryNames.size(); ++i)
        presetBox.addItem (factoryNames[i], i + 1);

    presetBox.onChange = [this, factoryNames]
    {
        const auto idx = presetBox.getSelectedItemIndex();
        if (juce::isPositiveAndBelow (idx, factoryNames.size()))
            audioProcessor.getPresetManager().loadFactoryPreset (factoryNames[idx]);
    };

    presetBox.setSelectedItemIndex (0);
    addAndMakeVisible (presetBox);

    loadButton.addListener (this);
    saveButton.addListener (this);
    generateButton.addListener (this);
    addAndMakeVisible (loadButton);
    addAndMakeVisible (saveButton);

    osc1Morph = makeKnob();
    osc2Morph = makeKnob();
    osc3Morph = makeKnob();
    addAndMakeVisible (osc1Morph);
    addAndMakeVisible (osc2Morph);
    addAndMakeVisible (osc3Morph);

    addAndMakeVisible (osc1View);
    addAndMakeVisible (osc2View);
    addAndMakeVisible (osc3View);

    macroTone = makeKnob();
    macroTexture = makeKnob();
    macroAI = makeKnob();
    addAndMakeVisible (macroTone);
    addAndMakeVisible (macroTexture);
    addAndMakeVisible (macroAI);

    fxDrive = makeKnob();
    fxDelay = makeKnob();
    fxReverb = makeKnob();

    ampAttack = makeVerticalSlider();
    ampDecay = makeVerticalSlider();
    ampSustain = makeVerticalSlider();
    ampRelease = makeVerticalSlider();

    lfoPitchRate = makeVerticalSlider();
    lfoPitchDepth = makeVerticalSlider();
    lfoFilterRate = makeVerticalSlider();
    lfoFilterDepth = makeVerticalSlider();
    lfoAmpRate = makeVerticalSlider();
    lfoAmpDepth = makeVerticalSlider();

    for (auto* c : std::array<juce::Component*, 13> { &fxDrive, &fxDelay, &fxReverb, &ampAttack, &ampDecay, &ampSustain,
                                                      &ampRelease, &lfoPitchRate, &lfoPitchDepth, &lfoFilterRate,
                                                      &lfoFilterDepth, &lfoAmpRate, &lfoAmpDepth })
        addAndMakeVisible (c);

    promptEditor.setTextToShowWhenEmpty ("Type a prompt (e.g., 'dark aggressive 808 with short pluck tail')", juce::Colours::grey);
    addAndMakeVisible (promptEditor);
    addAndMakeVisible (generateButton);

    auto& state = audioProcessor.getValueTreeState();
    osc1Attach = std::make_unique<SliderAttachment> (state, "osc1Morph", osc1Morph);
    osc2Attach = std::make_unique<SliderAttachment> (state, "osc2Morph", osc2Morph);
    osc3Attach = std::make_unique<SliderAttachment> (state, "osc3Morph", osc3Morph);
    toneAttach = std::make_unique<SliderAttachment> (state, "macroTone", macroTone);
    textureAttach = std::make_unique<SliderAttachment> (state, "macroTexture", macroTexture);
    aiAttach = std::make_unique<SliderAttachment> (state, "macroAI", macroAI);

    driveAttach = std::make_unique<SliderAttachment> (state, "fxDrive", fxDrive);
    delayAttach = std::make_unique<SliderAttachment> (state, "fxDelay", fxDelay);
    reverbAttach = std::make_unique<SliderAttachment> (state, "fxReverb", fxReverb);

    atkAttach = std::make_unique<SliderAttachment> (state, "ampAttack", ampAttack);
    decAttach = std::make_unique<SliderAttachment> (state, "ampDecay", ampDecay);
    susAttach = std::make_unique<SliderAttachment> (state, "ampSustain", ampSustain);
    relAttach = std::make_unique<SliderAttachment> (state, "ampRelease", ampRelease);

    lfoPRAttach = std::make_unique<SliderAttachment> (state, "lfoPitchRate", lfoPitchRate);
    lfoPDAttach = std::make_unique<SliderAttachment> (state, "lfoPitchDepth", lfoPitchDepth);
    lfoFRAttach = std::make_unique<SliderAttachment> (state, "lfoFilterRate", lfoFilterRate);
    lfoFDAttach = std::make_unique<SliderAttachment> (state, "lfoFilterDepth", lfoFilterDepth);
    lfoARAttach = std::make_unique<SliderAttachment> (state, "lfoAmpRate", lfoAmpRate);
    lfoADAttach = std::make_unique<SliderAttachment> (state, "lfoAmpDepth", lfoAmpDepth);
}

ZyronixTAudioProcessorEditor::~ZyronixTAudioProcessorEditor()
{
    loadButton.removeListener (this);
    saveButton.removeListener (this);
    generateButton.removeListener (this);
}

void ZyronixTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0c0f16));

    auto bounds = getLocalBounds();

    g.setGradientFill (juce::ColourGradient (
        juce::Colour (0xff111726), bounds.getX(), bounds.getY(),
        juce::Colour (0xff090c13), bounds.getRight(), bounds.getBottom(), false));
    g.fillRoundedRectangle (bounds.toFloat().reduced (6.0f), 16.0f);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (bounds.toFloat().reduced (6.0f), 16.0f, 1.2f);
}

void ZyronixTAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (14);
    auto top = area.removeFromTop (56);

    titleLabel.setBounds (top.removeFromLeft (220));
    authorLabel.setBounds (top.removeFromRight (220));

    auto presetArea = top.reduced (8, 8);
    presetBox.setBounds (presetArea.removeFromLeft (220));
    saveButton.setBounds (presetArea.removeFromLeft (90).reduced (4, 0));
    loadButton.setBounds (presetArea.removeFromLeft (90).reduced (4, 0));

    auto bottom = area.removeFromBottom (120);
    promptEditor.setBounds (bottom.removeFromLeft (area.getWidth() - 170).reduced (8));
    generateButton.setBounds (bottom.reduced (8));

    auto left = area.removeFromLeft (370);
    auto center = area.removeFromLeft (280);
    auto right = area;

    auto oscHeight = left.getHeight() / 3;

    for (auto [view, knob] : std::array<std::pair<juce::Component*, juce::Component*>, 3> {
            std::pair<juce::Component*, juce::Component*> (&osc1View, &osc1Morph),
            std::pair<juce::Component*, juce::Component*> (&osc2View, &osc2Morph),
            std::pair<juce::Component*, juce::Component*> (&osc3View, &osc3Morph) })
    {
        auto lane = left.removeFromTop (oscHeight).reduced (6);
        view->setBounds (lane.removeFromLeft (250));
        knob->setBounds (lane.reduced (16));
    }

    auto macroArea = center.reduced (20);
    auto macroWidth = macroArea.getWidth() / 3;
    macroTone.setBounds (macroArea.removeFromLeft (macroWidth).reduced (8));
    macroTexture.setBounds (macroArea.removeFromLeft (macroWidth).reduced (8));
    macroAI.setBounds (macroArea.reduced (8));

    auto fxArea = right.removeFromTop (180).reduced (8);
    auto fxWidth = fxArea.getWidth() / 3;
    fxDrive.setBounds (fxArea.removeFromLeft (fxWidth).reduced (4));
    fxDelay.setBounds (fxArea.removeFromLeft (fxWidth).reduced (4));
    fxReverb.setBounds (fxArea.reduced (4));

    auto envLfo = right.reduced (8);
    auto laneWidth = envLfo.getWidth() / 10;
    std::array<juce::Component*, 10> bars { &ampAttack, &ampDecay, &ampSustain, &ampRelease,
                                            &lfoPitchRate, &lfoPitchDepth, &lfoFilterRate, &lfoFilterDepth,
                                            &lfoAmpRate, &lfoAmpDepth };

    for (auto* bar : bars)
        bar->setBounds (envLfo.removeFromLeft (laneWidth).reduced (2));
}

juce::Slider ZyronixTAudioProcessorEditor::makeKnob()
{
    juce::Slider s;
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
    s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff5be3ff));
    s.setColour (juce::Slider::thumbColourId, juce::Colour (0xffd8fbff));
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    s.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff0f1524));
    return s;
}

juce::Slider ZyronixTAudioProcessorEditor::makeVerticalSlider()
{
    juce::Slider s;
    s.setSliderStyle (juce::Slider::LinearVertical);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 48, 16);
    s.setColour (juce::Slider::trackColourId, juce::Colour (0xff355f8a));
    s.setColour (juce::Slider::thumbColourId, juce::Colour (0xff8fe8ff));
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    s.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff101826));
    return s;
}

juce::Label ZyronixTAudioProcessorEditor::makeHeaderLabel (const juce::String& text)
{
    juce::Label l;
    l.setText (text, juce::dontSendNotification);
    l.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    l.setColour (juce::Label::textColourId, juce::Colours::white);
    return l;
}

void ZyronixTAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton)
    {
        audioProcessor.applyPromptToSynth (promptEditor.getText());
        return;
    }

    if (button == &saveButton)
    {
        juce::FileChooser chooser ("Save ZyronixT Preset", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.zyr");
        if (chooser.browseForFileToSave (true))
            audioProcessor.getPresetManager().saveUserPresetToFile (chooser.getResult(), "UserPreset");
    }

    if (button == &loadButton)
    {
        juce::FileChooser chooser ("Load ZyronixT Preset", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.zyr");
        if (chooser.browseForFileToOpen())
            audioProcessor.getPresetManager().loadUserPresetFromFile (chooser.getResult());
    }
}

