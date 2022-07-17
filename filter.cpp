#include "filter.h"

Filter::StereoFilter::StereoFilter() {}
Filter::StereoFilter::~StereoFilter() {}

void Filter::StereoFilter::prepare(double sr)
{
    sampleRate = sr;

    reset();

    updateCoefficients();
}

void Filter::StereoFilter::process(QBuffer buffer, const int numSamples)
{
    for (int ch = buffer.getNumChannels(); --ch >= 0;)
            iirFilters.getUnchecked(ch)->processSamples(buffer.getWritePointer(ch), numSamples);
}

void Filter::StereoFilter::setFrequency(float newValue)
{
    frequency = std::min(newValue, (float)(sampleRate * 0.499));
    updateCoefficients();
}

void Filter::StereoFilter::setQuality(float newValue)
{
    quality = newValue;
    updateCoefficients();
}

void Filter::StereoFilter::setType(float newValue)
{
    type = static_cast<int>(newValue);
}

void Filter::StereoFilter::reset()
{
    for (int f = iirFilters.size(); --f >= 0;)
        iirFilters.getUnchecked(f)->reset();
}

float Filter::StereoFilter::getFrequency()
{
    return frequency;
}

void Filter::StereoFilter::updateCoefficients()
{
    DSP::IIR::Coefficients</*Qbuffer cells type*/> iirCoeffs;

    switch (type)
    {
    case 0:
        iirCoeffs = DSP::IIR::ArrayCoefficients</*Qbuffer cells type*/>::makeAllPass(sampleRate, frequency, quality);
        break;
    case 1:
        iirCoeffs = DSP::IIR::ArrayCoefficients</*Qbuffer cells type*/>::makeLowPass(sampleRate, frequency, quality);
        break;
    case 2:
        iirCoeffs = DSP::IIR::ArrayCoefficients</*Qbuffer cells type*/>::makeHighPass(sampleRate, frequency, quality);
        break;
    case 3:
        iirCoeffs = DSP::IIR::ArrayCoefficients</*Qbuffer cells type*/>::makeBandPass(sampleRate, frequency, quality);
        break;
    default:
        break;
    }

    for (int f = iirFilters.size(); --f >= 0;)
        iirFilters.getUnchecked(f)->setCoefficients(iirCoeffs);
}
