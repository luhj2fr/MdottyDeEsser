// Source/PluginProcessor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyDeEsserAudioProcessor::MdottyDeEsserAudioProcessor() {}
MdottyDeEsserAudioProcessor::~MdottyDeEsserAudioProcessor() {}

void MdottyDeEsserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    deEsser.prepare(sampleRate);
}

void MdottyDeEsserAudioProcessor::releaseResources() {}

void MdottyDeEsserAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, 
                                               juce::MidiBuffer&) {
    deEsser.processBlock(buffer, params);
}

juce::AudioProcessorEditor* MdottyDeEsserAudioProcessor::createEditor() {
    return new MdottyDeEsserAudioProcessorEditor(*this);
}

void MdottyDeEsserAudioProcessor::setParameter(const juce::String& name, float value) {
    if (name == "frequency") params.frequency = value;
    else if (name == "threshold") params.threshold = value;
    else if (name == "ratio") params.ratio = value;
    else if (name == "range") params.range = value;
    else if (name == "lookahead") params.lookahead = value;
    else if (name == "makeup") params.makeup = value;
    else if (name == "output") params.output = value;
    else if (name == "monitor") params.monitor = (value > 0.5f);
}

void MdottyDeEsserAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto tree = std::make_unique<juce::XmlElement>("MdottyDeEsser");
    tree->setAttribute("frequency", params.frequency);
    tree->setAttribute("threshold", params.threshold);
    tree->setAttribute("ratio", params.ratio);
    tree->setAttribute("range", params.range);
    copyXmlToBinary(*tree, destData);
}

void MdottyDeEsserAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    auto tree = getXmlFromBinary(data, sizeInBytes);
    if (tree) {
        params.frequency = tree->getDoubleAttribute("frequency", 4500.0);
        params.threshold = tree->getDoubleAttribute("threshold", -20.0);
        params.ratio = tree->getDoubleAttribute("ratio", 4.0);
        params.range = tree->getDoubleAttribute("range", 12.0);
    }
}