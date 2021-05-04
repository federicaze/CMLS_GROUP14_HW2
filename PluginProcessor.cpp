/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FlangerAudioProcessor::FlangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FlangerAudioProcessor::~FlangerAudioProcessor()
{
}

//==============================================================================
const juce::String FlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FlangerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void FlangerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //we initialize the size of the buffer
    
    delay_buffer.setSize(1, 10000);
    delay_buffer.clear();
    
    //we set the initial values of the parameters we will use in the processBlock method
    
    fs = sampleRate;
    
    gff = 0.0;
    gfb = 0.0;
    lfo_offset = 0.0;
    lfo_freq = 0.0;
    lfo_width = 0.0;
    phase = 0.0;
    
    dw = 0;
    dr = 0.0;
    
}

void FlangerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FlangerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 7; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    
    //we declare and initialize some local variables we need inside the for loop
    
    int n_input = buffer.getNumSamples();
    int n_delay = delay_buffer.getNumSamples();
    
    const float* channel_in = buffer.getReadPointer(0);
    float* channel_out_L = buffer.getWritePointer(0);
    float* channel_out_R = buffer.getWritePointer(1);
    
    float freq = lfo_freq;
    float width = lfo_width;
    float offset = lfo_offset;
    
    
    for (int i = 0; i < n_input; ++i)
    {
        
        //we gather the input sample
        float input = channel_in[i];      //R.I.P. Pino
        
        //we copy the input sample, as it is, in the copy_buffer
        delay_buffer.setSample(0, dw, input);
        
        //we calculate the delay given by the LFO, first in milliseconds, then in samples
        float current_delay = LFO_value(freq, width, offset, fs);
        dr = fmodl(dw - (current_delay * fs) + n_delay - 1, n_delay);
        
        //we interpolate the sample reading from the copy_buffer at the indexes corresponding to the given delay
        float fraction = dr - floorf(dr);
        int previous_sample = (int)floorf(dr) % n_delay;
        int next_sample = (previous_sample + 1) % n_delay;
        float interpolated_sample = fraction * delay_buffer.getSample(0, next_sample) + (1.0 - fraction) * delay_buffer.getSample(0, previous_sample);
        
        //first, we use the interpolated_sample to add the sample back into the delay_buffer in order to create feedback
        float sample_feedback = gfb * interpolated_sample;
        delay_buffer.addSample(0, dw, sample_feedback);
        
        //secondly, we use the interpolated_sample to create the feedforward effect
        float sample_feedforward = gff * interpolated_sample;
        
        //we send the samples to the output bus
        
        channel_out_L[i] = input + sample_feedforward;
        channel_out_R[i] = input + sample_feedforward;
        
        //we wrap the write pointer around the length of the buffer in order to create circularity
        dw = (dw + 1) % n_delay;
        
    }
}

//==============================================================================
bool FlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FlangerAudioProcessor::createEditor()
{
    return new FlangerAudioProcessorEditor (*this);
}

//==============================================================================
void FlangerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}

//we implement the methods we need to controll the parameters through the GUI

void FlangerAudioProcessor::setGff(float val)
{
    gff = val;
}

void FlangerAudioProcessor::setGfb(float val)
{
    gfb = val;
}

void FlangerAudioProcessor::setLfoOffset(float val)
{
    lfo_offset = val;
}

void FlangerAudioProcessor::setLfoFreq(float val)
{
    lfo_freq = val;
}

void FlangerAudioProcessor::setLfoWidth(float val)
{
    lfo_width = val;
}

//we implement the method that calculates the value of the delay in milliseconds

float FlangerAudioProcessor::LFO_value(float freq, float amp, float offset, double fs)
{ 
    phase += (float) ( M_PI * 2.0 *((double) freq / (double) fs));
    
    if(phase > M_PI * 2.0)
    {
        phase -= M_PI * 2.0;
    }
    
    return (offset + amp * (1 + (float) sin ((double) phase)))/1000;
}