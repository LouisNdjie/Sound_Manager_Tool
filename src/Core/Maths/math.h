#pragma once

#include<math.h>
#include<random>

namespace SoundManager
{
    namespace Maths
    {
        #define PI 3.14159265358979323846

        float Power(const float&, const float&);
        float Factorial(const float&);
        float Sign(const float&);
        inline float Max(const float& a, const float& b) { return a > b ? a : b; }

        float SineOscillator(const float&, const float&, float &);
        float CosineOscillator(const float&, const float&, float&);
        float SquareWave(const float&, const float&, float&);
        float TriangularWave(const float&, const float&, float&);
        float Sawtooth(const float&, float&);
        float WhiteNoise(const float&);
        float PinkNoise(const float&);
    };
};