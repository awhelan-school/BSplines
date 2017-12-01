#include "Bezier.h"
#include <iostream>
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


void Bezier::spline(int order, std::vector<float> knots, std::list<vpt> &ctrPts, int numCtr, int CurvePts){
    
    vpt bezCurvePt;
    float u;
    int start = order - 1;
    int segments = numCtr - order + 1;
    
    for(int i = 0; i < segments; i++){
        
        u = knots[start];
        
        std::cout << "U = "<< u << "\n";
        
        for(int k = 0; k < CurvePts; k++){
        
        float ratio = float(k) / float(CurvePts);
        bezCurvePt = computeSplinePt(order, ctrPts, knots, u+ratio, start);
        plotPoint(bezCurvePt);
        }
        
        start++;
    }
 
}

vpt Bezier::computeSplinePt(int order, std::list<vpt> ctrPts, std::vector<float> knots, float u, int I){
    
    std::vector <vpt> Pts;
    Pts.resize(ctrPts.size());
    int index = 0;
    
    for(auto pt : ctrPts){
        Pts[index++] = pt;   
    }
    
    for(int j = 1; j <= order - 1; j++){
        
        for(int i = I - (order - 1); i <= I - j; i++){
            

            Pts[i].x = ( (knots[i+order] - u)/(knots[i+order] - knots[i+j]) ) * Pts[i].x
                      + ( (u - knots[i+j])/(knots[i+order] - knots[i+j]) ) * Pts[i+1].x;
            
            Pts[i].y = ( (knots[i+order] - u)/(knots[i+order] - knots[i+j]) ) * Pts[i].y
                      + ( (u - knots[i+j])/(knots[i+order] - knots[i+j]) ) * Pts[i+1].y;
            
        }   
    }
    
    return Pts[I - (order - 1)];
}

void Bezier::plotPoint(vpt bezCurvePt){
    
    glPointSize(1.0);
    glBegin(GL_POINTS);
        glVertex2f(bezCurvePt.x, bezCurvePt.y);
    glEnd();
}