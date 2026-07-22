// Source/DSP/DeEsserProcessor.h
#pragma once
#include <JuceHeader.h>
#include <cmath>

class DeEsserDSP {
public:
    struct Parameters {
        float frequency = 4500.0f;      // peak frequency (1k - 16k)
        float threshold = -20.0f;        // threshold (dB)
        float ratio = 4.0f;              // ratio (1:1 - 12:1)
        float range = 12.0f;             // max reduction (dB)
        float lookahead = 5.0f;          // lookahead (ms)
        float makeup = 0.0f;             // makeup gain (dB)
        float output = 0.0f;             // output level (dB)
        bool monitor = false;            // listen mode
        float bandWidth = 1.0f;          // Q factor
    };

    void prepare(double sampleRate);
    void processBlock(juce::AudioBuffer<float>& buffer, const Parameters& params);

private:
    double sampleRate = 44100.0;
    std::vector<float> lookAheadBuffer;
    int lookAheadPos = 0;
    
    // Biquad coefficients for peak EQ
    struct BiquadCoef {
        float b0, b1, b2, a1, a2;
    } biquad;
    
    float z1 = 0.0f, z2 = 0.0f; // filter state
    
    void updateBiquadPeakCoefficients(float freq, float Q, float gain);
    float processSiblanceDetection(const float* samples, int numSamples, float freq, float Q);
    void applyGainReduction(juce::AudioBuffer<float>& buffer, 
                           const float* gainReduction, int numSamples);
};