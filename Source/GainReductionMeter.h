#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
    A vertical gain-reduction meter: fills from the top downward as more
    reduction is applied (Waves/SSL style GR meter convention), with a
    neon-glow fill over a dark track and dB gridlines.
*/
class GainReductionMeter : public juce::Component,
                            private juce::Timer
{
public:
    GainReductionMeter();
    ~GainReductionMeter() override;

    // Called from the editor with a thread-safe getter into the processor.
    void setValueSource (std::function<float()> getGainReductionDb);

    void paint (juce::Graphics& g) override;

private:
    void timerCallback() override;

    std::function<float()> valueSource;
    float displayedReductionDb = 0.0f;
    float maxRangeDb = 24.0f;
};
