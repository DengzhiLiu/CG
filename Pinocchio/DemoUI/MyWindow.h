/*
Copyright (c) 2007 Ilya Baran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <FL/Fl_Gl_Window.H>
#include "../Pinocchio/mesh.h"
#include "../Pinocchio/transform.h"
#include "../Pinocchio/pinocchioApi.h"
#include "DisplayMesh.h"

struct LineSegment
{
    LineSegment() : thickness(1.) {}
    LineSegment(const Vector3 &inP1, const Vector3 &inP2,
        const Vector3 &inColor = Vector3(1, 1, 1), double inThickness = 1.)
        : p1(inP1), p2(inP2), color(inColor), thickness(inThickness) {}

    Vector3 p1, p2, color;
    double thickness;
};

class MyWindow : public Fl_Gl_Window
{
public:
    MyWindow();

    virtual ~MyWindow() {}
    virtual void draw();
    virtual int handle(int);

    void addMesh(DisplayMesh *inM) { meshes.push_back(inM); }
	void addSphere(vector<Sphere> insph) { spheres = insph; }
	void addMatchPoint(vector<Vector3> inmatch) { matchPoint = inmatch; }
	void addOriginPoints(vector<Vector3> inorigin) { originPoints = inorigin; }
    void addLine(const LineSegment &l) { lines.push_back(l); }
	void addOriginLines(LineSegment &l) { originLines.push_back(l); }
	void addExtractLines(LineSegment &l) { extractlines.push_back(l); }
	void setCIdx(vector<int> inidx) { cPoints = inidx; }
	void addLimbVerts(Vector3 pos) { limbVerts.push_back(pos); }
	void addEmbeddingV(Vector3 pos) { EmbeddingV.push_back(pos); }
	void addDiscreteEmbedV(vector<Vector3> inDis) { discreteEmbedding = inDis; }
    void clearLines() { lines.clear(); }

private:
	//int frame_no;
	//float animation_time;
    bool flatShading, floor, skeleton;
	bool drawsph, drawTri, drawh, drawline, drawskl, drawbefore, onAnimation;
    Transform<> transform;
    vector<DisplayMesh *> meshes;
    vector<LineSegment> lines;
	vector<LineSegment> originLines;
	vector<LineSegment> extractlines;
	vector<Sphere> spheres;
	vector<Vector3> matchPoint;
	vector<Vector3> originPoints;
	vector<int> cPoints;
	vector<Vector3> limbVerts;
	vector<Vector3> EmbeddingV;
	vector<Vector3> discreteEmbedding;

    void resetTransform();
    void initGL();
    void drawMesh(const Mesh &m, bool flatShading, Vector3 trans = Vector3());
	void drawSphere(vector<Sphere> spheres, bool flatShading);
    void drawFloor();
};

#endif //MYWINDOW_H
