/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

#ifndef M_PI
#define M_PI (3.14159265)
#endif

//==============================================================================
/**
*/
class FlangerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FlangerAudioProcessor();
    ~FlangerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    //we declare some methods that will change the values of the main parameters when called
    
    void setGff(float val);
    void setGfb(float val);
    void setLfoOffset(float val);
    void setLfoFreq(float val);
    void setLfoWidth(float val);

private:
    
    juce::AudioSampleBuffer delay_buffer; //buffer we use to create the delay effect
    
    int dw; //pointer we use to write into the delay_buffer
    float dr; //pointer we use to read from the delay_buffer
    
    double fs; //sampling frequency of the input
    
    float gff; //gain applied to the feedforward signal
    float gfb; //gain applied to the feedback signal
    float lfo_offset; //delay parameter of the LFO
    float lfo_freq; //frequency value of the LFO
    float lfo_width; //width value of the LFO
    float phase; //intermediate variable we use to calculare the value of the delay
    
    
    
    float LFO_value(float freq, float amp, float avg, double sample_rate); //calculates the value of the lfo function that governs the delay
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerAudioProcessor)
};
