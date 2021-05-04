/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor (FlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    //we set the dimensions of the GUI window and the appearence of the GUI elements
    
    setSize (600, 200);
    
    gff_slider.setRange (0.0, 0.7, 0.01);
    gff_slider.setSliderStyle(juce::Slider::Rotary);
    gff_slider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    gff_slider.addListener(this);
    gff_label.setText ("Mix dry/wet", juce::dontSendNotification);
    
    addAndMakeVisible (gff_slider);
    addAndMakeVisible (gff_label);
    
    gfb_slider.setRange (0.0, 0.7, 0.01);
    gfb_slider.setSliderStyle(juce::Slider::Rotary);
    gfb_slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 100, 20);
    gfb_slider.addListener(this);
    gfb_label.setText ("Feedback", juce::dontSendNotification);
    
    addAndMakeVisible (gfb_slider);
    addAndMakeVisible (gfb_label);
    
    lfo_offset_slider.setRange (0.00, 5.00, 0.01);
    lfo_offset_slider.setSliderStyle(juce::Slider::Rotary);
    lfo_offset_slider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    lfo_offset_slider.addListener(this);
    lfo_offset_label.setText ("Delay", juce::dontSendNotification);
    
    addAndMakeVisible (lfo_offset_slider);
    addAndMakeVisible (lfo_offset_label);
    
    lfo_freq_slider.setRange(0.0, 10.0, 0.01);
    lfo_freq_slider.setSliderStyle(juce::Slider::Rotary);
    lfo_freq_slider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    lfo_freq_slider.addListener(this);
    lfo_freq_label.setText("LFO frequency",juce::dontSendNotification);
    
    addAndMakeVisible(lfo_freq_slider);
    addAndMakeVisible(lfo_freq_label);
    
    lfo_width_slider.setRange(0.00, 10.00, 0.01);
    lfo_width_slider.setSliderStyle(juce::Slider::Rotary);
    lfo_width_slider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    lfo_width_slider.addListener(this);
    lfo_width_label.setText("LFO sweep width", juce::dontSendNotification);
    
    addAndMakeVisible(lfo_width_slider);
    addAndMakeVisible(lfo_width_label);
    
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
}

//==============================================================================
void FlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("", getLocalBounds(), juce::Justification::centred, 1);
}

void FlangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    //we set the dimensions of all the GUI elements
    
    gff_label.setBounds(10, 10, 100, 20);
    gff_slider.setBounds(10, 40, 100, 100);
    
    gfb_label.setBounds(120, 10, 100, 20);
    gfb_slider.setBounds(120, 40, 100, 100);
    
    lfo_offset_label.setBounds(230, 10, 100, 20);
    lfo_offset_slider.setBounds(230, 40, 100, 100);
    
    lfo_freq_label.setBounds(340, 10, 100, 20);
    lfo_freq_slider.setBounds(340, 40, 100, 100);
    
    lfo_width_label.setBounds(450, 10, 100, 20);
    lfo_width_slider.setBounds(450, 40, 100, 100);
    
}

//we implement the method that will update the values of the main parameters

void FlangerAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &gff_slider)
        audioProcessor.setGff(gff_slider.getValue());
    else if (slider == &gfb_slider)
        audioProcessor.setGfb(gfb_slider.getValue());
    else if (slider == &lfo_offset_slider)
        audioProcessor.setLfoOffset(lfo_offset_slider.getValue());
    else if (slider == &lfo_freq_slider)
        audioProcessor.setLfoFreq(lfo_freq_slider.getValue());
    else if (slider == &lfo_width_slider)
        audioProcessor.setLfoWidth(lfo_width_slider.getValue());
}