#include "raytracer.h"

/* A global scene pointer                                         */
/*   This needs to be global for the OpenGLInterface, as we need  */
/*   access to it in GLUT's display() routine                     */
Scene* myScene;

int main(int argc, char** argv)
{
  /* creates a new scene */
  myScene = new Scene();

  /* sets up this scene */
  SetupHomeworkScene( myScene );

  /* if the program is run as "<myProgName> -b" it outputs a single image  */
  if (argc > 1 && !strcmp(argv[1], "-b"))
        return BatchModeMain( argc, argv );

  /* otherwise, it opens up an OpenGL window which displays the result and */
  /*     allows you to move around the scene (albeit slowly)               */
  return OpenGLMain( argc, argv );
}

