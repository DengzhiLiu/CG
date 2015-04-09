#ifndef _RAYTRACER_H
#define _RAYTRACER_H

#include <iostream>
#include <stdio.h>

#include "Image.h"
#include "Camera.h"
#include "Vector.h"
#include "Point.h"
#include "Color.h"
#include "Scene.h"
//#include "Sphere.h"
//#include "InfinitePlane.h"
#include "ConstantColorMaterial.h"
#include "LambertianMaterial.h"
#include "Light.h"
#include "Definitions.h"

using namespace std;


/* scene setup functions */
void SetupHomeworkScene( Scene *scene );

/* The main entry point if using OpenGL */
int OpenGLMain(int argc, char** argv);

/* The main entry point if writing an image to a PPM */
int BatchModeMain(int argc, char** argv);

/* Functions used in the OpenGL mode display */
void idle( void );
void display( void );
void reshape( int w, int h );
void keys(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void DisplayTimer( void );
unsigned char ConvertToUnsigned( double val );


#endif

