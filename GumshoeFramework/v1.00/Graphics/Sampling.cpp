//-------------------------------------------------------------------------------
//
// Gumshoe Framework v1.00
//   - Based on MJP's DX11 Sample Framework (http://mynameismjp.wordpress.com/)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#include "PCH.h"
#include "Sampling.h"

namespace GumshoeFramework10
{

#define RadicalInverse_(base) \
    { \
        const float radical = 1.0f / float(base); \
        uint64 value = 0; \
        float factor = 1.0f; \
        while(sampleIdx) { \
            uint64 next  = sampleIdx / base; \
            uint64 digit = sampleIdx - next * base; \
            value = value * base + digit; \
            factor *= radical; \
            sampleIdx = next; \
        } \
        inverse = float(value) * factor; \
    }

float RadicalInverseFast(uint64 baseIdx, uint64 sampleIdx)
{
    Assert_(baseIdx < 64);

    float inverse = 0.0f;

    switch (baseIdx)
    {
        case 0: RadicalInverse_(2); break;
        case 1: RadicalInverse_(3); break;
        case 2: RadicalInverse_(5); break;
        case 3: RadicalInverse_(7); break;
        case 4: RadicalInverse_(11); break;
        case 5: RadicalInverse_(13); break;
        case 6: RadicalInverse_(17); break;
        case 7: RadicalInverse_(19); break;
        case 8: RadicalInverse_(23); break;
        case 9: RadicalInverse_(29); break;
        case 10: RadicalInverse_(31); break;
        case 11: RadicalInverse_(37); break;
        case 12: RadicalInverse_(41); break;
        case 13: RadicalInverse_(43); break;
        case 14: RadicalInverse_(47); break;
        case 15: RadicalInverse_(53); break;
        case 16: RadicalInverse_(59); break;
        case 17: RadicalInverse_(61); break;
        case 18: RadicalInverse_(67); break;
        case 19: RadicalInverse_(71); break;
        case 20: RadicalInverse_(73); break;
        case 21: RadicalInverse_(79); break;
        case 22: RadicalInverse_(83); break;
        case 23: RadicalInverse_(89); break;
        case 24: RadicalInverse_(97); break;
        case 25: RadicalInverse_(101); break;
        case 26: RadicalInverse_(103); break;
        case 27: RadicalInverse_(107); break;
        case 28: RadicalInverse_(109); break;
        case 29: RadicalInverse_(113); break;
        case 30: RadicalInverse_(127); break;
        case 31: RadicalInverse_(131); break;
        case 32: RadicalInverse_(137); break;
        case 33: RadicalInverse_(139); break;
        case 34: RadicalInverse_(149); break;
        case 35: RadicalInverse_(151); break;
        case 36: RadicalInverse_(157); break;
        case 37: RadicalInverse_(163); break;
        case 38: RadicalInverse_(167); break;
        case 39: RadicalInverse_(173); break;
        case 40: RadicalInverse_(179); break;
        case 41: RadicalInverse_(181); break;
        case 42: RadicalInverse_(191); break;
        case 43: RadicalInverse_(193); break;
        case 44: RadicalInverse_(197); break;
        case 45: RadicalInverse_(199); break;
        case 46: RadicalInverse_(211); break;
        case 47: RadicalInverse_(223); break;
        case 48: RadicalInverse_(227); break;
        case 49: RadicalInverse_(229); break;
        case 50: RadicalInverse_(233); break;
        case 51: RadicalInverse_(239); break;
        case 52: RadicalInverse_(241); break;
        case 53: RadicalInverse_(251); break;
        case 54: RadicalInverse_(257); break;
        case 55: RadicalInverse_(263); break;
        case 56: RadicalInverse_(269); break;
        case 57: RadicalInverse_(271); break;
        case 58: RadicalInverse_(277); break;
        case 59: RadicalInverse_(281); break;
        case 60: RadicalInverse_(283); break;
        case 61: RadicalInverse_(293); break;
        case 62: RadicalInverse_(307); break;
        case 63: RadicalInverse_(311); break;
    }
    return std::min(inverse, OneMinusEpsilon);
}

}