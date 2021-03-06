/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FlangerAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    FlangerAudioProcessorEditor (FlangerAudioProcessor&);
    ~FlangerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FlangerAudioProcessor& audioProcessor;
    
    //we declare the sliders and the labels
    
    juce::Slider gff_slider; //gain feedforward
    juce::Label gff_label;
    
    juce::Slider gfb_slider; //gain feedback
    juce::Label gfb_label;
    
    juce::Slider lfo_offset_slider; //LFO delay parameter
    juce::Label lfo_offset_label;
    
    juce::Slider lfo_freq_slider; //LFO frequency
    juce::Label lfo_freq_label;
    
    juce::Slider lfo_width_slider; //LFO sweep width
    juce::Label lfo_width_label;
    
    
    //callback function that changes the values of a parameter everytime the corrisponding slider is touched
    
    void sliderValueChanged(juce::Slider* slider) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerAudioProcessorEditor)
};
