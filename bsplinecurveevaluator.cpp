#include "bsplinecurveevaluator.h"
#include "beziercurveevaluator.h"
#include <assert.h>
#include "vec.h"
#include "mat.h"

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
    ptvEvaluatedCurvePts.clear();

    std::vector<Point> boor;

	Mat4f Q( -1, +3, -3, +1,
			+3, -6, +3, +0,
			-3, +0, +3, +0, 
			+1, +4, +1, +0);

	if (ptvCtrlPts.size() == 2 && !bWrap)
	{
		boor.insert(boor.begin(), 3, ptvCtrlPts.front());
		boor.insert(boor.end(), 2, ptvCtrlPts.back());
	}
	else if (bWrap)
	{
		boor.push_back( Point((ptvCtrlPts.end() - 2)->x - fAniLength,(ptvCtrlPts.end() - 2)->y) );
		boor.push_back( Point((ptvCtrlPts.end() - 1)->x - fAniLength,(ptvCtrlPts.end() - 1)->y) );
		boor.insert(boor.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		boor.push_back( Point((ptvCtrlPts.begin())->x + fAniLength, (ptvCtrlPts.begin())->y) );
		boor.push_back( Point((ptvCtrlPts.begin() + 1)->x + fAniLength, (ptvCtrlPts.begin() + 1)->y) );
	}
	else if (!bWrap)
	{
		boor.push_back(ptvCtrlPts.front());
		boor.push_back(ptvCtrlPts.front());
		boor.insert(boor.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		boor.push_back(ptvCtrlPts.back());
		boor.push_back(ptvCtrlPts.back());
	}

	float bound, start;
	bool used = 0, touched = 0;
	
	for(int i = 0; i < boor.size() - 3; ++i)
	{
		Vec4f xx( boor[i + 0].x, boor[i + 1].x, boor[i + 2].x, boor[i + 3].x);
		Vec4f yy( boor[i + 0].y, boor[i + 1].y, boor[i + 2].y, boor[i + 3].y);

		for(float t = 0; t <= 1; t += 0.01)
		{
			Vec4f T( pow(t, 3), pow(t, 2), t, 1);
			Vec4f res(T * Q / 6.0);
			// if (res * xx <= fAniLength)
			// {
			// 	bound = res * yy;
			// 	used = 1;
			// 	ptvEvaluatedCurvePts.push_back( Point( res * xx, res * yy ) );
			// }
			// else
			// 	ptvEvaluatedCurvePts.push_back( Point( res * xx - fAniLength, res * yy ) );
			if (res * xx <= fAniLength && res * xx >= 0)
			{
				bound = res * yy;
				ptvEvaluatedCurvePts.push_back( Point( res * xx, res * yy ));
			}
			else if (res * xx > fAniLength)
			{
				used = 1;
				ptvEvaluatedCurvePts.push_back( Point( res * xx - fAniLength, res * yy ));
			}
			else if (res * xx < 0)
			{
				touched = 1;
				start = res * yy;
				ptvEvaluatedCurvePts.push_back( Point( res * xx + fAniLength, res * yy ));
			}
		}
	}

	// float x = 0.0;
	// float y1;

	// int iCtrlPtCount = ptvCtrlPts.size();

	// if (bWrap)
	// {
	// 	if (used)
	// 		y1 = bound;
	// 	else if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
	// 		y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) + 
	// 			  ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
	// 			 (ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
	// 	}
	// 	else 
	// 		y1 = ptvCtrlPts[0].y;
	// }
	// else
	// 	y1 = ptvCtrlPts[0].y;

	// ptvEvaluatedCurvePts.push_back(Point(x, y1));
	if (!bWrap)
		ptvEvaluatedCurvePts.push_back( Point(fAniLength, ptvEvaluatedCurvePts.back().y) );
	// else if (touched)
	// 	ptvEvaluatedCurvePts.push_back( Point(fAniLength, start) );
	// else
	// 	ptvEvaluatedCurvePts.push_back( Point(fAniLength, ptvEvaluatedCurvePts.begin()->y) );

	if (!bWrap)
		ptvEvaluatedCurvePts.push_back( Point(0, ptvEvaluatedCurvePts.front().y) );
	// else if (used)
	// 	ptvEvaluatedCurvePts.push_back( Point(0, bound) );
	// else
	// 	ptvEvaluatedCurvePts.push_back( Point(0, (ptvEvaluatedCurvePts.end() - 1)->y) );

	// float y2;
    // x = fAniLength;
    // if (bWrap && iCtrlPtCount <= 3)
	// {
	// 	y2 = y1;
	// 	ptvEvaluatedCurvePts.push_back(Point(x, y2));
	// }
    // else if (!bWrap)
	// {
	// 	y2 = ptvCtrlPts[iCtrlPtCount - 1].y;
	// 	ptvEvaluatedCurvePts.push_back(Point(x, y2));
	// }
}
