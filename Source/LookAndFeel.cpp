#include "LookAndFeel.h"

const juce::Colour MdottyLookAndFeel::background   { 0xff0d0f14 };
const juce::Colour MdottyLookAndFeel::panel        { 0xff161a22 };
const juce::Colour MdottyLookAndFeel::accent       { 0xff00e5c7 }; // cyan/teal neon
const juce::Colour MdottyLookAndFeel::accentDim    { 0xff0a5f57 };
const juce::Colour MdottyLookAndFeel::text         { 0xffe8f1f2 };
const juce::Colour MdottyLookAndFeel::textDim      { 0xff8a97a3 };

MdottyLookAndFeel::MdottyLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, background);
    setColour (juce::Slider::thumbColourId, accent);
    setColour (juce::Slider::rotarySliderFillColourId, accent);
    setColour (juce::Slider::rotarySliderOutlineColourId, panel);
    setColour (juce::Slider::textBoxTextColourId, text);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Label::textColourId, text);
    setColour (juce::ComboBox::backgroundColourId, panel);
    setColour (juce::ComboBox::textColourId, text);
    setColour (juce::ComboBox::outlineColourId, accentDim);
    setColour (juce::ToggleButton::textColourId, text);
    setColour (juce::PopupMenu::backgroundColourId, panel);
    setColour (juce::PopupMenu::textColourId, text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accentDim);
}

void MdottyLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle,
                                           float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (6.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const float trackThickness = radius * 0.16f;
    const float ringRadius = radius - trackThickness * 0.5f - 2.0f;

    // Outer track (dim)
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, ringRadius, ringRadius, 0.0f,
                          rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (panel.brighter (0.15f));
    g.strokePath (track, juce::PathStrokeType (trackThickness, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    // Value arc (glowing accent)
    if (sliderPos > 0.001f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, ringRadius, ringRadius, 0.0f,
                                 rotaryStartAngle, angle, true);

        // soft glow underlay
        g.setColour (accent.withAlpha (0.25f));
        g.strokePath (valueArc, juce::PathStrokeType (trackThickness * 1.8f,
                                                        juce::PathStrokeType::curved,
                                                        juce::PathStrokeType::rounded));
        g.setColour (accent);
        g.strokePath (valueArc, juce::PathStrokeType (trackThickness, juce::PathStrokeType::curved,
                                                        juce::PathStrokeType::rounded));
    }

    // Knob body
    const float bodyRadius = ringRadius - trackThickness * 0.9f;
    juce::ColourGradient bodyGrad (panel.brighter (0.12f), centre.x, centre.y - bodyRadius,
                                    panel.darker (0.4f), centre.x, centre.y + bodyRadius, false);
    g.setGradientFill (bodyGrad);
    g.fillEllipse (centre.x - bodyRadius, centre.y - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f);

    g.setColour (accentDim.withAlpha (0.8f));
    g.drawEllipse (centre.x - bodyRadius, centre.y - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f, 1.2f);

    // Pointer
    juce::Path pointer;
    const float pointerLength = bodyRadius * 0.78f;
    const float pointerThickness = 2.6f;
    pointer.addRoundedRectangle (-pointerThickness * 0.5f, -pointerLength, pointerThickness,
                                  pointerLength * 0.85f, pointerThickness * 0.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
    g.setColour (juce::Colours::white);
    g.fillPath (pointer);

    // Centre dot
    g.setColour (accent);
    g.fillEllipse (centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
}

void MdottyLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (shouldDrawButtonAsDown);

    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = button.getToggleState();

    juce::Colour fill = on ? accent : panel.brighter (shouldDrawButtonAsHighlighted ? 0.15f : 0.05f);
    juce::Colour border = on ? accent : accentDim;

    g.setColour (fill.withAlpha (on ? 0.9f : 1.0f));
    g.fillRoundedRectangle (bounds, 5.0f);
    g.setColour (border);
    g.drawRoundedRectangle (bounds, 5.0f, 1.4f);

    g.setColour (on ? juce::Colours::black : text);
    g.setFont (juce::Font (juce::FontOptions (14.0f)));

    auto textBounds = bounds.reduced (8.0f, 0.0f);
    g.drawFittedText (button.getButtonText(), textBounds.toNearestInt(),
                       juce::Justification::centred, 1);
}

void MdottyLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                       int, int, int, int, juce::ComboBox& box)
{
    juce::ignoreUnused (isButtonDown);
    auto bounds = juce::Rectangle<float> (0, 0, (float) width, (float) height).reduced (1.0f);

    g.setColour (panel);
    g.fillRoundedRectangle (bounds, 5.0f);
    g.setColour (box.isMouseOver() ? accent : accentDim);
    g.drawRoundedRectangle (bounds, 5.0f, 1.2f);

    // Simple dropdown arrow
    juce::Path arrow;
    const float ax = width - 16.0f;
    const float ay = height * 0.5f;
    arrow.addTriangle (ax - 5.0f, ay - 3.0f, ax + 5.0f, ay - 3.0f, ax, ay + 4.0f);
    g.setColour (accent);
    g.fillPath (arrow);
}

juce::Font MdottyLookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (juce::FontOptions (14.0f, juce::Font::plain));
}

juce::Font MdottyLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return juce::Font (juce::FontOptions (14.0f, juce::Font::plain));
}
