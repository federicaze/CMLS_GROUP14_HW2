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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    delay_buffer.setSize(getTotalNumOutputChannels(), 10000); //?????
    delay_buffer.clear();
    copia_ingresso.setSize(getTotalNumOutputChannels(), 10000);
    copia_ingresso.clear();
    sample_rate = sampleRate;
    
    delay_buffer_write = 0;
    
    phase = 0.0;
    gff = 0.0;
    gfb = 0.0;
    avg_lfo_amp = 1.0;
    lfo_freq = 0.0;
    lfo_amp = 0.0;
    
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
    
    
    int num_samples_in = buffer.getNumSamples();    
    int num_samples_db = delay_buffer.getNumSamples();
    
    float* channel_out_L = buffer.getWritePointer(0);
    float* channel_out_R = buffer.getWritePointer(1);
    const float* channel_in = buffer.getReadPointer(0);
    
    float freq = lfo_freq;
    float amp = lfo_amp;
    float avg_amp = avg_lfo_amp;
    float sample_feedforward = 0.0;
    
    for (int i = 0; i < num_samples_in; ++i)
    {
        const float in = channel_in[i];
        float pino = in;
        
        copia_ingresso.setSample(0, delay_buffer_write, in); 
        
        //if (sample_feedforward)
            //float in = in + sample_feedforward;
        
        float current_delay = LFO_delay(freq, amp, avg_amp, sample_rate);
        
        
        delay_buffer_read = fmodf((float)delay_buffer_write - (float)(current_delay * sample_rate)+ (float)num_samples_db - 3.0, (float)num_samples_db);
        
        float fraction = delay_buffer_read - floorf(delay_buffer_read);
        int previous_sample = (int)floorf(delay_buffer_read) % num_samples_db;
        int next_sample = (previous_sample + 1) % num_samples_db;
        float interpolated_sample = fraction * copia_ingresso.getSample(0, next_sample) + (1.0 - fraction) * copia_ingresso.getSample(0, previous_sample);
        
        
        float sample_feedback = interpolated_sample * gfb;
        
        delay_buffer.setSample(0, delay_buffer_write, sample_feedback);
        
        
        sample_feedforward = gff * (copia_ingresso.getSample(0, delay_buffer_write) + delay_buffer.getSample(0, delay_buffer_write));
        
        
        delay_buffer_write = (delay_buffer_write + 1) % num_samples_db;
    
        channel_out_L[i] = pino + sample_feedforward;
        channel_out_R[i] = pino + sample_feedforward;
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

void FlangerAudioProcessor::setGff(float val)
{
    gff = val;
}

void FlangerAudioProcessor::setGfb(float val)
{
    gfb = val;
}

void FlangerAudioProcessor::setDelay(float val)
{
    avg_lfo_amp = val;
}

void FlangerAudioProcessor::setLfoFreq(float val)
{
    lfo_freq = val;
}

void FlangerAudioProcessor::setLfoAmp(float val)
{
    lfo_amp = val;
}

float FlangerAudioProcessor::LFO_delay(float freq, float amp, float avg, double sample_rate)
{ 
    phase += (float) ( M_PI * 2.0 *((double) freq / (double) sample_rate));
    
    if(phase > M_PI * 2.0)
    {
        phase -= M_PI * 2.0;
    }
    
    return (avg + amp * (float) sin ((double) phase))/1000;
}