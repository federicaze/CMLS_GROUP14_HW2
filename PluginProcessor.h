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
    
    void setGff(float val);
    void setGfb(float val);
    void setDelay(float val);
    void setLfoFreq(float val);
    void setLfoAmp(float val);

private:
    
    juce::AudioSampleBuffer delay_buffer; //ingresso + feedback
    juce::AudioSampleBuffer copia_ingresso; //qui copiamo i valori in ingresso
    float gff;
    float gfb;
    float avg_lfo_amp;
    float lfo_freq;
    float lfo_amp;
    float phase;
    double sample_rate;
    int delay_buffer_write; // Write pointer into the delay buffer
    float delay_buffer_read; //read pointer into the delay buffer
    
    
    float LFO_delay(float freq, float amp, float avg, double sample_rate);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerAudioProcessor)
};
