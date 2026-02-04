#include"micro.h"   

namespace SoundManager
{
    int Microphone::callbackCount = 0;
    
    Microphone::Microphone()
    {
        /**
        * @brief Constructeur de la classe Microphone. Initialise PortAudio.
        */
       
        this->mError = Pa_Initialize();
        this->mNumberOfDevices = Pa_GetDeviceCount();
        this->CheckPaError();
        this->CheckDeviceError();
        this->mDevice = 0;  // Device 
        
        std::cout << "Utilisation du device " << this->mDevice << ": " 
                  << Pa_GetDeviceInfo(this->mDevice)->name << std::endl;
        
        /*
        * Configuration des paramètres d'entrée audio.
        */
        memset(&this->mInputParameters, 0, sizeof(this->mInputParameters));
        this->mInputParameters.channelCount = 2;  // STÉRÉO (corrigé de 1 à 2)
        this->mInputParameters.device = this->mDevice;
        this->mInputParameters.hostApiSpecificStreamInfo = NULL;
        this->mInputParameters.sampleFormat = paFloat32;
        this->mInputParameters.suggestedLatency = Pa_GetDeviceInfo(this->mDevice)->defaultLowInputLatency;
    }

    void Microphone::CheckPaError()
    {
        if (this->mError != paNoError)
        {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(this->mError) << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void Microphone::CheckDeviceError()
    {
        if (this->mNumberOfDevices < 0)
        {
            std::cerr << "ERROR: Pa_GetDeviceCount returned " << this->mNumberOfDevices << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void Microphone::PrintAvailableAudioDevices()
    {
        std::cout << "Number of audio devices: " << this->mNumberOfDevices << std::endl;
    }

    void Microphone::ListAudioDevicesInformation()
    {
        for (int i = 0; i < this->mNumberOfDevices; i++)
        {
            this->mDeviceInfo = Pa_GetDeviceInfo(i);
            std::cout << "Device " << i << ": " << std::endl;
            std::cout << " Name : " << this->mDeviceInfo->name << std::endl;
            std::cout << " Max Input Channels : " << this->mDeviceInfo->maxInputChannels << std::endl;
            std::cout << " Max Output Channels : " << this->mDeviceInfo->maxOutputChannels << std::endl;
            std::cout << " Default Sample Rate : " << this->mDeviceInfo->defaultSampleRate << std::endl;
            std::cout << "---------------------------------------" << std::endl;
        }
    }
    
    int Microphone::CallBack(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, 
                             const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
    {
        callbackCount++;
        
        float* input = (float*)inputBuffer;
        (void)outputBuffer;

        paTestData* data = (paTestData*)userData;

        // Enregistrer les échantillons
        if (input != NULL && data->recordedSamples != NULL)
        {
            // Capturer tous les échantillons (2 canaux stéréo)
            for (unsigned long i = 0; i < framesPerBuffer * 2; i++)
            {
                data->recordedSamples->push_back(input[i]);
            }
            data->frameIndex += framesPerBuffer;  // Important pour le compteur
        }

        // Affichage visuel (toutes les 50 fois pour ne pas ralentir)
        if (callbackCount % 50 == 0)
        {
            int dispSize = 50;
            printf("\r");

            float vol_l = 0;
            float vol_r = 0;

            if (input != NULL)
            {
                for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2)
                {
                    vol_l = Maths::Max(vol_l, std::abs(input[i]));
                    vol_r = Maths::Max(vol_r, std::abs(input[i + 1]));
                }

                printf("L:");
                for (int i = 0; i < dispSize; i++)
                {
                    float barProportion = i / (float)dispSize;
                    if (barProportion <= vol_l)
                        printf("█");
                    else
                        printf("·");
                }

                printf(" R:");
                for (int i = 0; i < dispSize; i++)
                {
                    float barProportion = i / (float)dispSize;
                    if (barProportion <= vol_r)
                        printf("█");
                    else
                        printf("·");
                }

                printf(" [%d samples]", (int)data->recordedSamples->size());
            }

            fflush(stdout);
        }

        // Continuer tant qu'on n'a pas atteint la durée maximale
        if (data->frameIndex < data->maxFrames)
            return paContinue;
        else
            return paComplete;
    }

    void Microphone::InitializePortAudio(int duration)
    {
        /**
        * @brief Initialise PortAudio et enregistre.
        */

        this->myRecording.clear();
        this->myRecording.reserve(duration * SAMPLE_RATE * 2);  // Réserver de l'espace
        
        // Initialiser la structure de données
        data.recordedSamples = &this->myRecording;
        data.maxFrames = duration * SAMPLE_RATE;
        data.frameIndex = 0;

        std::cout << "\nOuverture du stream..." << std::endl;
        this->mError = Pa_OpenStream(
            &this->mStream, 
            &this->mInputParameters, 
            NULL,  // Pas de sortie
            SAMPLE_RATE, 
            FRAMES_PER_BUFFER, 
            paNoFlag, 
            this->CallBack, 
            &data
        );
        this->CheckPaError();

        std::cout << "Démarrage du stream..." << std::endl;
        this->mError = Pa_StartStream(this->mStream);
        this->CheckPaError();

        std::cout << "🔴 Enregistrement pendant " << duration << " secondes... Parlez dans le micro!" << std::endl;

        // Attendre la fin de l'enregistrement
        while (Pa_IsStreamActive(this->mStream))
        {
            Pa_Sleep(100);
        }
        
        std::cout << "\n\n✅ Enregistrement terminé!" << std::endl;
        std::cout << "Total d'échantillons capturés : " << myRecording.size() << std::endl;
        std::cout << "Durée réelle : " << (myRecording.size() / 2.0f / SAMPLE_RATE) << " secondes" << std::endl;
    }
    
    void Microphone::ClosePortAudio()
    {
        /**
        * @brief Ferme PortAudio.
        */
        
        this->mError = Pa_StopStream(this->mStream);
        this->CheckPaError();

        this->mError = Pa_CloseStream(this->mStream);
        this->CheckPaError();
    }

    void Microphone::CaptureAudio(int duration)
    {
        /**
        * @brief Capture l'audio pendant une durée spécifiée.
        * @param duration durée en secondes pendant laquelle l'audio est capturé.
        */

        this->InitializePortAudio(duration);
    }
    
    const std::vector<float> Microphone::GetRecordedSamples() const 
    {
        return this->myRecording;
    }

    Microphone::~Microphone()
    {
        /**
        * @brief Destructeur de la classe Microphone. Termine PortAudio.
        */

        this->ClosePortAudio();
        this->mError = Pa_Terminate();
        this->CheckPaError();
    }
}