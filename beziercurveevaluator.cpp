#include "BezierCurveEvaluator.h"
#include <assert.h>
#include "vec.h"
#include "mat.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
    Vec4f T;
    Mat4f Q( -1, +3, -3, +1,
             +3, -6, +3, +0,
             -3, +3, +0, +0, 
             +1, +0, +0, +0);
    Vec4f xx( ptvCtrlPts[0].x, ptvCtrlPts[1].x, ptvCtrlPts[2].x, ptvCtrlPts[3].x);
    Vec4f yy( ptvCtrlPts[0].y, ptvCtrlPts[1].y, ptvCtrlPts[2].y, ptvCtrlPts[3].y);

    for(float t = 0; t < 1; t += 0.1)
    {
        T = Vec4f( pow(t, 3), pow(t, 2), t, 1);
        Vec4f res(T * Q);
        ptvEvaluatedCurvePts.push_back( Point( T * xx, T * yy ));
    }

    ptvEvaluatedCurvePts.push_back(Point(0, 0));
    ptvEvaluatedCurvePts.push_back(Point(0, 0));
}

// #include "BezierCurveEvaluator.h"
// #include <assert.h>

// void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
// 										 std::vector<Point>& ptvEvaluatedCurvePts, 
// 										 const float& fAniLength, 
// 										 const bool& bWrap) const
// {
// 	int iCtrlPtCount = ptvCtrlPts.size();

// 	ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

// 	float x = 0.0;
// 	float y1;

// 	if (bWrap) {
// 		// if wrapping is on, interpolate the y value at xmin and
// 		// xmax so that the slopes of the lines adjacent to the
// 		// wraparound are equal.

// 		if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
// 			y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) + 
// 				  ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
// 				 (ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
// 		}
// 		else 
// 			y1 = ptvCtrlPts[0].y;
// 	}
// 	else {
// 		// if wrapping is off, make the first and last segments of
// 		// the curve horizontal.

// 		y1 = ptvCtrlPts[0].y;
//     }

// 	ptvEvaluatedCurvePts.push_back(Point(x, y1));

// 	/// set the endpoint based on the wrap flag.
// 	float y2;
//     x = fAniLength;
//     if (bWrap)
// 		y2 = y1;
//     else
// 		y2 = ptvCtrlPts[iCtrlPtCount - 1].y;

// 	ptvEvaluatedCurvePts.push_back(Point(x, y2));
// }
