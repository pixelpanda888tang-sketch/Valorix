#pragma once

#define JucePlugin_Name "ZyronixT"
#define JucePlugin_Desc "AI-assisted wavetable synth"
#define JucePlugin_Manufacturer "BeatsPanda"
#define JucePlugin_ManufacturerCode 0x42745064
#define JucePlugin_PluginCode 0x5a795254
#define JucePlugin_IsSynth 1
#define JucePlugin_WantsMidiInput 1
#define JucePlugin_ProducesMidiOutput 0
#define JucePlugin_IsMidiEffect 0
#define JucePlugin_EditorRequiresKeyboardFocus 0

#define JucePlugin_Version 0x010000
#define JucePlugin_VersionString "1.0.0"
#define JucePlugin_VersionCode 0x010000

#define JucePlugin_MaxNumInputChannels 0
#define JucePlugin_MaxNumOutputChannels 2
#define JucePlugin_PreferredChannelConfigurations {0, 2}

#define JucePlugin_AUMainType 'aumu'
#define JucePlugin_AUSubType JucePlugin_PluginCode
#define JucePlugin_AUExportPrefix ZyronixT
#define JucePlugin_AUManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_VSTUniqueID JucePlugin_PluginCode
#define JucePlugin_VSTCategory kPlugCategSynth
#define JucePlugin_Vst3Category "Instrument|Synth"

#ifndef JucePlugin_Build_VST
 #define JucePlugin_Build_VST 0
#endif
#ifndef JucePlugin_Build_VST3
 #define JucePlugin_Build_VST3 1
#endif
#ifndef JucePlugin_Build_AU
 #define JucePlugin_Build_AU 0
#endif
#ifndef JucePlugin_Build_Standalone
 #define JucePlugin_Build_Standalone 1
#endif

