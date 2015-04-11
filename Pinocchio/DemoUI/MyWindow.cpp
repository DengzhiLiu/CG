#include <FL/gl.h>
#include <FL/Fl.H>
#include <Windows.h>
#include <gl/glut.h>
#include "MyWindow.h"
#include "../Pinocchio/skeleton.h"

static HumanSkeleton human;

MyWindow *win = NULL;

void idle(void *s)
{
    if(win != NULL)
        win->redraw();
}

MyWindow::MyWindow() : Fl_Gl_Window(1024, 768, "Pinocchio"), flatShading(false), floor(true), skeleton(false), 
					drawTri(true), drawh(true), drawsph(false), drawline(false), drawskl(false), drawbefore(false),
					onAnimation(true)
{
    size_range(20, 20, 5000, 5000);
    end();

    resetTransform();
    win = this;
    Fl::add_idle(idle);
}

static int prevX, prevY;

int MyWindow::handle(int event) {
    switch(event) {
    case FL_PUSH:
        prevX = Fl::event_x();
        prevY = Fl::event_y();
        return 1;
    case FL_DRAG:
        if(Fl::event_state(FL_BUTTON3)) {
            int dx = Fl::event_x() - prevX, dy = Fl::event_y() - prevY;
            double len = sqrt(double(SQR(dx) + SQR(dy))) * 0.01;
            Transform<> cur = Transform<>(Vector3(0.5, 0.5, 0.5)) *
                Transform<>(Quaternion<>(Vector3(dy, dx, 0), len)) *
                Transform<>(Vector3(-0.5, -0.5, -0.5));

            transform = cur * transform;
        }
        else if(Fl::event_state(FL_BUTTON1)) {
            double scale = min(w(), h()) / 2.5;
            Transform<> cur = Transform<>(Vector3(Fl::event_x() - prevX, prevY - Fl::event_y(), 0) / scale);

            transform = cur * transform;
        }
        prevX = Fl::event_x();
        prevY = Fl::event_y();

        return 1;
    case FL_MOUSEWHEEL:
        {
            double scale = exp(-double(Fl::event_dy()) / 10.);

            Transform<> cur = Transform<>(Vector3(0.5, 0.5, 0.5)) *
                    Transform<>(scale) *
                    Transform<>(Vector3(-0.5, -0.5, -0.5));
                
            transform = cur * transform;
        }
        return 1;

    case FL_KEYBOARD:
        switch(Fl::event_key()) {
        case 't':
            resetTransform();
            break;
        case 'z':
            transform = Transform<>();
            break;
        case 's':
            skeleton = !skeleton;
            break;
        case 'f':
            flatShading = !flatShading;
            break;
        case 'g':
            floor = !floor;
            break;
		case 'a':
			drawTri = !drawTri;
			break;
		case 'm':
			onAnimation = !onAnimation;
			break;
		case 'h':
			drawh = !drawh;
			break;
		case 'l':
			drawline = !drawline;
			break;
		case 'p':
			drawsph = !drawsph;
			break;
		case 'd':
			drawskl = !drawskl;
			break;
		case 'b':
			drawbefore = !drawbefore;
			break;
        default:
            break;
        }
        return 1;
    default:
        // pass other events to the base class...
        return Fl_Gl_Window::handle(event);
    }
}

void MyWindow::draw() {
    int i;
    if (!valid()) { //Init viewport and projection
        initGL();

        double w = this->w(), h = this->h();
        glViewport(0, 0, (int)w, (int)h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        double left = -.1;
        double bottom = -.1;
        double right = 1.1;
        double top = 1.1;
        if(w > 1 && h > 1) {
            if(w > h) {
                right = -0.1 + 1.2 * w / h;
            }
            if(h > w) {
                bottom = 1.1 - 1.2 * h / w;
            }
        }

        double scale = 1. / 1000.;
        left = -w * scale;
        right = w * scale;
        bottom = -h * scale;
        top = h * scale;
        glFrustum(left, right, bottom, top, 5., 30.);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //Transform------
    Vector3 trans = transform.getTrans();
    glTranslated(trans[0], trans[1], -10 + trans[2]);
    
    double scale = transform.getScale();
    glScaled(scale, scale, scale);
    
    Quaternion<> r = transform.getRot();
    double ang = r.getAngle();
    if(fabs(ang) > 1e-6) {
        Vector3 ax = r.getAxis();
        glRotated(ang * 180. / M_PI, ax[0], ax[1], ax[2]);
    }

    //Draw----------
    if(floor)
        drawFloor();

	// get mesh
    vector<const Mesh *> ms(meshes.size());
    for(i = 0; i < (int)meshes.size(); ++i) {
        ms[i] = &(meshes[i]->getMesh(onAnimation));
    }

    //shadows
    if(floor) {
        Vector3 lightRay = transform.getRot().inverse() * Vector3(1, 2, 2);
        if(lightRay[1] == 0)
            lightRay[1] = 1e-5;
        lightRay = -lightRay / lightRay[1];
            
        glDisable(GL_LIGHTING);
        glColor3f(0.1f, 0.1f, 0.1f);
        glPushMatrix();
        float matr[16] = {1,0,0,0, (float)lightRay[0],0,(float)lightRay[2],0, 0,0,1,0, 0,0.01f,0,1};
        glMultMatrixf(matr);
        glDepthMask(0);

		if (drawh)	{
	        for(i = 0; i < (int)ms.size(); ++i)
            drawMesh(*(ms[i]), flatShading);   
		}
		//drawSphere(spheres, flatShading);

        glDepthMask(1);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }

    static GLfloat colr[4] = {1.f, .9f, .75f, 1.0f };
    static GLfloat colrb[4] = {1.f, .9f, .75f, 1.0f };
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colr);
    glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, colrb);

    //draw meshes
	if (drawh)	{
		for(i = 0; i < (int)meshes.size(); ++i) {
			drawMesh(*(ms[i]), flatShading);
		}
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	if (drawsph) {
		drawSphere(spheres, flatShading);
	}

	if (drawline)	{
		for(i = 0; i < (int)extractlines.size(); ++i) {
			glColor3d(extractlines[i].color[0], extractlines[i].color[1], extractlines[i].color[2]);
			glLineWidth((float)extractlines[i].thickness);
			glBegin(GL_LINES);
			glVertex3d(extractlines[i].p1[0], extractlines[i].p1[1], extractlines[i].p1[2]);
			glVertex3d(extractlines[i].p2[0], extractlines[i].p2[1], extractlines[i].p2[2]);
			glEnd();
		}
		glColor3f(0.5804, 0, 0.8275);
		for (i = 0; i < (int)limbVerts.size(); ++i) {
			glPushMatrix();
			glTranslatef(limbVerts[i][0], limbVerts[i][1], limbVerts[i][2]);
			glutSolidSphere(0.005, 16, 2); 
			glPopMatrix();
		}

		glColor3f(0.7529, 1.0000, 0.2431);
		for (i = 0; i < (int)EmbeddingV.size(); ++i) {
			glPushMatrix();
			glTranslatef(EmbeddingV[i][0], EmbeddingV[i][1], EmbeddingV[i][2]);
			glutSolidSphere(0.005, 16, 2); 
			glPopMatrix();
		}

		//glColor3f(0.4000, 0.8039, 0);
		if (drawbefore)	{
			glColor3f(1.0, 0.0, 0.0);
			for (i = 0; i < (int)discreteEmbedding.size(); ++i) {
				glPushMatrix();
				glTranslatef(discreteEmbedding[i][0], discreteEmbedding[i][1], discreteEmbedding[i][2]);
				glutSolidSphere(0.005, 16, 3); 
				glPopMatrix();
			}
		}
	}
	
    //draw lines
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_LIGHTING);
	if (drawskl) {
		for(i = 0; i < (int)lines.size(); ++i) {
			glColor3d(lines[i].color[0], lines[i].color[1], lines[i].color[2]);
			glLineWidth((float)lines[i].thickness);
			glBegin(GL_LINES);
			glVertex3d(lines[i].p1[0], lines[i].p1[1], lines[i].p1[2]);
			glVertex3d(lines[i].p2[0], lines[i].p2[1], lines[i].p2[2]);
			glEnd();
		}

		int j;
		// draw match joints
		//glColor3f(0.f, 0.f, 1.f);
		for (i = 0; i < (int)matchPoint.size(); ++i) {
			glPushMatrix();
			glTranslatef(matchPoint[i][0], matchPoint[i][1], matchPoint[i][2]);
			for (j = 0; j < (int)cPoints.size(); j++)
			{
				if (i == cPoints[j])
					break;
			}
			if (j == (int)cPoints.size())
				glColor3f(0.f, 0.f, 1.f);
			else
				glColor3f(1.f, 0.f, 0.f);
			
			glutSolidSphere(0.01, 16, 2); 
			glPopMatrix();
		}
	}

	for(i = 0; i < (int)originLines.size(); ++i) {
		glColor3d(originLines[i].color[0], originLines[i].color[1], originLines[i].color[2]);
		glLineWidth((float)originLines[i].thickness);
		glBegin(GL_LINES);
		glVertex3d(originLines[i].p1[0], originLines[i].p1[1], originLines[i].p1[2]);
		glVertex3d(originLines[i].p2[0], originLines[i].p2[1], originLines[i].p2[2]);
		glEnd();
	}

	// draw origin joints
	glColor3f(0.f, 0.f, 1.f);
	for (i = 0; i < (int)originPoints.size(); ++i) {
		glPushMatrix();
		glTranslatef(originPoints[i][0], originPoints[i][1], originPoints[i][2]);
		glutSolidSphere(0.01, 16, 2); 
		glPopMatrix();
	}

    if(skeleton) {
        glLineWidth(5);
        for(i = 0; i < (int)meshes.size(); ++i) {
			// 这里是绘制运动时骨骼的信息的
			// getSkel是得到运动时各个关节点的坐标
            vector<Vector3> v = meshes[i]->getSkel();
            if(v.size() == 0)
                continue;
            glColor3d(.5, 0, 0);

            const vector<int> &prev = human.fPrev();
            glBegin(GL_LINES);
            for(int j = 1; j < (int)prev.size(); ++j) {
                int k = prev[j];
                glVertex3d(v[j][0], v[j][1], v[j][2]);
                glVertex3d(v[k][0], v[k][1], v[k][2]);
            }
            glEnd();
        }
    }
}

void MyWindow::resetTransform()
{
    transform = Transform<>(Quaternion<>(Vector3(1, 0, 0), .2), 1.5, Vector3(-0.66, -0.66, 0));
}

void MyWindow::initGL()
{
    static GLfloat pos[4] = {5.0, 5.0, 10.0, 1.0 };

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glLightfv( GL_LIGHT0, GL_POSITION, pos );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_NORMALIZE);
    glDisable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_FLAT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
}

void MyWindow::drawSphere(vector<Sphere> spheres, bool flatShading)
{
	glColor3f(0.5f, 0.0f, 0.0f);
	vector<Sphere>::iterator iter;
	for (iter = spheres.begin(); iter != spheres.end(); ++iter)
	{
		glPushMatrix();
		//glLoadIdentity();
		glTranslatef(iter->center[0], iter->center[1], iter->center[2]);
		glutWireSphere(iter->radius, 16, 2);
		glPopMatrix();
	}
	
}

void MyWindow::drawMesh(const Mesh &m, bool flatShading, Vector3 trans)
{
    int i;
    Vector3 normal;
	
	if (drawTri)
	{
		glBegin(GL_TRIANGLES);
		for(i = 0; i < (int)m.edges.size(); ++i) {
			int v = m.edges[i].vertex;
			const Vector3 &p = m.vertices[v].pos;

			if(!flatShading) {
				normal = m.vertices[v].normal;
				glNormal3d(normal[0], normal[1], normal[2]);
			}
			else if(i % 3 == 0) {
				const Vector3 &p2 = m.vertices[m.edges[i + 1].vertex].pos;
				const Vector3 &p3 = m.vertices[m.edges[i + 2].vertex].pos;

				normal = ((p2 - p) % (p3 - p)).normalize();
				glNormal3d(normal[0], normal[1], normal[2]);
			}

			glVertex3d(p[0] + trans[0], p[1] + trans[1], p[2] + trans[2]);
		}
		glEnd();
	}
	else
	{
		//glColor3f(1.f, 1.f, 1.f);
		glBegin(GL_LINES);
		for(i = 0; i < (int)m.edges.size(); i+=3) {
			int v1 = m.edges[i].vertex;
			int v2 = m.edges[i+1].vertex;
			int v3 = m.edges[i+2].vertex;
			const Vector3 &p1 = m.vertices[v1].pos;
			const Vector3 &p2 = m.vertices[v2].pos;
			const Vector3 &p3 = m.vertices[v3].pos;
			Vector3 normal1 = m.vertices[v1].normal;
			Vector3 normal2 = m.vertices[v2].normal;
			Vector3 normal3 = m.vertices[v3].normal;
			
			glNormal3d(normal1[0], normal1[1], normal1[2]);
			glVertex3d(p1[0] + trans[0], p1[1] + trans[1], p1[2] + trans[2]);
			glNormal3d(normal2[0], normal2[1], normal2[2]);
			glVertex3d(p2[0] + trans[0], p2[1] + trans[1], p2[2] + trans[2]);
			glNormal3d(normal2[0], normal2[1], normal2[2]);
			glVertex3d(p2[0] + trans[0], p2[1] + trans[1], p2[2] + trans[2]);
			glNormal3d(normal3[0], normal3[1], normal3[2]);
			glVertex3d(p3[0] + trans[0], p3[1] + trans[1], p3[2] + trans[2]);
			glNormal3d(normal3[0], normal3[1], normal3[2]);
			glVertex3d(p3[0] + trans[0], p3[1] + trans[1], p3[2] + trans[2]);
			glNormal3d(normal1[0], normal1[1], normal1[2]);
			glVertex3d(p1[0] + trans[0], p1[1] + trans[1], p1[2] + trans[2]);
		}
		glEnd();
	}
}

void MyWindow::drawFloor()
{
    int i;
    Mesh floor;
    floor.vertices.resize(4);
    for(i = 0; i < 4; ++i) {
        floor.vertices[i].normal = Vector3(0, 1, 0);
        floor.vertices[i].pos = 10. * Vector3(((i + 0) % 4) / 2, 0, ((i + 1) % 4) / 2) - Vector3(4.5, 0, 4.5);
    }
    floor.edges.resize(6);
    for(i = 0; i < 6; ++i)
        floor.edges[i].vertex = (i % 3) + ((i > 3) ? 1 : 0);

    static GLfloat colrb[4] = {0.5f, .9f, .75f, 1.0f };
    static GLfloat colr[4] = {0.5f, .6f, .9f, 1.0f };
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colr);
    glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, colrb);

    glShadeModel(GL_SMOOTH);
	bool drawb = drawTri;
	drawTri = true;
    drawMesh(floor, false);
	drawTri = drawb;
    glShadeModel( flatShading ? GL_FLAT : GL_SMOOTH);

    glColor4d(.5, .6, .9, .3);
    glLineWidth(1.);
    int gridSize = 20;
    double y = floor.vertices[0].pos[1];
    double minX = floor.vertices[1].pos[0];
    double maxX = floor.vertices[2].pos[0];
    double minZ = floor.vertices[1].pos[2];
    double maxZ = floor.vertices[3].pos[2];
    double stepX = (maxX - minX) / double(gridSize);
    double stepZ = (maxZ - minZ) / double(gridSize);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    for(i = 0; i <= gridSize; ++i) {
        glVertex3d(minX + i * stepX, y, minZ);
        glVertex3d(minX + i * stepX, y, maxZ);
        glVertex3d(minX, y, minZ + i * stepZ);
        glVertex3d(maxX, y, minZ + i * stepZ);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
