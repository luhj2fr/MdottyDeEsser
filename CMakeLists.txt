cmake_minimum_required(VERSION 3.22)

project(MdottyDeEsser VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# JUCE
# ---------------------------------------------------------------------------
# Pulls JUCE straight from the official GitHub repo at configure time, so
# nothing needs to be vendored/committed into this repo. Pin a known-good
# tag so builds stay reproducible.
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ---------------------------------------------------------------------------
# Plugin target
# ---------------------------------------------------------------------------
juce_add_plugin(MdottyDeEsser
    COMPANY_NAME                "Mdotty"
    PLUGIN_MANUFACTURER_CODE    Mdty
    PLUGIN_CODE                 Mds1
    FORMATS                     VST3
    PRODUCT_NAME                "MdottyDeEsser"
    VST3_CATEGORIES              Fx EQ
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     TRUE
)
# NOTE: intentionally NOT setting VST3_COPY_DIR — JUCE's default VST3 copy
# step already installs to the standard system location:
#   C:\Program Files\Common Files\VST3\MdottyDeEsser.vst3

target_sources(MdottyDeEsser
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/LookAndFeel.cpp
        Source/GainReductionMeter.cpp
)

target_compile_definitions(MdottyDeEsser
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MdottyDeEsser
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
