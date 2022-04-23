#include "bsplinecurveevaluator.h"
#include <assert.h>
#include "vec.h"
#include "mat.h"

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
    ptvEvaluatedCurvePts.clear();

    std::vector<Point> poor;
    poor.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
    poor.insert(poor.begin(), 2, ptvCtrlPts[0]);
    poor.insert(poor.end(), 2, ptvCtrlPts[ptvCtrlPts.size() - 1]);

    for(int i = 0; i < poor.size() - 3; ++i)
	{
		Mat4f Q( -1, +3, -3, +1,
				 +3, -6, +3, +0,
				 -3, +0, +3, +0, 
				 +1, +4, +1, +0);
		Vec4f xx( poor[i + 0].x, poor[i + 1].x, poor[i + 2].x, poor[i + 3].x);
		Vec4f yy( poor[i + 0].y, poor[i + 1].y, poor[i + 2].y, poor[i + 3].y);

		for(float t = 0; t <= 1; t += 0.01)
		{
			Vec4f T( pow(t, 3), pow(t, 2), t, 1);
			Vec4f res(T * Q / 6.0);
			ptvEvaluatedCurvePts.push_back( Point( res * xx, res * yy ));
		}
	}

    int iCtrlPtCount = poor.size();

	float x = 0.0;
	float y1;

	if (bWrap) {
		// if wrapping is on, interpolate the y value at xmin and
		// xmax so that the slopes of the lines adjacent to the
		// wraparound are equal.

		if ((poor[0].x + fAniLength) - poor[iCtrlPtCount - 1].x > 0.0f) {
			y1 = (poor[0].y * (fAniLength - poor[iCtrlPtCount - 1].x) + 
				  poor[iCtrlPtCount - 1].y * poor[0].x) /
				 (poor[0].x + fAniLength - poor[iCtrlPtCount - 1].x);
		}
		else 
			y1 = ptvCtrlPts[0].y;
	}
	else {
		// if wrapping is off, make the first and last segments of
		// the curve horizontal.

		y1 = poor[0].y;
    }

	ptvEvaluatedCurvePts.push_back(Point(x, y1));

	/// set the endpoint based on the wrap flag.
	float y2;
    x = fAniLength;
    if (bWrap)
		y2 = y1;
    else
		y2 = poor[iCtrlPtCount - 1].y;

	ptvEvaluatedCurvePts.push_back(Point(x, y2));
}
