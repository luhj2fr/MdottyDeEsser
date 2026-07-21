#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
    A dark, futuristic LookAndFeel for MdottyDeEsser: glowing-ring rotary
    knobs and neon-accented toggle buttons, replacing JUCE's default grey
    controls entirely.
*/
class MdottyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MdottyLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider& slider) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                        int buttonX, int buttonY, int buttonW, int buttonH,
                        juce::ComboBox& box) override;

    juce::Font getLabelFont (juce::Label&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;

    static const juce::Colour background;
    static const juce::Colour panel;
    static const juce::Colour accent;
    static const juce::Colour accentDim;
    static const juce::Colour text;
    static const juce::Colour textDim;
};
