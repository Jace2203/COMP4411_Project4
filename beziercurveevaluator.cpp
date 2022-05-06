#include "linearcurveevaluator.h"
#include "BezierCurveEvaluator.h"
#include <assert.h>
#include "vec.h"
#include "mat.h"
#include "modelerapp.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();

	float epsilon = ModelerApplication::Instance()->GetEpsilon();
	bool adaptive = ModelerApplication::Instance()->GetAdaptiveBcurce();

	printf("%d\n", adaptive);

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
			Point V0 = ctrl[ctrl.size() - iCtrlPtCount + 0],
				  V1 = ctrl[ctrl.size() - iCtrlPtCount + 1],
				  V2 = ctrl[ctrl.size() - iCtrlPtCount + 2],
				  V3 = ctrl[ctrl.size() - iCtrlPtCount + 3];

			Vec4f xx( ctrl[ctrl.size() - iCtrlPtCount + 0].x, ctrl[ctrl.size() - iCtrlPtCount + 1].x, ctrl[ctrl.size() - iCtrlPtCount + 2].x, ctrl[ctrl.size() - iCtrlPtCount + 3].x);
			Vec4f yy( ctrl[ctrl.size() - iCtrlPtCount + 0].y, ctrl[ctrl.size() - iCtrlPtCount + 1].y, ctrl[ctrl.size() - iCtrlPtCount + 2].y, ctrl[ctrl.size() - iCtrlPtCount + 3].y);

			if (adaptive)
				displayBezier(V0, V1, V2, V3, epsilon, ptvEvaluatedCurvePts);
			else
			{
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

	printf("%d\n", ptvEvaluatedCurvePts.size());
}

void displayBezier(Point V0, Point V1, Point V2, Point V3, float epsilon, std::vector<Point>& ptvEvaluatedCurvePts)
{
	if ((V0.distance(V1) + V1.distance(V2) + V2.distance(V3)) / V0.distance(V3) < 1 + epsilon)
	{
		ptvEvaluatedCurvePts.push_back(V0);
		ptvEvaluatedCurvePts.push_back(V3);
	}
	else
	{
		Point V0_( (V0.x + V1.x) / 2, (V0.y + V1.y) / 2 ),
			  V1_( (V1.x + V2.x) / 2, (V1.y + V2.y) / 2 ),
			  V2_( (V2.x + V3.x) / 2, (V2.y + V3.y) / 2 ),
			  V0__( (V0_.x + V1_.x) / 2, (V0_.y + V1_.y) / 2 ),
			  V1__( (V1_.x + V2_.x) / 2, (V1_.y + V2_.y) / 2 ),
			  Q( (V0__.x + V1__.x) / 2, (V0__.y + V1__.y) / 2 );

		displayBezier(V0, V0_, V0__, Q, epsilon, ptvEvaluatedCurvePts);
		displayBezier(Q, V1__, V2_, V3, epsilon, ptvEvaluatedCurvePts);
	}
}