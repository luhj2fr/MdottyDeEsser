#include "GainReductionMeter.h"
#include "LookAndFeel.h"

GainReductionMeter::GainReductionMeter()
{
    startTimerHz (30);
}

GainReductionMeter::~GainReductionMeter()
{
    stopTimer();
}

void GainReductionMeter::setValueSource (std::function<float()> getGainReductionDb)
{
    valueSource = std::move (getGainReductionDb);
}

void GainReductionMeter::timerCallback()
{
    const float target = valueSource ? valueSource() : 0.0f;
    // Fast attack, slower fall-off so the meter reads clearly.
    if (target > displayedReductionDb)
        displayedReductionDb = target;
    else
        displayedReductionDb += (target - displayedReductionDb) * 0.25f;

    repaint();
}

void GainReductionMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (MdottyLookAndFeel::panel);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (MdottyLookAndFeel::accentDim);
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    auto meterArea = bounds.reduced (3.0f);
    const float proportion = juce::jlimit (0.0f, 1.0f, displayedReductionDb / juce::jmax (1.0f, maxRangeDb));
    const float fillHeight = meterArea.getHeight() * proportion;

    auto fillRect = meterArea.removeFromTop (fillHeight);

    juce::ColourGradient grad (juce::Colour (0xffff5a5a), fillRect.getX(), fillRect.getY(),
                                MdottyLookAndFeel::accent, fillRect.getX(), fillRect.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (fillRect, 3.0f);

    // Gridlines every 6 dB
    g.setColour (MdottyLookAndFeel::textDim.withAlpha (0.4f));
    for (float d = 6.0f; d < maxRangeDb; d += 6.0f)
    {
        const float y = bounds.getY() + bounds.getHeight() * (d / maxRangeDb);
        g.drawHorizontalLine ((int) y, bounds.getX(), bounds.getRight());
    }

    g.setColour (MdottyLookAndFeel::textDim);
    g.setFont (juce::Font (juce::FontOptions (10.0f)));
    g.drawFittedText ("GR", getLocalBounds().removeFromBottom (14),
                       juce::Justification::centred, 1);
}
