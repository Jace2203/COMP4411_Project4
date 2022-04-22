#include "linearcurveevaluator.h"
#include "BezierCurveEvaluator.h"
#include <assert.h>
#include "vec.h"
#include "mat.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();
	int iCtrlPtCount = ptvCtrlPts.size();

	//ptvEvaluatedCurvePts.push_back( Point( 0, ptvCtrlPts[0].y ));

	while (iCtrlPtCount > 3)
	{
		Mat4f Q( -1, +3, -3, +1,
				 +3, -6, +3, +0,
				 -3, +3, +0, +0, 
				 +1, +0, +0, +0);
		Vec4f xx( ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 0].x, ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 1].x, ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 2].x, ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 3].x);
		Vec4f yy( ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 0].y, ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 1].y, ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 2].y, ptvCtrlPts[ptvCtrlPts.size() - iCtrlPtCount + 3].y);

		for(float t = 0; t <= 1; t += 0.01)
		{
			Vec4f T( pow(t, 3), pow(t, 2), t, 1);
			Vec4f res(T * Q);
			ptvEvaluatedCurvePts.push_back( Point( res * xx, res * yy ));
		}
		iCtrlPtCount -= 3;
	}

	for(int i = ptvCtrlPts.size() - iCtrlPtCount; i < ptvCtrlPts.size(); ++i)
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i]);

	iCtrlPtCount = ptvCtrlPts.size();

	float x = 0.0;
	float y1;

	if (bWrap) {
		// if wrapping is on, interpolate the y value at xmin and
		// xmax so that the slopes of the lines adjacent to the
		// wraparound are equal.

		if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
			y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) + 
				  ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
				 (ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
		}
		else 
			y1 = ptvCtrlPts[0].y;
	}
	else {
		// if wrapping is off, make the first and last segments of
		// the curve horizontal.

		y1 = ptvCtrlPts[0].y;
    }

	ptvEvaluatedCurvePts.push_back(Point(x, y1));

	/// set the endpoint based on the wrap flag.
	float y2;
    x = fAniLength;
    if (bWrap)
		y2 = y1;
    else
		y2 = ptvCtrlPts[iCtrlPtCount - 1].y;

	ptvEvaluatedCurvePts.push_back(Point(x, y2));
}
