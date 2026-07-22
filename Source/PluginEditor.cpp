// Source/PluginEditor.cpp
#include "PluginEditor.h"

FuturisticSlider::FuturisticSlider() {
    setSliderStyle(juce::Slider::LinearHorizontal);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF0A0E27));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xFF00D9FF));
    setColour(juce::Slider::trackColourId, juce::Colour(0xFF1A2947));
}

void FuturisticSlider::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    float trackY = bounds.getCentreY() - 2.0f;
    
    // Background track
    g.setColour(juce::Colour(0xFF1A2947));
    g.fillRoundedRectangle(bounds.getX(), trackY, bounds.getWidth(), 4.0f, 2.0f);
    
    // Filled track
    float thumbPos = (getValueNormalized() * bounds.getWidth());
    g.setColour(isHovered ? juce::Colour(0xFF00F0FF) : juce::Colour(0xFF00D9FF));
    g.fillRoundedRectangle(bounds.getX(), trackY, thumbPos, 4.0f, 2.0f);
    
    // Glow effect
    if (isHovered) {
        g.setColour(juce::Colour(0xFF00D9FF).withAlpha(0.3f));
        g.fillRoundedRectangle(bounds.getX(), trackY - 4.0f, thumbPos, 12.0f, 2.0f);
    }
    
    // Thumb
    float thumbX = thumbPos - 6.0f;
    g.setColour(isHovered ? juce::Colour(0xFF00F0FF) : juce::Colour(0xFF00D9FF));
    g.fillRoundedRectangle(thumbX, trackY - 6.0f, 12.0f, 16.0f, 2.0f);
    
    // Thumb shadow
    g.setColour(juce::Colour(0xFF00D9FF).withAlpha(0.2f));
    g.drawRoundedRectangle(thumbX - 1.0f, trackY - 7.0f, 14.0f, 18.0f, 2.0f, 2.0f);
    
    // Text
    auto textBounds = bounds.removeFromBottom(25.0f);
    g.setColour(juce::Colour(0xFFCCCCCC));
    g.setFont(12.0f);
    g.drawText(getTextFromValue(getValue()), textBounds, juce::Justification::centred);
}

void FuturisticSlider::mouseEnter(const juce::MouseEvent&) {
    isHovered = true;
    repaint();
}

void FuturisticSlider::mouseExit(const juce::MouseEvent&) {
    isHovered = false;
    repaint();
}

MdottyDeEsserAudioProcessorEditor::MdottyDeEsserAudioProcessorEditor(
    MdottyDeEsserAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(600, 520);
    setResizable(false, false);
    
    // Frequency slider
    frequencySlider = std::make_unique<FuturisticSlider>();
    frequencySlider->setRange(1000.0, 16000.0, 1.0);
    frequencySlider->setValue(4500.0);
    frequencySlider->onValueChange = [this] {
        audioProcessor.setParameter("frequency", frequencySlider->getValue());
    };
    addAndMakeVisible(*frequencySlider);
    
    // Threshold slider
    thresholdSlider = std::make_unique<FuturisticSlider>();
    thresholdSlider->setRange(-60.0, 0.0, 0.1);
    thresholdSlider->setValue(-20.0);
    thresholdSlider->onValueChange = [this] {
        audioProcessor.setParameter("threshold", thresholdSlider->getValue());
    };
    addAndMakeVisible(*thresholdSlider);
    
    // Ratio slider
    ratioSlider = std::make_unique<FuturisticSlider>();
    ratioSlider->setRange(1.0, 12.0, 0.1);
    ratioSlider->setValue(4.0);
    ratioSlider->onValueChange = [this] {
        audioProcessor.setParameter("ratio", ratioSlider->getValue());
    };
    addAndMakeVisible(*ratioSlider);
    
    // Range slider
    rangeSlider = std::make_unique<FuturisticSlider>();
    rangeSlider->setRange(1.0, 24.0, 0.1);
    rangeSlider->setValue(12.0);
    rangeSlider->onValueChange = [this] {
        audioProcessor.setParameter("range", rangeSlider->getValue());
    };
    addAndMakeVisible(*rangeSlider);
    
    // Lookahead slider
    lookaheadSlider = std::make_unique<FuturisticSlider>();
    lookaheadSlider->setRange(0.0, 10.0, 0.1);
    lookaheadSlider->setValue(5.0);
    lookaheadSlider->onValueChange = [this] {
        audioProcessor.setParameter("lookahead", lookaheadSlider->getValue());
    };
    addAndMakeVisible(*lookaheadSlider);
    
    // Makeup slider
    makeupSlider = std::make_unique<FuturisticSlider>();
    makeupSlider->setRange(-12.0, 12.0, 0.1);
    makeupSlider->setValue(0.0);
    makeupSlider->onValueChange = [this] {
        audioProcessor.setParameter("makeup", makeupSlider->getValue());
    };
    addAndMakeVisible(*makeupSlider);
    
    // Output slider
    outputSlider = std::make_unique<FuturisticSlider>();
    outputSlider->setRange(-24.0, 24.0, 0.1);
    outputSlider->setValue(0.0);
    outputSlider->onValueChange = [this] {
        audioProcessor.setParameter("output", outputSlider->getValue());
    };
    addAndMakeVisible(*outputSlider);
    
    // Labels
    frequencyLabel = std::make_unique<juce::Label>("", "Frequency (Hz)");
    frequencyLabel->setFont(12.0f);
    frequencyLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*frequencyLabel);
    
    thresholdLabel = std::make_unique<juce::Label>("", "Threshold (dB)");
    thresholdLabel->setFont(12.0f);
    thresholdLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*thresholdLabel);
    
    ratioLabel = std::make_unique<juce::Label>("", "Ratio");
    ratioLabel->setFont(12.0f);
    ratioLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*ratioLabel);
    
    rangeLabel = std::make_unique<juce::Label>("", "Range (dB)");
    rangeLabel->setFont(12.0f);
    rangeLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*rangeLabel);
    
    lookaheadLabel = std::make_unique<juce::Label>("", "Lookahead (ms)");
    lookaheadLabel->setFont(12.0f);
    lookaheadLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*lookaheadLabel);
    
    makeupLabel = std::make_unique<juce::Label>("", "Makeup Gain (dB)");
    makeupLabel->setFont(12.0f);
    makeupLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*makeupLabel);
    
    outputLabel = std::make_unique<juce::Label>("", "Output (dB)");
    outputLabel->setFont(12.0f);
    outputLabel->setColour(juce::Label::textColourId, juce::Colour(0xFF00D9FF));
    addAndMakeVisible(*outputLabel);
    
    // Monitor button
    monitorButton.setButtonText("Listen");
    monitorButton.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFF00D9FF));
    monitorButton.onClick = [this] {
        audioProcessor.setParameter("monitor", monitorButton.getToggleState() ? 1.0f : 0.0f);
    };
    addAndMakeVisible(monitorButton);
}

MdottyDeEsserAudioProcessorEditor::~MdottyDeEsserAudioProcessorEditor() {}

void MdottyDeEsserAudioProcessorEditor::paint(juce::Graphics& g) {
    // Futuristic dark background
    g.fillAll(juce::Colour(0xFF0A0E27));
    
    // Subtle grid lines
    g.setColour(juce::Colour(0xFF1A2947).withAlpha(0.3f));
    for (int i = 0; i < getWidth(); i += 50) {
        g.drawVerticalLine(i, 0.0f, (float)getHeight());
    }
    
    // Title
    g.setColour(juce::Colour(0xFF00D9FF));
    g.setFont(24.0f);
    g.drawText("MdottyDeEsser", 20, 10, 200, 30, juce::Justification::left);
}

void MdottyDeEsserAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    auto sliderHeight = 60;
    
    bounds.removeFromTop(40);
    
    frequencyLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    frequencySlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    thresholdLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    thresholdSlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    ratioLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    ratioSlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    rangeLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    rangeSlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    lookaheadLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    lookaheadSlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    makeupLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    makeupSlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    outputLabel->setBounds(bounds.getX(), bounds.getY(), 150, 20);
    outputSlider->setBounds(bounds.getX(), bounds.getY() + 20, bounds.getWidth(), sliderHeight);
    bounds.removeFromTop(sliderHeight + 25);
    
    monitorButton.setBounds(bounds.getX(), bounds.getY(), 100, 30);
}