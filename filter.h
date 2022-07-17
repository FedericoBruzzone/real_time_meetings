#include <docs_filter.h>
#include <vector>

#include <QBuffer>

#ifndef FILTER_H
#define FILTER_H

#define DEFAULT_FILTER_FREQUENCY 8000.0f
#define DEFAULT_FILTER_QUALITY 0.707f

namespace Filter
{
    class StereoFilter
    {
    public:
        StereoFilter();
        ~StereoFilter();

        void prepare(double sr);

        void process(QBuffer buffer, const int numSamples);

        void setFrequency(float newValue);

        void setQuality(float newValue);

        void setType(float newValue);

        void reset();

        float getFrequency();

    private:
        void updateCoefficients();

        float frequency = DEFAULT_FILTER_FREQUENCY;
        float quality = DEFAULT_FILTER_QUALITY;
        double sampleRate = 44100.0;
        int type = 0;

        std::vector<DSP::IIR::Filter<QBuffer>> iirFilters;

    };

#endif // FILTER_H
}
