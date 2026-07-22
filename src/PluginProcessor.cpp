#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyDeEsserProcessor::MdottyDeEsserProcessor()
    : apvts(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

MdottyDeEsserProcessor::~MdottyDeEsserProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MdottyDeEsserProcessor::createParameterLayout() {
    using Parameter = juce::AudioProcessorValueTreeState::Parameter;
    std::vector<std::unique_ptr<Parameter>> params;

    params.push_back(std::make_unique<Parameter>(
        "inputGain", "Input Gain", "dB",
        juce::NormalisableRange<float>(-12.f, 12.f, 0.1f), 0.0f,
        [](float value) { return juce::String(value, 1); },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    params.push_back(std::make_unique<Parameter>(
        "frequency", "Frequency", "Hz",
        juce::NormalisableRange<float>(80.f, 8000.f, 1.f, 0.5f), 4000.f,
        [](float value) { return juce::String(static_cast<int>(value)); },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    params.push_back(std::make_unique<Parameter>(
        "threshold", "Threshold", "dB",
        juce::NormalisableRange<float>(-60.f, 0.f, 0.1f), -20.f,
        [](float value) { return juce::String(value, 1); },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    params.push_back(std::make_unique<Parameter>(
        "ratio", "Ratio", ":1",
        juce::NormalisableRange<float>(1.f, 10.f, 0.1f), 4.0f,
        [](float value) { return juce::String(value, 1); },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    params.push_back(std::make_unique<Parameter>(
        "lookahead", "Lookahead", "ms",
        juce::NormalisableRange<float>(0.f, 20.f, 0.5f), 5.0f,
        [](float value) { return juce::String(value, 1); },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    params.push_back(std::make_unique<Parameter>(
        "outputGain", "Output Gain", "dB",
        juce::NormalisableRange<float>(-12.f, 12.f, 0.1f), 0.0f,
        [](float value) { return juce::String(value, 1); },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    params.push_back(std::make_unique<Parameter>(
        "makeup", "Auto Makeup", "",
        juce::NormalisableRange<float>(0.f, 1.f, 1.f), 1.0f,
        [](float value) { return value > 0.5f ? "On" : "Off"; },
        [](const juce::String& text) { return text == "On" ? 1.0f : 0.0f; }
    ));

    return { params.begin(), params.end() };
}

void MdottyDeEsserProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    deEsserL.prepare(sampleRate, samplesPerBlock);
    deEsserR.prepare(sampleRate, samplesPerBlock);
}

void MdottyDeEsserProcessor::releaseResources() {}

void MdottyDeEsserProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    auto inputGain = apvts.getRawParameterValue("inputGain")->load();
    auto frequency = apvts.getRawParameterValue("frequency")->load();
    auto threshold = apvts.getRawParameterValue("threshold")->load();
    auto ratio = apvts.getRawParameterValue("ratio")->load();
    auto lookahead = apvts.getRawParameterValue("lookahead")->load();
    auto outputGain = apvts.getRawParameterValue("outputGain")->load();
    auto makeup = apvts.getRawParameterValue("makeup")->load() > 0.5f;

    // Apply input gain
    buffer.applyGain(juce::Decibels::decibelsToGain(inputGain));

    // Process left channel
    if (buffer.getNumChannels() > 0) {
        auto* left = buffer.getWritePointer(0);
        deEsserL.process(left, buffer.getNumSamples(), frequency, threshold, ratio, lookahead, makeup);
    }

    // Process right channel
    if (buffer.getNumChannels() > 1) {
        auto* right = buffer.getWritePointer(1);
        deEsserR.process(right, buffer.getNumSamples(), frequency, threshold, ratio, lookahead, makeup);
    }

    // Apply output gain
    buffer.applyGain(juce::Decibels::decibelsToGain(outputGain));

    // Soft clip to prevent clipping
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        auto* samples = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            if (samples[i] > 1.f) samples[i] = std::tanh(samples[i]);
            if (samples[i] < -1.f) samples[i] = std::tanh(samples[i]);
        }
    }
}

void MdottyDeEsserProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyValueTree();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MdottyDeEsserProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(apvts.state.getType())) {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

juce::AudioProcessorEditor* MdottyDeEsserProcessor::createEditor() {
    return new MdottyDeEsserEditor(*this);
}