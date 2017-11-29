#include "Bezier.h"
#include <GL/glut.h>

Bezier::Bezier()
{
}

void Bezier::binomial(int n, int *C){
    
    int k, j;
    
    for(k = 0; k <= n; k++){
        C[k] = 1;
        for(j = n; j >= k + 1; j-- ){
            C[k] *= j;
        }
        for(j = n - k; j >= 2; j--){
            C[k] /= j;
        }
    } 
}


void Bezier::computeBezPt(float u, vpt *bezPt, int numCtr, std::list<vpt> &ctrPts, int *C){
    
    int k = 0, n = numCtr - 1;
    float bezBlendFcn;
    
    bezPt->x = bezPt->y = 0.0;
    
    for(auto itr = ctrPts.begin(); itr != ctrPts.end(); itr++){
        
        bezBlendFcn = C[k] * pow(u, k) * pow(1 - u, n - k);
        bezPt->x += itr->x * bezBlendFcn;
        bezPt->y += itr->y * bezBlendFcn;
        k++;
    }
    
    
}
void Bezier::bezier(std::list<vpt> &ctrPts, int numCtr, int nBezCurvePts){
    
    vpt bezCurvePt;
    float u;
    GLint *C, k;
    
    C = new int[numCtr];
    
    binomial(numCtr - 1, C);
    
    for(k = 0; k <= nBezCurvePts; k++){
        
        u = float(k) / float(nBezCurvePts);
        computeBezPt(u, &bezCurvePt, numCtr, ctrPts, C);
        plotPoint(bezCurvePt);
        
    }
    
    delete [] C;
    
}

void Bezier::plotPoint(vpt bezCurvePt){
    
    glPointSize(1.0);
    glBegin(GL_POINTS);
        glVertex2f(bezCurvePt.x, bezCurvePt.y);
    glEnd();
}