# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(MdottyDeEsser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE
add_subdirectory(JUCE)

juce_add_plugin(MdottyDeEsser
    COMPANY_NAME "Mdotty"
    PLUGIN_MANUFACTURER_CODE Mdot
    PLUGIN_CODE Mdes
    FORMATS VST3
    PRODUCT_NAME "MdottyDeEsser"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

target_sources(MdottyDeEsser PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/DeEsserProcessor.cpp
)

target_compile_definitions(MdottyDeEsser PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MdottyDeEsser PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)