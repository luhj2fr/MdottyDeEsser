#include "PluginEditor.h"

MdottyDeEsserAudioProcessorEditor::MdottyDeEsserAudioProcessorEditor (MdottyDeEsserAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&lookAndFeel);

    titleLabel.setText ("MDOTTY  DE-ESSER", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (20.0f, juce::Font::bold)));
    titleLabel.setColour (juce::Label::textColourId, MdottyLookAndFeel::accent);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    configureKnob (thresholdSlider, thresholdLabel, "THRESHOLD");
    configureKnob (frequencySlider, frequencyLabel, "FREQUENCY");
    configureKnob (rangeSlider, rangeLabel, "RANGE");

    modeLabel.setText ("MODE", juce::dontSendNotification);
    modeLabel.setJustificationType (juce::Justification::centred);
    modeLabel.setColour (juce::Label::textColourId, MdottyLookAndFeel::textDim);
    modeLabel.setFont (juce::Font (juce::FontOptions (11.0f)));
    addAndMakeVisible (modeLabel);

    modeBox.addItem ("Split-band", 1);
    modeBox.addItem ("Wideband", 2);
    addAndMakeVisible (modeBox);

    listenButton.setClickingTogglesState (true);
    listenButton.setColour (juce::ToggleButton::textColourId, MdottyLookAndFeel::text);
    addAndMakeVisible (listenButton);

    grLabel.setText ("GAIN REDUCTION", juce::dontSendNotification);
    grLabel.setJustificationType (juce::Justification::centred);
    grLabel.setColour (juce::Label::textColourId, MdottyLookAndFeel::textDim);
    grLabel.setFont (juce::Font (juce::FontOptions (11.0f)));
    addAndMakeVisible (grLabel);

    addAndMakeVisible (grMeter);
    grMeter.setValueSource ([this] { return processorRef.getCurrentGainReductionDb(); });

    auto& apvts = processorRef.apvts;
    thresholdAttachment = std::make_unique<SliderAttachment> (apvts, MdottyDeEsserAudioProcessor::thresholdId, thresholdSlider);
    frequencyAttachment = std::make_unique<SliderAttachment> (apvts, MdottyDeEsserAudioProcessor::frequencyId, frequencySlider);
    rangeAttachment     = std::make_unique<SliderAttachment> (apvts, MdottyDeEsserAudioProcessor::rangeId, rangeSlider);
    modeAttachment      = std::make_unique<ComboAttachment>  (apvts, MdottyDeEsserAudioProcessor::modeId, modeBox);
    listenAttachment    = std::make_unique<ButtonAttachment> (apvts, MdottyDeEsserAudioProcessor::listenId, listenButton);

    setSize (560, 360);
}

MdottyDeEsserAudioProcessorEditor::~MdottyDeEsserAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void MdottyDeEsserAudioProcessorEditor::configureKnob (juce::Slider& slider, juce::Label& label,
                                                        const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 18);
    slider.setColour (juce::Slider::textBoxTextColourId, MdottyLookAndFeel::text);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, MdottyLookAndFeel::textDim);
    label.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    addAndMakeVisible (label);
}

void MdottyDeEsserAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::ColourGradient bg (MdottyLookAndFeel::background.brighter (0.03f), bounds.getX(), bounds.getY(),
                              MdottyLookAndFeel::background.darker (0.35f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill (bg);
    g.fillAll();

    // Subtle top accent line under the title bar
    g.setColour (MdottyLookAndFeel::accent.withAlpha (0.5f));
    g.fillRect (juce::Rectangle<float> (0, 44.0f, bounds.getWidth(), 1.5f));

    // Panel behind the controls
    auto panelBounds = bounds.reduced (16.0f).withTrimmedTop (48.0f);
    g.setColour (MdottyLookAndFeel::panel.withAlpha (0.5f));
    g.fillRoundedRectangle (panelBounds, 10.0f);
    g.setColour (MdottyLookAndFeel::accentDim.withAlpha (0.6f));
    g.drawRoundedRectangle (panelBounds, 10.0f, 1.0f);
}

void MdottyDeEsserAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    titleLabel.setBounds (bounds.removeFromTop (44).withTrimmedLeft (16));

    auto content = bounds.reduced (24, 20);

    // GR meter column on the right
    auto meterColumn = content.removeFromRight (90);
    grLabel.setBounds (meterColumn.removeFromBottom (16));
    meterColumn.removeFromBottom (6);
    grMeter.setBounds (meterColumn.reduced (10, 0));

    content.removeFromRight (16);

    // Knob row
    auto knobRow = content.removeFromTop (200);
    const int knobWidth = knobRow.getWidth() / 3;

    auto layoutKnob = [] (juce::Rectangle<int> area, juce::Slider& slider, juce::Label& label)
    {
        auto labelArea = area.removeFromTop (20);
        label.setBounds (labelArea);
        slider.setBounds (area.reduced (14, 4));
    };

    layoutKnob (knobRow.removeFromLeft (knobWidth), thresholdSlider, thresholdLabel);
    layoutKnob (knobRow.removeFromLeft (knobWidth), frequencySlider, frequencyLabel);
    layoutKnob (knobRow, rangeSlider, rangeLabel);

    content.removeFromTop (12);

    // Mode + Listen row
    auto controlsRow = content.removeFromTop (56);
    auto modeArea = controlsRow.removeFromLeft (controlsRow.getWidth() / 2).reduced (8, 4);
    modeLabel.setBounds (modeArea.removeFromTop (16));
    modeBox.setBounds (modeArea.reduced (0, 2));

    auto listenArea = controlsRow.reduced (8, 12);
    listenButton.setBounds (listenArea);
}
