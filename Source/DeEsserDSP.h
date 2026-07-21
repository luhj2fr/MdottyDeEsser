#pragma once

#include <juce_dsp/juce_dsp.h>
#include <atomic>

/**
    Core DSP engine for MdottyDeEsser.

    Architecture (per channel):
      1. A band-splitting filter pair isolates the "sibilant band" centred on
         a user-adjustable frequency (the detection/processing band).
      2. An envelope follower (RMS-style, attack/release smoothed) tracks the
         level of that band.
      3. A gain-reduction curve derived from Threshold + Range converts the
         detected level into a reduction amount, in dB, clamped to Range.
      4. In Split-band mode, that reduction is applied ONLY to the sibilant
         band before it's summed back with the untouched low band (classic
         Waves-style de-essing). In Wideband mode, the same detected
         reduction is applied to the entire signal instead (behaves like a
         frequency-triggered compressor).
      5. Listen mode solos the sibilant band post-filter (pre gain reduction)
         so the user can tune Frequency/Threshold by ear.
*/
class DeEsserDSP
{
public:
    enum class Mode
    {
        splitBand = 0,
        wideband
    };

    DeEsserDSP() = default;

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        numChannels = (int) spec.numChannels;

        for (auto& f : sibilantFilters)
            f.prepare (spec);

        envelopeState.assign ((size_t) numChannels, 0.0f);
        gainReductionDb.store (0.0f);

        updateFilter();
        updateEnvelopeCoefficients();
    }

    void reset()
    {
        for (auto& f : sibilantFilters)
            f.reset();

        std::fill (envelopeState.begin(), envelopeState.end(), 0.0f);
        gainReductionDb.store (0.0f);
    }

    void setParameters (float thresholdDb, float frequencyHz, float rangeDb,
                         Mode newMode, bool listenEnabled)
    {
        threshold = thresholdDb;
        range = rangeDb;
        mode = newMode;
        listen = listenEnabled;

        if (! juce::approximatelyEqual (frequencyHz, frequency))
        {
            frequency = frequencyHz;
            filterNeedsUpdate = true;
        }
    }

    // Processes a block in place. `block` must have numChannels channels
    // matching what was passed to prepare().
    void process (juce::dsp::AudioBlock<float>& block)
    {
        if (filterNeedsUpdate.exchange (false))
            updateFilter();

        const int numSamples = (int) block.getNumSamples();
        const int chans = juce::jmin ((int) block.getNumChannels(), numChannels);

        float blockPeakReductionDb = 0.0f;

        for (int ch = 0; ch < chans; ++ch)
        {
            auto* data = block.getChannelPointer ((size_t) ch);
            auto& filter = sibilantFilters[(size_t) ch];
            float& envelope = envelopeState[(size_t) ch];

            for (int n = 0; n < numSamples; ++n)
            {
                const float input = data[n];

                // Extract the sibilant (high) band via the bandpass filter.
                const float sibilant = filter.processSample (input);
                // The complementary low band is simply the residual.
                const float low = input - sibilant;

                // --- Envelope follower (RMS-ish, one-pole attack/release) ---
                const float rectified = sibilant * sibilant; // squared -> power
                const float coeff = (rectified > envelope) ? attackCoeff : releaseCoeff;
                envelope = coeff * envelope + (1.0f - coeff) * rectified;
                const float rms = std::sqrt (juce::jmax (envelope, 1.0e-12f));
                const float levelDb = juce::Decibels::gainToDecibels (rms, -100.0f);

                // --- Gain reduction curve ---
                // Soft-knee-ish: reduction ramps once level exceeds threshold,
                // clamped to the user's Range (max reduction).
                float reductionDb = 0.0f;
                if (levelDb > threshold)
                {
                    const float over = levelDb - threshold;
                    // ~2:1 detector-to-reduction slope feels natural for de-essing;
                    // clamp to the configured max Range.
                    reductionDb = juce::jmin (over * 2.0f, range);
                }

                blockPeakReductionDb = juce::jmax (blockPeakReductionDb, reductionDb);

                const float reductionGain = juce::Decibels::decibelsToGain (-reductionDb);

                float output;

                if (listen)
                {
                    // Solo the detected sibilant band, post-filter, pre-reduction,
                    // so the user can tune Frequency/Threshold by ear.
                    output = sibilant;
                }
                else if (mode == Mode::splitBand)
                {
                    // Only the high band gets ducked; low band passes untouched.
                    output = low + sibilant * reductionGain;
                }
                else // wideband
                {
                    // Apply the same detected reduction to the full signal.
                    output = (low + sibilant) * reductionGain;
                }

                data[n] = output;
            }
        }

        gainReductionDb.store (blockPeakReductionDb);
    }

    // Thread-safe read for the GUI meter (peak GR in dB, positive = reduction).
    float getCurrentGainReductionDb() const { return gainReductionDb.load(); }

private:
    void updateFilter()
    {
        if (sampleRate <= 0.0)
            return;

        // Bandpass tuned around `frequency`, wide enough to capture the
        // sibilant range but centred where the user points it.
        const float q = 0.7f;
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass (
            sampleRate, juce::jlimit (1000.0f, (float) (sampleRate * 0.45), frequency), q);

        for (auto& f : sibilantFilters)
            *f.coefficients = *coeffs;
    }

    void updateEnvelopeCoefficients()
    {
        constexpr float attackMs = 1.5f;
        constexpr float releaseMs = 60.0f;

        attackCoeff = std::exp (-1.0f / (float) (0.001 * attackMs * sampleRate));
        releaseCoeff = std::exp (-1.0f / (float) (0.001 * releaseMs * sampleRate));
    }

    double sampleRate = 44100.0;
    int numChannels = 2;

    float threshold = -24.0f;
    float frequency = 6000.0f;
    float range = 12.0f;
    Mode mode = Mode::splitBand;
    bool listen = false;

    std::atomic<bool> filterNeedsUpdate { true };

    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    std::vector<float> envelopeState;

    std::array<juce::dsp::IIR::Filter<float>, 2> sibilantFilters;

    std::atomic<float> gainReductionDb { 0.0f };
};
