#include"math.h"

namespace SoundManager
{
    namespace Maths
    {

       float Power(const float& x, const float& y)
       
       {
            /**
            * @brief Calcule la puissance de deux nombres.
            * @param x l'exposant.
            * @param y nombre dont on veut la puissance.
            * @return le nombre y exposant le nombre x.
            */

        	float exp = 1.0f;
	        for(int i=1; i<=x; i++)
            {
		        exp = exp * y;
	        }
	        return exp;
       }

       float Factorial(const float& x)
       {
            /**
            * @brief Calcule le factoriel d'un nombre.
            * @param x nombre dont on veut le factoriel.
            * @return le factoriel de x.
            */

            if(x < 0)return 0;

            float fact = 1.0f;
	        for(int i=1; i<=x; i++)
            {
		        fact = fact * i;
	        }
	        return fact;
       }

        float Sign(const float& x)
        {
            /**
            * @brief Détermine le signe d'un nombre.
            * @param x nombre dont on veut le signe.
            * @return 1 si positif ou égale à 0, -1 si négatif.
            */

            if (x >= 0) return 1.0f;
            return -1.0f;
        }

       
        float SineOscillator(const float& amp, const float& offset, float& angle)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction sinus.
            * @param amp amplitude.
            * @param offset période.
            * @param angle angle courant.
            * @return les valuers audios.
            */

            float sample = amp * sin(angle);
            angle += offset;
            return sample;
        }

        float CosineOscillator(const float& amp, const float& offset, float& angle)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction cosinus.
            * @param amp amplitude.
            * @param offset période.
            * @param angle angle courant.
            * @return les valuers audios.
            */

            float sample = amp * cos(angle);
            angle += offset;
            return sample;
        }
        float SquareWave(const float& amp, const float& offset, float& angle)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction onde carrée.
            * @param amp amplitude.
            * @param offset période.
            * @param angle angle courant.
            * @return les valuers audios.
            */

            float sample = amp * Sign(sin(angle));
            angle += offset;
            return sample;
        }

        float TriangularWave(const float& amp, const float& offset, float& angle)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction onde triangulaire.
            * @param amp amplitude.
            * @param offset période.
            * @param angle angle courant.
            * @return les valuers audios.
            */

            float sample = (2 * amp / PI) * asin(sin(angle));
            angle += offset;
            return sample;
        }

        float Sawtooth(const float& offset, float& angle)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction onde en dents de scie.
            * @param offset période.
            * @param angle angle courant.
            * @return les valuers audios.
            */

            
            double phase = angle / (2 * PI);
            float sample = 2.0f * (phase - floor(phase + 0.5f));
            angle += offset;
            return sample;
        }

        float WhiteNoise(const float& amp)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction bruit blanc.
            * @param amp amplitude.
            * @return les valuers audios.
            */

            static std::mt19937 generator;
            static std::uniform_real_distribution<float> distribution(-1.0f * amp, 1.0f * amp);
            float sample = distribution(generator);
            return sample;
        }

        float PinkNoise(const float& amp)
        {
            /**
            * @brief permet de générer une plage de valeur en se servant de la fonction bruit rose.
            * @param amp amplitude.
            * @return les valuers audios.
            */

            float pinkRows[7] = {0.0f};    //initialisation des rangées de bruit rose (7 octaves)
            float total = 0.0f;
            static std::mt19937 generator;
            static std::uniform_real_distribution<float> distribution(-1.0f * amp, 1.0f * amp);

            float sample = 0.0f;
            int i = 0;

            //mise à jour des rangées de bruit rose de manière aléatoire
            while ((rand() % 2) == 0 && i < 6) i++;

            total -= pinkRows[i];
            pinkRows[i] = distribution(generator);
            total += pinkRows[i];

            sample = total + distribution(generator);     //ajout d'un peu de bruit blanc pour plus de réalisme
            return sample;
        }
    };
};