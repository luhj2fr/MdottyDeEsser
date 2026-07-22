#pragma once
#include <juce/JuceHeader.h>
#include "PluginProcessor.h"

class ModernSlider : public juce::Slider {
public:
    ModernSlider() {
        setSliderStyle(juce::Slider::LinearHorizontal);
        setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF00D9FF).withAlpha(0.9f));
        setColour(juce::Slider::trackColourId, juce::Colour(0xFF1A1A2E).withAlpha(0.8f));
        setColour(juce::Slider::backgroundColourId, juce::Colour(0x0F0F1E));
    }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        auto trackHeight = 4.f;
        auto trackY = bounds.getCentreY() - trackHeight / 2.f;

        // Futuristic glow background
        g.setColour(juce::Colour(0xFF1A1A2E).withAlpha(0.3f));
        g.fillRoundedRectangle(bounds.toFloat().withHeight(26.f), 8.f);

        // Animated gradient track
        juce::ColourGradient gradient(juce::Colour(0xFF0099FF), 0, 0,
                                      juce::Colour(0xFF00D9FF), bounds.getWidth(), 0, false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(
            juce::Rectangle<float>(bounds.getX(), trackY, 
                                   getValue() / getMaximum() * bounds.getWidth(), 
                                   trackHeight), 2.f);

        // Draw thumb with glow
        auto thumbX = static_cast<float>(getPositionOfValue(getValue()));
        auto thumbBounds = juce::Rectangle<float>(thumbX - 6.f, bounds.getCentreY() - 10.f, 12.f, 20.f);

        g.setColour(juce::Colour(0xFF00D9FF).withAlpha(0.2f));
        g.fillEllipse(thumbBounds.expanded(4.f));

        g.setColour(juce::Colour(0xFF00D9FF));
        g.fillRoundedRectangle(thumbBounds, 2.f);

        g.setColour(juce::Colour(0xFFFFFFFF).withAlpha(0.1f));
        g.drawRoundedRectangle(thumbBounds, 2.f, 1.f);

        juce::Slider::paint(g);
    }

    void mouseEnter(const juce::MouseEvent&) override {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF00FFFF));
    }

    void mouseExit(const juce::MouseEvent&) override {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF00D9FF).withAlpha(0.9f));
    }
};

class MdottyDeEsserEditor : public juce::AudioProcessorEditor {
public:
    MdottyDeEsserEditor(MdottyDeEsserProcessor&);
    ~MdottyDeEsserEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    MdottyDeEsserProcessor& audioProcessor;

    ModernSlider inputGainSlider, frequencySlider, thresholdSlider, 
                 ratioSlider, lookaheadSlider, outputGainSlider;

    juce::Label inputLabel, freqLabel, threshLabel, ratioLabel, lookaheadLabel, outputLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> inputAttach, freqAttach, threshAttach, 
                                       ratioAttach, lookaheadAttach, outputAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MdottyDeEsserEditor)
};