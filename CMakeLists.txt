cmake_minimum_required(VERSION 3.22)
project(MdottyDeEsser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.9
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(
    MdottyDeEsser
    VERSION 1.0.0
    DESCRIPTION "Pro-grade de-esser for sibilance reduction"
    MANUFACTURER_CODE Mdot
    PLUGIN_CODE Mdds
    PLUGIN_MANUFACTURER_EMAIL "contact@mdotty.audio"
    PLUGIN_MANUFACTURER_WEBSITE "https://mdotty.audio"
    FORMATS VST3 AU Standalone
    COPY_PLUGIN_AFTER_BUILD TRUE
    NEEDS_MIDI_OUTPUT FALSE
    NEEDS_MIDI_INPUT FALSE
    IS_SYNTH FALSE
)

target_sources(MdottyDeEsser PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
)

target_compile_definitions(MdottyDeEsser PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MdottyDeEsser PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)