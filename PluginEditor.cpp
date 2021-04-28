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
    setSize (600, 200);
    
    gff_slider.setRange (0.0, 0.7, 0.01);
    gff_slider.setSliderStyle(juce::Slider::Rotary);
    gff_slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 100, 20);
    gff_slider.addListener(this);
    gff_label.setText ("Feedforward", juce::dontSendNotification);
    
    addAndMakeVisible (gff_slider);
    addAndMakeVisible (gff_label);
    
    gfb_slider.setRange (0.0, 0.7, 0.01);
    gfb_slider.setSliderStyle(juce::Slider::Rotary);
    gfb_slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 100, 20);
    gfb_slider.addListener(this);
    gfb_label.setText ("Feedback", juce::dontSendNotification);
    
    addAndMakeVisible (gfb_slider);
    addAndMakeVisible (gfb_label);
    
    delay_slider.setRange (1, 2, 0.1);
    delay_slider.setSliderStyle(juce::Slider::Rotary);
    delay_slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 100, 20);
    delay_slider.addListener(this);
    delay_label.setText ("Average delay", juce::dontSendNotification);
    
    addAndMakeVisible (delay_slider);
    addAndMakeVisible (delay_label);
    
    lfo_freq_slider.setRange(0.0, 10.0, 0.1);
    lfo_freq_slider.setSliderStyle(juce::Slider::Rotary);
    lfo_freq_slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 100, 20);
    lfo_freq_slider.addListener(this);
    lfo_freq_label.setText("LFO frequency",juce::dontSendNotification);
    
    addAndMakeVisible(lfo_freq_slider);
    addAndMakeVisible(lfo_freq_label);
    
    lfo_amp_slider.setRange(0.00, 1, 0.01);
    lfo_amp_slider.setSliderStyle(juce::Slider::Rotary);
    lfo_amp_slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 100, 20);
    lfo_amp_slider.addListener(this);
    lfo_amp_label.setText("LFO amplitude", juce::dontSendNotification);
    
    addAndMakeVisible(lfo_amp_slider);
    addAndMakeVisible(lfo_amp_label);
    
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
    
    gff_label.setBounds(10, 10, 100, 20);
    gff_slider.setBounds(10, 40, 100, 100);
    
    gfb_label.setBounds(120, 10, 100, 20);
    gfb_slider.setBounds(120, 40, 100, 100);
    
    delay_label.setBounds(230, 10, 100, 20);
    delay_slider.setBounds(230, 40, 100, 100);
    
    lfo_freq_label.setBounds(340, 10, 100, 20);
    lfo_freq_slider.setBounds(340, 40, 100, 100);
    
    lfo_amp_label.setBounds(450, 10, 100, 20);
    lfo_amp_slider.setBounds(450, 40, 100, 100);
    
}

void FlangerAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &gff_slider)
        audioProcessor.setGff(gff_slider.getValue());
    else if (slider == &gfb_slider)
        audioProcessor.setGfb(gfb_slider.getValue());
    else if (slider == &delay_slider)
        audioProcessor.setDelay(delay_slider.getValue());
    else if (slider == &lfo_freq_slider)
        audioProcessor.setLfoFreq(lfo_freq_slider.getValue());
    else if (slider == &lfo_amp_slider)
        audioProcessor.setLfoAmp(lfo_amp_slider.getValue());
}