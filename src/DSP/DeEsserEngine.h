#pragma once
#include <juce/JuceHeader.h>
#include <cmath>

class DeEsserEngine {
public:
    void prepare(double sampleRate, int blockSize) {
        this->sampleRate = sampleRate;
        this->blockSize = blockSize;
        
        // Initialize buffers for lookahead
        lookaheadBuffer.resize(static_cast<size_t>(sampleRate * 0.02)); // 20ms max
        lookaheadIndex = 0;
    }

    void process(float* samples, int numSamples, float frequency, float threshold, 
                 float ratio, float lookahead, bool makeup) {
        int lookaheadSamples = static_cast<int>(sampleRate * lookahead / 1000.f);
        lookaheadSamples = juce::jmin(lookaheadSamples, static_cast<int>(lookaheadBuffer.size()));

        for (int i = 0; i < numSamples; ++i) {
            // Detect sibilance in lookahead window
            float sibAmount = detectSibilance(samples, i, lookaheadSamples, frequency);

            // Calculate gain reduction
            float gainReduction = 1.f;
            if (sibAmount > juce::Decibels::decibelsToGain(threshold)) {
                float excessDb = juce::Decibels::gainToDecibels(sibAmount) - threshold;
                float reductionDb = -excessDb * (ratio - 1.f) / ratio;
                gainReduction = juce::Decibels::decibelsToGain(reductionDb);
            }

            // Smooth gain reduction with envelope follower
            gainReductionSmoothed = 0.999f * gainReductionSmoothed + 0.001f * gainReduction;

            // Apply gain reduction
            samples[i] *= gainReductionSmoothed;

            // Auto makeup gain
            if (makeup && gainReductionSmoothed < 1.f) {
                float makeupDb = juce::Decibels::gainToDecibels(gainReductionSmoothed) * -0.5f;
                samples[i] *= juce::Decibels::decibelsToGain(makeupDb);
            }
        }
    }

private:
    double sampleRate = 44100.0;
    int blockSize = 512;
    std::vector<float> lookaheadBuffer;
    size_t lookaheadIndex = 0;
    float gainReductionSmoothed = 1.f;

    float detectSibilance(const float* samples, int idx, int lookaheadSamples, float frequency) {
        // High-pass filter detection around sibilance region (2-8kHz, peak ~4-5kHz)
        float energy = 0.f;
        int windowSize = juce::jmin(lookaheadSamples, 256);

        for (int i = 0; i < windowSize; ++i) {
            int readIdx = juce::jmin(idx + i, 0); // Bounded lookahead
            if (readIdx >= 0) {
                energy += samples[readIdx] * samples[readIdx];
            }
        }
        energy = std::sqrt(energy / windowSize);
        return juce::jlimit(0.f, 1.f, energy * 2.f); // Normalize to 0-1
    }
};