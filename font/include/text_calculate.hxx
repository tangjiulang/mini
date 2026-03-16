#pragma once
#include <trigo.hxx>
#include <util.hxx> // for KiROUND
#include <font.hxx>

int GetPenSizeForBold(int aTextSize)
{
    return KiROUND(aTextSize / 5.0);
}


int GetPenSizeForDemiBold(int aTextSize)
{
    return KiROUND(aTextSize / 6.0);
}


int GetPenSizeForBold(const VECTOR2I& aTextSize)
{
    return GetPenSizeForBold(std::min(aTextSize.x, aTextSize.y));
}


int GetPenSizeForDemiBold(const VECTOR2I& aTextSize)
{
    return GetPenSizeForDemiBold(std::min(aTextSize.x, aTextSize.y));
}


int GetPenSizeForNormal(int aTextSize)
{
    return KiROUND(aTextSize / 8.0);
}


int GetPenSizeForNormal(const VECTOR2I& aTextSize)
{
    return GetPenSizeForNormal(std::min(aTextSize.x, aTextSize.y));
}


int ClampTextPenSize(int aPenSize, int aSize, bool aStrict = false)
{
    double scale = aStrict ? 0.18 : 0.25;
    int    maxWidth = KiROUND((double) aSize * scale);

    return std::min(aPenSize, maxWidth);
}


float ClampTextPenSize(float aPenSize, int aSize, bool aStrict = false)
{
    double scale = aStrict ? 0.18 : 0.25;
    float  maxWidth = (float) aSize * scale;

    return std::min(aPenSize, maxWidth);
}


int ClampTextPenSize(int aPenSize, const VECTOR2I& aSize, bool aStrict = false)
{
    int size = std::min(std::abs(aSize.x), std::abs(aSize.y));

    return ClampTextPenSize(aPenSize, size, aStrict);
}

