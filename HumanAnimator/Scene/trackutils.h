#ifndef _TRACKUTILS_H
#define _TRACKUTILS_H

#include <vector>
#include <gl/gl.h>
using namespace std;

namespace trackutils {

	// drawing section
	class DrawingHint {
	public:

		DrawingHint () {
			CircleStep = 64;
			HideStill = false;
			DrawTrack = false;
			LineWidthStill = 0.5f;
			LineWidthMoving = 1.5f;
			//color = Color4b::LightBlue;
		}
		/// The circles resolution.
		int CircleStep;
		/// currently not in use.
		bool HideStill;
		/// currently not in use.
		bool DrawTrack;
		/// circle color
		//Color4b color;
		/// circle line width when inactive.
		float LineWidthStill;
		/// circle line width when active.
		float LineWidthMoving;
	};

	DrawingHint DH;

	void DrawPlaneHandle ()
	{
	  float r = 1.0;
	  float dr = r / 10.0f;

	  glBegin (GL_LINE_STRIP);
	  glVertex3f (+r + dr, +r, 0.0);
	  glVertex3f (+r, +r + dr, 0.0);
	  glVertex3f (+r - dr, +r, 0.0);
	  glVertex3f (+r, +r - dr, 0.0);
	  glVertex3f (+r + dr, +r, 0.0);
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	  glVertex3f (-r + dr, -r, 0.0);
	  glVertex3f (-r, -r + dr, 0.0);
	  glVertex3f (-r - dr, -r, 0.0);
	  glVertex3f (-r, -r - dr, 0.0);
	  glVertex3f (-r + dr, -r, 0.0);
	  glEnd ();
	}


	void DrawCircle ()
	{
	  int nside = DH.CircleStep;
	  const double pi2 = 3.14159265 * 2.0;
	  glBegin (GL_LINE_LOOP);
	  for (double i = 0; i < nside; i++) {
		glNormal3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
		glVertex3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
	  }
	  glEnd ();
	}


	void DrawSphereIcon (Trackball * tb,bool active)
	{  
	  glPushAttrib (GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	  glMatrixMode(GL_MODELVIEW);
	  glPushMatrix ();
	  //glTranslatef (tb->center[0], tb->center[1], tb->center[2]);
	  //glMultMatrix (tb->track.InverseMatrix ());
	  //glScale (tb->radius);
	  //Matrix44f r;
	  //tb->track.rot.ToMatrix (r);
	  //glMultMatrix (r);
	  
	  float amb[4] = { .3f, .3f, .3f, 1.0f };
	  float col[4] = { .5f, .5f, .8f, 1.0f };
	  glEnable (GL_LINE_SMOOTH);
	  if (active)
		glLineWidth (DH.LineWidthMoving);
	  else
		glLineWidth (DH.LineWidthStill);
	  glEnable (GL_LIGHTING);
	  glEnable (GL_LIGHT0);
	  glEnable (GL_BLEND);
	  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  //glColor (DH.color);
	  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, amb);
	  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
		
		DrawCircle ();
		glRotatef (90, 1, 0, 0);
		DrawCircle ();
		glRotatef (90, 0, 1, 0);
		DrawCircle ();
		
		glPopMatrix ();
		glPopAttrib ();
	}


	void prepare_attrib()
	{
	  float amb[4] = { .3f, .3f, .3f, 1.0f };
	  float col[4] = { .5f, .5f, .8f, 1.0f };
	  glEnable (GL_LIGHTING);
	  glEnable (GL_LIGHT0);
	  glEnable (GL_LINE_SMOOTH);
	  glEnable (GL_BLEND);
	  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, amb);
	  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
	}

}
#endif
