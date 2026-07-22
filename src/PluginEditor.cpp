#include "PluginEditor.h"

MdottyDeEsserEditor::MdottyDeEsserEditor(MdottyDeEsserProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p) {
    setSize(800, 300);
    setResizable(false, false);

    // Setup sliders
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(frequencySlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(lookaheadSlider);
    addAndMakeVisible(outputGainSlider);

    // Setup labels
    auto setupLabel = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setFont(12.f);
        label.setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
        addAndMakeVisible(label);
    };

    setupLabel(inputLabel, "INPUT GAIN");
    setupLabel(freqLabel, "FREQUENCY");
    setupLabel(threshLabel, "THRESHOLD");
    setupLabel(ratioLabel, "RATIO");
    setupLabel(lookaheadLabel, "LOOKAHEAD");
    setupLabel(outputLabel, "OUTPUT GAIN");

    // Attachments
    inputAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "inputGain", inputGainSlider);
    freqAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "frequency", frequencySlider);
    threshAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "threshold", thresholdSlider);
    ratioAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "ratio", ratioSlider);
    lookaheadAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "lookahead", lookaheadSlider);
    outputAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "outputGain", outputGainSlider);
}

MdottyDeEsserEditor::~MdottyDeEsserEditor() {}

void MdottyDeEsserEditor::paint(juce::Graphics& g) {
    // Dark futuristic background
    g.fillAll(juce::Colour(0xFF0F0F1E));

    // Header bar
    juce::ColourGradient grad(juce::Colour(0xFF1A1A2E), 0, 0,
                              juce::Colour(0xFF00D9FF).withAlpha(0.1f), getWidth(), 0, false);
    g.setGradientFill(grad);
    g.fillRect(0, 0, getWidth(), 50);

    // Title
    g.setColour(juce::Colour(0xFF00D9FF));
    g.setFont(juce::Font(28.f, juce::Font::bold));
    g.drawText("MdottyDeEsser", 20, 10, 300, 30, juce::Justification::left);

    // Grid lines for visual depth
    g.setColour(juce::Colour(0xFF00D9FF).withAlpha(0.05f));
    for (int i = 0; i < getWidth(); i += 50) {
        g.drawVerticalLine(i, 50.f, static_cast<float>(getHeight()));
    }
}

void MdottyDeEsserEditor::resized() {
    auto setupSlider = [this](ModernSlider& slider, juce::Label& label, int x) {
        label.setBounds(x, 60, 120, 20);
        slider.setBounds(x, 85, 120, 40);
    };

    setupSlider(inputGainSlider, inputLabel, 20);
    setupSlider(frequencySlider, freqLabel, 150);
    setupSlider(thresholdSlider, threshLabel, 280);
    setupSlider(ratioSlider, ratioLabel, 410);
    setupSlider(lookaheadSlider, lookaheadLabel, 540);
    setupSlider(outputGainSlider, outputLabel, 670);
}