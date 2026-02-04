#pragma once

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<vector>
#include"../Maths/math.h"
#include<portaudio.h>

namespace SoundManager
{
    #define SAMPLE_RATE 44100
    #define FRAMES_PER_BUFFER 512

    class Microphone
    {
        private:
            typedef struct 
            {
                std::vector<float>* recordedSamples;
                int maxFrames;
                int frameIndex;
            } paTestData;

            std::vector<float> myRecording;
            paTestData data;
            static int callbackCount; 
            int mNumberOfDevices;
            int mDevice; 

            const PaDeviceInfo* mDeviceInfo; 
            PaStreamParameters mInputParameters;
            PaStreamParameters mOutputParameters;
            PaStream* mStream;
            PaError mError;
            

            void CheckPaError();
            void CheckDeviceError();
            void PrintAvailableAudioDevices();
           
            static int CallBack(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *);
            void InitializePortAudio(int);
            void ClosePortAudio();

            public:
                Microphone();
                void CaptureAudio(int duration);
                 void ListAudioDevicesInformation();
                const std::vector<float> GetRecordedSamples() const ;
                ~Microphone();
    };
};