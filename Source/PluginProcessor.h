// Source/PluginProcessor.h
#pragma once
#include <JuceHeader.h>
#include "DSP/DeEsserProcessor.h"

class MdottyDeEsserAudioProcessor : public juce::AudioProcessor {
public:
    MdottyDeEsserAudioProcessor();
    ~MdottyDeEsserAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    
    const juce::String getName() const override { return "MdottyDeEsser"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter access
    DeEsserDSP::Parameters getParameters() const { return params; }
    void setParameter(const juce::String& name, float value);

private:
    DeEsserDSP deEsser;
    DeEsserDSP::Parameters params;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MdottyDeEsserAudioProcessor)
};