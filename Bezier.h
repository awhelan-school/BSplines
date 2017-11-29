#ifndef BEZIER_H
#define BEZIER_H

#include <cmath>
#include <list>

typedef struct{
    float x,y;
    int ID;
}vpt;

class Bezier {
public:
    Bezier();
    virtual ~Bezier(){};
    /*Compute Binomial Coefficients C for a given value of n*/
    static void binomial(int n, int *C);
    static void computeBezPt(float u, vpt *bezPt, int numCtr, std::list<vpt> &ctrPts, int *C);
    static void bezier(std::list<vpt> &ctrPts, int numCtr, int nBezCurvePts);
    static void plotPoint(vpt bezCurvePt);
private:

};

#endif /* BEZIER_H */

