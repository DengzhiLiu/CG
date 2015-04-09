#include "raytracer.h"

/* a global scene pointer, since we need it to render the scene. */
extern Scene *myScene;

/* This function takes in an image and a scene, traces a ray through every */
/*    pixel in the scene, and stores the color in the image.               */
void ShootRays( Image &image, Scene *scene )
{
        for (int y=0; y<image.GetYRes(); y++)
                for (int x=0; x<image.GetXRes(); x++)
                {
                        Ray r( scene->GetCamera()->GetEye(), scene->GetCamera()->GenerateRay( (double)x, (double)y ) );
                        image(x,y) = scene->RayTrace(r, 0);
                }
}

/* This routine creates an image, check to make sure there is a scene       */
/*    defined, and then calls the ShootRays() function to create the image, */
/*    before saving the result to the file "myResult.ppm".                  */
int BatchModeMain(int argc, char** argv)
{
        if (!myScene)
        {
          std::cout << "BatchModeMain() called, but the scene hasn't been initialized!!\n";
          exit(0);
        }

        Image myImage( myScene->GetWidth(), myScene->GetHeight());

        ShootRays( myImage, myScene );
        myImage.Save( "myResult.ppm" );

        return 0;
}

