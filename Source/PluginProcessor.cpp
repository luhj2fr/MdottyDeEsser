#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyDeEsserAudioProcessor::MdottyDeEsserAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    thresholdParam = apvts.getRawParameterValue (thresholdId);
    frequencyParam = apvts.getRawParameterValue (frequencyId);
    rangeParam     = apvts.getRawParameterValue (rangeId);
    modeParam      = apvts.getRawParameterValue (modeId);
    listenParam    = apvts.getRawParameterValue (listenId);
}

MdottyDeEsserAudioProcessor::~MdottyDeEsserAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout MdottyDeEsserAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { thresholdId, 1 }, "Threshold",
        juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -24.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { frequencyId, 1 }, "Frequency",
        juce::NormalisableRange<float> (2000.0f, 12000.0f, 1.0f, 0.5f), 6000.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { rangeId, 1 }, "Range",
        juce::NormalisableRange<float> (0.0f, 24.0f, 0.1f), 12.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { modeId, 1 }, "Mode",
        juce::StringArray { "Split-band", "Wideband" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { listenId, 1 }, "Listen", false));

    return { params.begin(), params.end() };
}

void MdottyDeEsserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) juce::jmax (1, getTotalNumOutputChannels());

    deEsser.prepare (spec);
}

void MdottyDeEsserAudioProcessor::releaseResources()
{
    deEsser.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MdottyDeEsserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();

    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();

    if (mainOut != mono && mainOut != stereo)
        return false;

    // Support mono and stereo; in/out channel counts must match.
    return mainIn == mainOut;
}
#endif

void MdottyDeEsserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused (midi);
    juce::ScopedNoDenormals noDenormals;

    const auto totalIn = getTotalNumInputChannels();
    const auto totalOut = getTotalNumOutputChannels();
    for (auto ch = totalIn; ch < totalOut; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    const auto mode = modeParam->load() < 0.5f ? DeEsserDSP::Mode::splitBand
                                                : DeEsserDSP::Mode::wideband;

    deEsser.setParameters (thresholdParam->load(), frequencyParam->load(),
                            rangeParam->load(), mode, listenParam->load() > 0.5f);

    juce::dsp::AudioBlock<float> block (buffer);
    deEsser.process (block);
}

juce::AudioProcessorEditor* MdottyDeEsserAudioProcessor::createEditor()
{
    return new MdottyDeEsserAudioProcessorEditor (*this);
}

void MdottyDeEsserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); true)
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }
}

void MdottyDeEsserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// This creates new instances of the plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MdottyDeEsserAudioProcessor();
}
