// Source/PluginEditor.h
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class FuturisticSlider : public juce::Slider {
public:
    FuturisticSlider();
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;

private:
    bool isHovered = false;
    juce::Colour glowColour = juce::Colour(0xFF00D9FF);
};

class MdottyDeEsserAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit MdottyDeEsserAudioProcessorEditor(MdottyDeEsserAudioProcessor&);
    ~MdottyDeEsserAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MdottyDeEsserAudioProcessor& audioProcessor;
    
    std::unique_ptr<FuturisticSlider> frequencySlider;
    std::unique_ptr<FuturisticSlider> thresholdSlider;
    std::unique_ptr<FuturisticSlider> ratioSlider;
    std::unique_ptr<FuturisticSlider> rangeSlider;
    std::unique_ptr<FuturisticSlider> lookaheadSlider;
    std::unique_ptr<FuturisticSlider> makeupSlider;
    std::unique_ptr<FuturisticSlider> outputSlider;
    
    std::unique_ptr<juce::Label> frequencyLabel;
    std::unique_ptr<juce::Label> thresholdLabel;
    std::unique_ptr<juce::Label> ratioLabel;
    std::unique_ptr<juce::Label> rangeLabel;
    std::unique_ptr<juce::Label> lookaheadLabel;
    std::unique_ptr<juce::Label> makeupLabel;
    std::unique_ptr<juce::Label> outputLabel;
    
    juce::ToggleButton monitorButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MdottyDeEsserAudioProcessorEditor)
};