#ifndef _BEZIER_PATH_H
#include "mathHelper.hpp"

class BezierPath
{
    private:
        std::vector<BezierCurve> mCurves;
        std::vector<int> mSamples;

    public:
        BezierPath();
        ~BezierPath();

        void AddCurve(BezierCurve curve, int sample);

        void Sample(std::vector<vector2D>* sampledPath);
        void SampleEqui(std::vector<vector2D>* sampledPath);      
};

BezierPath::BezierPath()
{

}

BezierPath::~BezierPath()
{

}

void BezierPath::AddCurve(BezierCurve curve, int sample)
{
    mCurves.push_back(curve);
    mSamples.push_back(sample);
}


#define SIZESQR 36
void BezierPath::Sample(std::vector<vector2D>* sampledPath)
{
    for(int i = 0; i < mCurves.size(); i++)
    {
        for(float t = 0.0f; t <= 1.0f; t += (1.0f / mSamples[i]))
        {
                sampledPath->push_back(mCurves[i].CalculateCurvePoint(t));
        }
    }
}

void BezierPath::SampleEqui(std::vector<vector2D>* sampledPath)
{
    vector2D v, tmp;
    for(int i = 0; i < mCurves.size(); i++)
    {
        for(float t = 0.0f; t <= 1.0f; t += (1.0f / 10000.0f))
        {
            v = mCurves[i].CalculateCurvePoint(t);
            if(sampledPath->size() > 0)
                tmp = v - *(sampledPath->end() - 1);
            if((tmp.magnitudeSqr() >= SIZESQR - 15.0f && tmp.magnitudeSqr() <= SIZESQR + 15.0f) || sampledPath->size() == 0)
                sampledPath->push_back(v);
        }
    }
}

#endif
