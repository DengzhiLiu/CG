/**
* $Id: SceneObject.h
*
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*
* The Original Code is Copyright (C) 日本電信電話株式会社 2009
* All rights reserved.
*
*/

#ifndef _SCENEOBJECT_H
#define _SCENEOBJECT_H

#include <gl/glew.h>

// drawing section
class DrawingHint {
public:

	DrawingHint () {
		CircleStep = 64;
		HideStill = false;
		DrawTrack = false;
		LineWidthStill = 0.5f;
		LineWidthMoving = 1.5f;
		color[0] = 0.03f;
		color[1] = 0.03f;
		color[2] = 1.f;
		color[3] = 1.f;
	}

	int		CircleStep;			// The circles resolution.
	bool	HideStill;			// currently not in use.
	bool	DrawTrack;			// currently not in use.
	GLfloat color[4];			// circle color
	float	LineWidthStill;		// circle line width when inactive.
	float	LineWidthMoving;	// circle line width when active.
};

class SceneObject
{
public:
	DrawingHint	DH;

	void DrawCircle ();

	void Cone(int slices, float lenght, float width);

	void Cylinder(int slices, float lenght, float width);

	void DrawCoordinate();

	void DrawSphereIcon (bool active);
};

#endif // _SCENEOBJECT_H