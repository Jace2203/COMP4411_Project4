#include "linearcurveevaluator.h"
#include "laneriesenfeldcurveevaluator.h"
#include <assert.h>
#include "vec.h"
#include "mat.h"

void LaneRiensenfeldCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();

	std::vector<Point> ctrl;
	if (bWrap)
        ctrl.push_back(Point(ptvCtrlPts.back().x - fAniLength ,ptvCtrlPts.back().y));
    
	ctrl.insert(ctrl.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
        
    if (bWrap)
        ctrl.push_back(Point((*ptvCtrlPts.begin()).x + fAniLength, (*ptvCtrlPts.begin()).y));

    for (int d = 0; d < 5; d++)
    {
        std::vector<Point> R;
        int iCtrlPtCount = ctrl.size();

        for (int i = 0; i < iCtrlPtCount - 1; i++)
        {
            R.push_back(Point(ctrl[i]));
            R.push_back(Point((ctrl[i].x + ctrl[i + 1].x) / 2.0, (ctrl[i].y + ctrl[i + 1].y) / 2.0));
        }
        R.push_back(Point(ctrl.back()));

        Point p0 = *(ctrl.begin()), pn = ctrl.back();
        ctrl.clear();

        ctrl.push_back(Point(p0));
        for (int i = 0; i < iCtrlPtCount * 2 - 2; i++)
            ctrl.push_back(Point((R[i].x + R[i + 1].x) / 2.0, (R[i].y + R[i + 1].y) / 2.0));
        ctrl.push_back(Point(pn));
    }

    bool start = false;
    auto it = ctrl.begin();
    for (it; it != ctrl.end(); it++)
    {
        if (!start && (*it).x > 0)
        {
            if (it == ctrl.begin())
            {
                ptvEvaluatedCurvePts.push_back(Point(0, (*it).y));
            }
            else
            {
                Point p1 = ctrl[it - ctrl.begin() - 1];
                Point p2 = *(ctrl.begin());
                float dy = (p2.y - p1.y) / (p2.x - p1.x);
                float y = p1.y + dy * (0 - p1.x);
                ptvEvaluatedCurvePts.push_back(Point(0, y));
            }
            start = true;
        }

        ptvEvaluatedCurvePts.push_back(Point(*it));
        if ((*it).x > fAniLength)
            break;
    }

    if (it == ctrl.end())
    {
        ptvEvaluatedCurvePts.push_back(Point(fAniLength, ctrl.back().y));
    }
    else
    {
        Point p1 = ctrl[it - ctrl.begin() - 1];
        Point p2 = *it;
        float dy = (p2.y - p1.y) / (p2.x - p1.x);
        float y = p1.y + dy * (fAniLength - p1.x);
        ptvEvaluatedCurvePts.push_back(Point(0, y));
    }
}
