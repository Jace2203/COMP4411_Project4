#include "CatmullRomCurveEvaluator.h"
#include <assert.h>

#include "vec.h"
#include "mat.h"
#include <algorithm>

void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	int sample = 50;
	std::vector<Point> CtrlPts;
	ptvEvaluatedCurvePts.clear();

	if (bWrap)
	{
		const Point& last_2 = *(ptvCtrlPts.end() - 2);
		const Point& last_1 = *(ptvCtrlPts.end() - 1);

		CtrlPts.push_back(Point(last_2.x - fAniLength, last_2.y));
		CtrlPts.push_back(Point(last_1.x - fAniLength, last_1.y));
	}
	else {
		CtrlPts.push_back(Point(0.0f, ptvCtrlPts.begin()->y));
	}

	CtrlPts.insert(CtrlPts.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
	
	if (bWrap)
	{
		const Point& start_1 = *(ptvCtrlPts.begin());
		const Point& start_2 = *(ptvCtrlPts.begin() + 1);

		CtrlPts.push_back(Point(start_1.x + fAniLength, start_1.y));
		CtrlPts.push_back(Point(start_2.x + fAniLength, start_2.y));
	}
	else {
		CtrlPts.push_back(Point(fAniLength, (ptvCtrlPts.end() - 1)->y));
	}

	Mat4f M(-1.0f,  3.0f, -3.0f,  1.0f,
			 2.0f, -5.0f,  4.0f, -1.0f,
			-1.0f,  0.0f,  1.0f,  0.0f,
			 0.0f,  2.0f,  0.0f,  0.0f);

	if (!bWrap)
		ptvEvaluatedCurvePts.push_back(Point(0.0f, CtrlPts.begin()->y));

	int mx;
	if (bWrap)
		mx = iCtrlPtCount;
	else if (iCtrlPtCount == 2)
		mx = 0;
	else
		mx = iCtrlPtCount - 1;

	for (int i = 0; i < mx; i++)
	{
		const Point& P0 = *(CtrlPts.begin() + i);
		const Point& P1 = *(CtrlPts.begin() + i + 1);
		const Point& P2 = *(CtrlPts.begin() + i + 2);
		const Point& P3 = *(CtrlPts.begin() + i + 3);
		Vec4f P(P0.y, P1.y, P2.y, P3.y);
		Vec4f MP = M * P;

		float tMin = P1.x, tMax = P2.x;
		float step = (tMax - tMin) / sample;

		int j = 0;
		if (bWrap && i == 0)
			j = (0.0 - tMin) / step;
		for (j; j < sample; j++)
		{
			float t = (float)j / sample;
			float tt = t * t;
			float ttt = tt * t;

			Vec4f T(ttt, tt, t, 1.0f);
			float y = 0.5f * T * MP;

			ptvEvaluatedCurvePts.push_back(Point(tMin + j * step, y));
		}
	}

	if (bWrap)
	{
		const Point& P0 = *(CtrlPts.end() - 1 - 3);
		const Point& P1 = *(CtrlPts.end() - 1 - 2);
		const Point& P2 = *(CtrlPts.end() - 1 - 1);
		const Point& P3 = *(CtrlPts.end() - 1);
		Vec4f P(P0.y, P1.y, P2.y, P3.y);
		Vec4f MP = M * P;

		float tMin = P1.x, tMax = P2.x;
		float step = (tMax - tMin) / sample;

		int mx = (fAniLength - tMin) / step;
		for (int j = 0; j < mx; j++)
		{
			float t = (float)j / sample;
			float tt = t * t;
			float ttt = tt * t;

			Vec4f T(ttt, tt, t, 1.0f);
			float y = 0.5f * T * MP;

			ptvEvaluatedCurvePts.push_back(Point(tMin + j * step, y));
		}
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvEvaluatedCurvePts.begin()->y));
	}
	else 
	{
		if (iCtrlPtCount > 2)
			ptvEvaluatedCurvePts.push_back(*(ptvCtrlPts.end() - 1));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, (CtrlPts.end() - 1)->y));
	}
}
