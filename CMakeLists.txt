cmake_minimum_required(VERSION 3.22)

project(JADE_DRAGON VERSION 2.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEFINED ENV{CI})
    set(COPY_AFTER_BUILD FALSE)
else()
    set(COPY_AFTER_BUILD TRUE)
endif()

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(JADE_DRAGON
    COMPANY_NAME "Hunterice Records"
    BUNDLE_ID "com.hunterice.jadedragon"
    PLUGIN_MANUFACTURER_CODE Hntr
    PLUGIN_CODE Jd21
    PRODUCT_NAME "JADE DRAGON"

    FORMATS VST3 Standalone

    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD ${COPY_AFTER_BUILD}
)

juce_generate_juce_header(JADE_DRAGON)

target_sources(JADE_DRAGON PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

target_compile_definitions(JADE_DRAGON PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_include_directories(JADE_DRAGON PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(JADE_DRAGON
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(JADE_DRAGON PRIVATE /W4 /utf-8)
    target_compile_definitions(JADE_DRAGON PRIVATE _USE_MATH_DEFINES)
else()
    target_compile_options(JADE_DRAGON PRIVATE -Wall -Wextra -Wpedantic)
endif()
