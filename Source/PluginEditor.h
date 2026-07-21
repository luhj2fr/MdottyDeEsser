#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "GainReductionMeter.h"

class MdottyDeEsserAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MdottyDeEsserAudioProcessorEditor (MdottyDeEsserAudioProcessor&);
    ~MdottyDeEsserAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void configureKnob (juce::Slider& slider, juce::Label& label, const juce::String& text);

    MdottyDeEsserAudioProcessor& processorRef;
    MdottyLookAndFeel lookAndFeel;

    juce::Slider thresholdSlider, frequencySlider, rangeSlider;
    juce::Label thresholdLabel, frequencyLabel, rangeLabel;

    juce::ComboBox modeBox;
    juce::Label modeLabel;

    juce::ToggleButton listenButton { "LISTEN" };

    GainReductionMeter grMeter;
    juce::Label grLabel;

    juce::Label titleLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> thresholdAttachment, frequencyAttachment, rangeAttachment;
    std::unique_ptr<ComboAttachment>  modeAttachment;
    std::unique_ptr<ButtonAttachment> listenAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MdottyDeEsserAudioProcessorEditor)
};
