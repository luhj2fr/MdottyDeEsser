// Source/DSP/DeEsserProcessor.cpp
#include "DeEsserProcessor.h"

void DeEsserDSP::prepare(double sr) {
    sampleRate = sr;
    int lookAheadSamples = static_cast<int>(sampleRate * 0.01); // 10ms max
    lookAheadBuffer.resize(lookAheadSamples, 0.0f);
    lookAheadPos = 0;
}

void DeEsserDSP::updateBiquadPeakCoefficients(float freq, float Q, float gain) {
    float w0 = 2.0f * juce::MathConstants<float>::pi * freq / sampleRate;
    float alpha = std::sin(w0) / (2.0f * Q);
    float A = std::pow(10.0f, gain / 40.0f);
    
    biquad.b0 = 1.0f + alpha * A;
    biquad.b1 = -2.0f * std::cos(w0);
    biquad.b2 = 1.0f - alpha * A;
    biquad.a1 = -2.0f * std::cos(w0);
    biquad.a2 = 1.0f - alpha;
    
    float a0 = 1.0f + alpha / A;
    biquad.b0 /= a0;
    biquad.b1 /= a0;
    biquad.b2 /= a0;
    biquad.a1 /= a0;
    biquad.a2 /= a0;
}

float DeEsserDSP::processSiblanceDetection(const float* samples, int numSamples, 
                                          float freq, float Q) {
    float maxEnergy = 0.0f;
    
    for (int i = 0; i < numSamples; ++i) {
        float in = samples[i];
        float out = biquad.b0 * in + biquad.b1 * z1 + biquad.b2 * z2
                  - biquad.a1 * z1 - biquad.a2 * z2;
        
        z1 = out;
        z2 = z1;
        
        maxEnergy = std::max(maxEnergy, std::abs(out));
    }
    
    return 20.0f * std::log10(maxEnergy + 1e-9f);
}

void DeEsserDSP::applyGainReduction(juce::AudioBuffer<float>& buffer, 
                                    const float* gainReduction, int numSamples) {
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
    
    for (int i = 0; i < numSamples; ++i) {
        float gain = std::pow(10.0f, gainReduction[i] / 20.0f);
        left[i] *= gain;
        if (right) right[i] *= gain;
    }
}

void DeEsserDSP::processBlock(juce::AudioBuffer<float>& buffer, const Parameters& params) {
    int numSamples = buffer.getNumSamples();
    updateBiquadPeakCoefficients(params.frequency, params.bandWidth, 12.0f);
    
    // Detect sibilance
    auto* left = buffer.getReadPointer(0);
    float sibEnergy = processSiblanceDetection(left, numSamples, params.frequency, params.bandWidth);
    
    // Calculate gain reduction
    std::vector<float> gainReduction(numSamples, 0.0f);
    float thresholdLinear = std::pow(10.0f, params.threshold / 20.0f);
    
    if (sibEnergy > params.threshold) {
        float excessDB = sibEnergy - params.threshold;
        float reduction = excessDB * (params.ratio - 1.0f) / params.ratio;
        reduction = std::min(reduction, params.range);
        
        // Apply lookahead + smooth envelope
        int lookaheadSamps = static_cast<int>(params.lookahead * sampleRate / 1000.0f);
        float envValue = std::pow(10.0f, -reduction / 20.0f);
        
        for (int i = 0; i < numSamples; ++i) {
            gainReduction[i] = -reduction;
        }
    }
    
    applyGainReduction(buffer, gainReduction.data(), numSamples);
    
    // Makeup gain + output
    float makeupGain = std::pow(10.0f, (params.makeup + params.output) / 20.0f);
    buffer.applyGain(makeupGain);
    
    // Listen mode: output only detected sibilance
    if (params.monitor) {
        buffer.clear();
    }
}