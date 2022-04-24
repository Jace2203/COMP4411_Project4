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

	Mat4f Q( -1, +3, -3, +1,
		+3, -6, +3, +0,
		-3, +3, +0, +0, 
		+1, +0, +0, +0);

	std::vector<Point> ctrl;
	ctrl.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap)
		ctrl.push_back( Point(ptvCtrlPts.front().x + fAniLength, ptvCtrlPts.front().y ));

	float bound;
	bool used = 0;

	int iCtrlPtCount = ctrl.size();

	if (iCtrlPtCount <= 3)
		ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
	else
	{
		while (iCtrlPtCount > 3)
		{
			Vec4f xx( ctrl[ctrl.size() - iCtrlPtCount + 0].x, ctrl[ctrl.size() - iCtrlPtCount + 1].x, ctrl[ctrl.size() - iCtrlPtCount + 2].x, ctrl[ctrl.size() - iCtrlPtCount + 3].x);
			Vec4f yy( ctrl[ctrl.size() - iCtrlPtCount + 0].y, ctrl[ctrl.size() - iCtrlPtCount + 1].y, ctrl[ctrl.size() - iCtrlPtCount + 2].y, ctrl[ctrl.size() - iCtrlPtCount + 3].y);

			for(float t = 0; t <= 1; t += 0.01)
			{
				Vec4f T( pow(t, 3), pow(t, 2), t, 1);
				Vec4f res(T * Q);
				if (res * xx <= fAniLength)
				{
					bound = res * yy;
					ptvEvaluatedCurvePts.push_back( Point( res * xx, res * yy ));
				}
				else
				{
					used = 1;
					ptvEvaluatedCurvePts.push_back( Point( res * xx - fAniLength, res * yy ));
				}
			}
			iCtrlPtCount -= 3;
		}

		while(--iCtrlPtCount >= 0)
			ptvEvaluatedCurvePts.push_back(ctrl[ctrl.size() - iCtrlPtCount - 1]);
	}

	float x = 0.0;
	float y1;

	iCtrlPtCount = ptvCtrlPts.size();

	if (bWrap)
	{
		if (used)
			y1 = bound;
		else if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
			y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) + 
				  ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
				 (ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
		}
		else 
			y1 = ptvCtrlPts[0].y;
	}
	else
		y1 = ptvCtrlPts[0].y;

	ptvEvaluatedCurvePts.push_back(Point(x, y1));

	float y2;
    x = fAniLength;
    if (bWrap)
	{
		y2 = y1;
		ptvEvaluatedCurvePts.push_back(Point(x, y2));
	}
    else if (!bWrap)
	{
		y2 = ptvCtrlPts[iCtrlPtCount - 1].y;
		ptvEvaluatedCurvePts.push_back(Point(x, y2));
	}
}
