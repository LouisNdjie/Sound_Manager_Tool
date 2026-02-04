#pragma once

#include<iostream>
#include"../Maths/math.h"
#include<fstream>



namespace SoundManager
{
    class WavFile
    {
    private:
        struct WavHeader
        {
            char fileTypeBlocId[5]; //défini le type de fichier par la constante "RIFF"
            int fileSize ;           //taille totale du fichier
            char fileFormatId[5];   //défini le format du fichier par la constante "WAVE"
        };

        struct WavAudioFormat
        {
            char formatBlocId[5]; //défini le type de bloc par la constante "fmt "
            int blocSize;        //taille du bloc
            int audioFormat : 16; //format audio (1 = PCM)
            int nbrChannels : 16; //nombre de canaux (1 = mono, 2 = stéréo)
            int sampleRate;      //fréquence d'échantillonnage(Hz)[valeur standard : 44100 Hz]
            int bytePerSec;     //nombre d'octets par seconde
            int bytePerBloc : 16; //nombre d'octets par bloc
            int bitsPerSample : 16; //nombre de bits par échantillon (8, 16, 24, 32)
        };
        
        struct WavBlocData
        {
            char dataBlocId[5]; //défini le type de bloc par la constante "data"
            int dataSize;       //taille des données audio
        };

        
        WavHeader mHeader;
        WavAudioFormat mAudioFormat;
        WavBlocData mBlocData;

        std::ofstream mWavFile; 

        float mMaxAmplitude;

        int mDuration = 5; //durée en secondes
        std::string mFilename;
        float mFrequency;
        float mAmplitude;
        float mAngle;
        float mOffset;

    private:
        void WriteHeader();                            //pour ecrire l'entête du fichier 
        void WriteAsByte(int, int);         //pour ecrire les informations sous formes de byte dans le fichier
        void WriteInAudioFile(const int&, const std::vector<float>&);                       //pour ecrire dans le fichier de manière concrète
        
    public:
        WavFile();
        void SetDuration(int);
        void SetFilename(const std::string&);
        void SetFrequency(float);
        void SetAmplitude(float);
        void SaveWavFile(const int&, const std::vector<float>&);
        ~WavFile();
    };
    
};

