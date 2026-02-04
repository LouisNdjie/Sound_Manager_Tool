#include"wav_file.h"

namespace SoundManager
{
    WavFile::WavFile()
    {
        mHeader = 
        {
            .fileTypeBlocId = {0x52, 0x49, 0x46, 0x46}, // "RIFF"
            .fileSize = 0,
            .fileFormatId = {0x57, 0x41, 0x56, 0x45}   // "WAVE"
        };

        // Configuration pour STÉRÉO (corrigé)
        mAudioFormat = 
        {
            .formatBlocId = {0x66, 0x6D, 0x74, 0x20}, // "fmt "
            .blocSize = 16,
            .audioFormat = 1,         // PCM
            .nbrChannels = 2,         // STÉRÉO 
            .sampleRate = 44100,
            .bytePerSec = 44100 * 2 * 2,  // sampleRate * nbrChannels * (bitsPerSample/8) = 176400
            .bytePerBloc = 4,             // nbrChannels * (bitsPerSample/8) = 2 * 2 = 4
            .bitsPerSample = 16
        };

        mBlocData = 
        {
            .dataBlocId = {0x64, 0x61, 0x74, 0x61}, // "data"
            .dataSize = 0
        };

        mMaxAmplitude = Maths::Power(2, 15) - 1;  // 32767 pour 16 bits
    }

    void WavFile::SetDuration(int duration)
    {
        this->mDuration = duration;
    }

    void WavFile::SetFilename(const std::string& filename)
    {
        this->mFilename = filename;
    }
    
    void WavFile::SetFrequency(float frequency)
    {
        this->mFrequency = frequency;
    }

    void WavFile::SetAmplitude(float amplitude)
    {
        this->mAmplitude = amplitude;
    }

    void WavFile::WriteAsByte(int value, int size)
    {
        this->mWavFile.write(reinterpret_cast<const char*>(&value), size);
    }

    void WavFile::WriteHeader()
    {
        if (this->mWavFile.is_open())
        {
            // Header
            this->mWavFile << this->mHeader.fileTypeBlocId;
            this->WriteAsByte(this->mHeader.fileSize, 4);
            this->mWavFile << this->mHeader.fileFormatId;

            // Header format
            this->mWavFile << this->mAudioFormat.formatBlocId;
            this->WriteAsByte(this->mAudioFormat.blocSize, 4);
            this->WriteAsByte(this->mAudioFormat.audioFormat, 2);
            this->WriteAsByte(this->mAudioFormat.nbrChannels, 2);
            this->WriteAsByte(this->mAudioFormat.sampleRate, 4);
            this->WriteAsByte(this->mAudioFormat.bytePerSec, 4);
            this->WriteAsByte(this->mAudioFormat.bytePerBloc, 2);
            this->WriteAsByte(this->mAudioFormat.bitsPerSample, 2);

            // Data
            this->mWavFile << this->mBlocData.dataBlocId;
            this->WriteAsByte(this->mBlocData.dataSize, 4);
        }
    }

    void WavFile::WriteInAudioFile(const int& choiceFonction, const std::vector<float>& recordedSamples)
    {
        this->mWavFile.open(this->mFilename, std::ios::binary);
        
        this->mOffset = 2 * PI * mFrequency / 44100;
        std::cout << mOffset << std::endl;
        
        if (this->mWavFile.is_open())
        {
            bool step = false;
            float sample = 0.0f;
            this->WriteHeader();

            int positionBeforeAddingAudioData = this->mWavFile.tellp();

            switch (choiceFonction)
            {
                case 1: // Sine Oscillator
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::SineOscillator(this->mAmplitude, this->mOffset, this->mAngle);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4); 
                    }
                    break;
                
                case 2: // Cosine Oscillator
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::CosineOscillator(this->mAmplitude, this->mOffset, this->mAngle);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4); 
                    }
                    break;

                case 3: // Square Wave
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::SquareWave(this->mAmplitude, this->mOffset, this->mAngle);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4);
                    }
                    break;

                case 4: // Triangular Wave
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::TriangularWave(this->mAmplitude, this->mOffset, this->mAngle);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4); 
                    }
                    break;

                case 5: // Sawtooth
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::Sawtooth(this->mOffset, this->mAngle);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4); 
                    }
                    break;

                case 6: // White Noise
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::WhiteNoise(this->mAmplitude);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4); 
                    }
                    break;
                    
                case 7: // Pink Noise
                    for (int i = 0; i < this->mDuration * this->mAudioFormat.sampleRate; i++)
                    {
                        sample = Maths::PinkNoise(this->mAmplitude);
                        int sampleInt = static_cast<int>(sample * this->mMaxAmplitude);
                        this->WriteAsByte(sampleInt, 4); 
                    }
                    break;
                    
                case 8: // From Microphone
                    step = true;
                    std::cout << "\nÉcriture depuis le microphone..." << std::endl;
                    std::cout << "Nombre d'échantillons à écrire : " << recordedSamples.size() << std::endl;
                    
                    // Les échantillons sont déjà en stéréo (L, R, L, R, ...)
                    for (size_t i = 0; i < recordedSamples.size(); i++)
                    {
                        // Convertir de float (-1.0 à 1.0) vers int16 (-32768 à 32767)
                        int sampleInt = static_cast<int>(recordedSamples[i] * this->mMaxAmplitude);
                        
                        // Clamper pour éviter le clipping
                        if (sampleInt > this->mMaxAmplitude) sampleInt = this->mMaxAmplitude;
                        if (sampleInt < -this->mMaxAmplitude) sampleInt = -this->mMaxAmplitude;

                        this->WriteAsByte(sampleInt, 2); 
                    }
                    
                    std::cout << "Écriture terminée!" << std::endl;
                    break;

                default:
                    std::cerr << "Fonction inconnue: " << choiceFonction << std::endl;
                    break;
            }

            int positionAfterAddingAudioData = this->mWavFile.tellp();

            // Mettre à jour la taille des données
            this->mWavFile.seekp(positionBeforeAddingAudioData - 4);
            this->WriteAsByte(positionAfterAddingAudioData - positionBeforeAddingAudioData, 4);
            
            // Mettre à jour la taille du fichier
            this->mWavFile.seekp(4, std::ios::beg);
            this->WriteAsByte(positionAfterAddingAudioData - 8, 4);
            
            std::cout << "\n✅ Fichier WAV sauvegardé: test.wav" << std::endl;
            std::cout << "   Taille: " << (positionAfterAddingAudioData - 8) / 1024.0 / 1024.0 << " MB" << std::endl;
            if (step)
            {
                std::cout << "   Échantillons: " << recordedSamples.size() << std::endl;
                std::cout << "   Durée: " << (recordedSamples.size() / 2.0 / this->mAudioFormat.sampleRate) << " secondes" << std::endl;
            }
        }
        else
        {
            std::cerr << "Erreur: impossible d'ouvrir test.wav" << std::endl;
        }
    }
    
    void WavFile::SaveWavFile(const int& choiceFonction, const std::vector<float>& recordedSamples)
    {
        this->WriteInAudioFile(choiceFonction, recordedSamples);
    }

    WavFile::~WavFile()
    {
        this->mWavFile.close();
    }
}