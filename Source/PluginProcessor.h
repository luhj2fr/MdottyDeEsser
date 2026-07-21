#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DeEsserDSP.h"

class MdottyDeEsserAudioProcessor : public juce::AudioProcessor
{
public:
    MdottyDeEsserAudioProcessor();
    ~MdottyDeEsserAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;

    // Thread-safe accessor for the GUI's GR meter.
    float getCurrentGainReductionDb() const { return deEsser.getCurrentGainReductionDb(); }

    // Parameter IDs, shared with the editor.
    static constexpr const char* thresholdId = "threshold";
    static constexpr const char* frequencyId = "frequency";
    static constexpr const char* rangeId     = "range";
    static constexpr const char* modeId      = "mode";
    static constexpr const char* listenId    = "listen";

private:
    DeEsserDSP deEsser;

    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* frequencyParam = nullptr;
    std::atomic<float>* rangeParam     = nullptr;
    std::atomic<float>* modeParam      = nullptr;
    std::atomic<float>* listenParam    = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MdottyDeEsserAudioProcessor)
};
