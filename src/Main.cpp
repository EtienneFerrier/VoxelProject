// -------------------------------------------
// gMini : a minimal OpenGL/GLUT application
// for 3D graphics.
// Copyright (C) 2006-2013 Tamy Boubekeur
// All rights reserved.
// -------------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include <GL/glut.h>
#include "Vec3.h"
#include "Camera.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Mesh.h"
#include "VoxelGrid.h"
#include "VoxelDAG.h"
#include "Octree.h"
#include "Node.h"

#define ORIGINAL_MESH 0
#define BSH_TREE 1
#define VOXEL_MESH 2

// -------------------------------------------
// OpenGL/GLUT application code.
// -------------------------------------------

static GLint window;
static unsigned int SCREENWIDTH = 800;
static unsigned int SCREENHEIGHT = 600;
static Camera camera;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static unsigned int FPS = 0;
static bool fullScreen = false;
static bool wireframe = false;
static int BSHdisplayLevel = 0;
static int maxBSHlevel = 0;
static int displayMode = ORIGINAL_MESH;

Mesh mesh;
Mesh sphereMesh;
Mesh voxelMesh;
Octree tree;
VoxelGrid voxGrid(64); // Modifier la resolution de la grille ici
VoxelDAG dag;

using namespace std;

void printUsage () {
	std::cerr << std::endl 
        /* << "gMini: a minimal OpenGL/GLUT application" << std::endl
         << "for 3D graphics." << std::endl 
         << "Author : Tamy Boubekeur" << std::endl << std::endl
         << "Usage : ./gmini [<file.off>]" << std::endl
         << "Keyboard commands" << std::endl 
         << "------------------" << std::endl
         << " ?: Print help" << std::endl
         << " w: Toggles Wireframe Mode" << std::endl
         << " g: Toggles GPU Shaders" << std::endl 
         << " f: Toggles full screen mode" << std::endl 
         << " <drag>+<left button>: rotate model" << std::endl 
         << " <drag>+<right button>: move model" << std::endl
         << " <drag>+<middle button>: zoom" << std::endl
		 << " q, <esc>: Quit" << std::endl << std::endl*/
	     << " 1 : Original mesh display" << std::endl
		 << " 2 : BSH tree display" << std::endl
         << " 3 : Voxel mesh display" << std::endl
         << " c : Clear highlights (voxel)" << std::endl
         << " o : Highlight octree parts" << std::endl
         << " d : Highlight DAG parts" << std::endl
         << " p : Increase BSH level" << std::endl
		 << " m : Decrease BSH level" << std::endl << std::endl;
}

void usage () {
    printUsage ();
    exit (EXIT_FAILURE);
}

// init. OpenGL lights. Obsolete in recent GL versions, use shaders variables instead.
void initLight () {
    GLfloat light_position[4] = {3.0f, 3.0f, 3.0f, 0.0f};
    GLfloat direction[3] = {-1.0f,-1.0f,-1.0f};
    GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv (GL_LIGHT1, GL_POSITION, light_position);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color);
    
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
    //glDisable (GL_LIGHTING);
}

void init (const char * modelFilename) {
#ifdef GPU_SHADERS
    glewInit();
    if (glewGetExtension ("GL_ARB_vertex_shader")        != GL_TRUE ||
        glewGetExtension ("GL_ARB_shader_objects")       != GL_TRUE ||
        glewGetExtension ("GL_ARB_shading_language_100") != GL_TRUE) {
        cerr << "Driver does not support OpenGL Shading Language" << endl;
        exit (EXIT_FAILURE);
    }
#endif
    initLight ();

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
    
    camera.resize (SCREENWIDTH, SCREENHEIGHT);
    mesh.loadOFF (modelFilename);
    
    // Specifies the faces to cull (here the ones pointing away from the camera)
    glCullFace (GL_BACK); 

    // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glEnable (GL_CULL_FACE);
    
    // Specify the depth test for the z-buffer
    glDepthFunc (GL_LESS);
    
    // Enable the z-buffer in the rasterization)
    glEnable (GL_DEPTH_TEST); 

    // Background color
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

    

#ifdef GPU_SHADERS
    try {
        simpleShader = new SimpleShader ();
    } catch (Shader::Exception e) {
        cerr << "Error Loading Shader: " << e.msg () << endl;
        exit (EXIT_FAILURE);
    }
#endif
}

void drawSphere(Vec3f center, float rad)
{
	for (unsigned int i = 0; i < sphereMesh.V.size(); i++)
		sphereMesh.V[i].p = sphereMesh.V[i].p*rad + center;

	for (unsigned int i = 0; i < sphereMesh.T.size(); i++)
		for (unsigned int j = 0; j < 3; j++) {
		const Vertex & v = sphereMesh.V[sphereMesh.T[i].v[j]];

		// Specifies current normal vertex
		glNormal3f(v.n[0], v.n[1], v.n[2]);

		// Emit a vertex (one triangle is emitted each time 3 vertices are emitted)
		glVertex3f(v.p[0], v.p[1], v.p[2]);
		}

	for (unsigned int i = 0; i < sphereMesh.V.size(); i++)
		sphereMesh.V[i].p = (sphereMesh.V[i].p - center) / rad;
}

void draw () {
	switch (displayMode)
	{
		case ORIGINAL_MESH:
			// IMPORTANT NOTE: disable lighting and use glColor3f (r,g,b) to specify per-vertex
			// color or use enable GL_COLOR_MATERIAL to use both vertex color and lighting
			// Starts a list of triangles (OpenGL immediat mode)
			glBegin(GL_TRIANGLES);

			for (unsigned int i = 0; i < mesh.T.size(); i++)
				for (unsigned int j = 0; j < 3; j++) {
				const Vertex & v = mesh.V[mesh.T[i].v[j]];

				// Specifies current normal vertex
				glNormal3f(v.n[0], v.n[1], v.n[2]);

				// Emit a vertex (one triangle is emitted each time 3 vertices are emitted)
				glVertex3f(v.p[0], v.p[1], v.p[2]);
				}

			// Ends the triangle drawing
			glEnd();
			break;

		case BSH_TREE:
			glBegin(GL_TRIANGLES);
			mesh.BSHtree->drawSphereTree(BSHdisplayLevel);
			glEnd();
			break;

		case VOXEL_MESH:
			glBegin(GL_QUADS);
			for (unsigned int i = 0; i < voxelMesh.Q.size(); i++) {
				if (voxelMesh.col[i])
					glColor3f(1.0f, 0.4f, 0.0f);
				else
					glColor3f(1.f, 1.f, 1.f);

				for (unsigned int j = 0; j < 4; j++) {
    				const Vertex & v = voxelMesh.V[voxelMesh.Q[i].v[j]];

    				glNormal3f(v.n[0], v.n[1], v.n[2]);

    				glVertex3f(v.p[0], v.p[1], v.p[2]);
				}
            }
			glEnd();
	}
}

void display () {
    // Loads the identity matrix as the current OpenGL matrix
    glLoadIdentity ();

    // Erase the color and z buffers.
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the modelview matrix and the projection matrix from the camera
    camera.apply (); 
    draw ();

    // Ensures any previous OpenGL call has been executed
    glFlush ();

    // swap the render buffer and the displayed (screen) one
    glutSwapBuffers (); 
}

// This function is executed in an infinite loop. It updated the window title
// (frame-per-second, model size) and ask for rendering
void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [128];
        unsigned int numOfTriangles = mesh.T.size ();
#ifdef _WIN32
        sprintf_s (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
#else
        sprintf (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
#endif
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }

    // calls the display function
    glutPostRedisplay (); 
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen) {
            glutReshapeWindow (SCREENWIDTH, SCREENHEIGHT);
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }      
        break;
#ifdef GPU_SHADERS    
    case 'g':
        if (useGPUShaders) {
            simpleShader->unbind ();
            useGPUShaders = false;
        } else {
            simpleShader->bind ();
            useGPUShaders = true;
        }      
        break;
#endif
    case 'q':
    case 27:
        exit (EXIT_SUCCESS);
        break;
    case 'w':
        glPolygonMode (GL_FRONT_AND_BACK, wireframe ? GL_FILL : GL_LINE);
        wireframe = !wireframe;
        break;
	case 'p' :
		BSHdisplayLevel = min(BSHdisplayLevel + 1, maxBSHlevel);
		cout << "BSH level : " << BSHdisplayLevel << endl;
		break;
	case 'm':
		BSHdisplayLevel = max(BSHdisplayLevel - 1, 0);
        cout << "BSH level : " << BSHdisplayLevel << endl;
		break;
	case '1':
		displayMode = ORIGINAL_MESH;
		break;
	case '2':
		displayMode = BSH_TREE;
		break;
	case '3':
		displayMode = VOXEL_MESH;
		break;
	case 'o':
		voxGrid.colorSubOctree();
		voxGrid.convertToMesh(voxelMesh); // Non efficace mais instantané
		break;
    case 'd':
        if (!dag.isEmpty()) {
            voxGrid.clearColor();
            dag.toVoxelGrid(voxGrid, 0, 0, 0, 0, 0, true);
            voxGrid.convertToMesh(voxelMesh); // Non efficace mais instantané
        }
        break;
	case 'c':
		voxGrid.clearColor();
		voxGrid.convertToMesh(voxelMesh);
		break;
    default:
        printUsage ();
		//cout << keyPressed << endl;
        break;
    }
    idle ();
}

void mouse (int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        mouseMovePressed = false;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    } else {
        if (button == GLUT_LEFT_BUTTON) {
            camera.beginRotate (x, y);
            mouseMovePressed = false;
            mouseRotatePressed = true;
            mouseZoomPressed = false;
        } else if (button == GLUT_RIGHT_BUTTON) {
            lastX = x;
            lastY = y;
            mouseMovePressed = true;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (mouseZoomPressed == false) {
                lastZoom = y;
                mouseMovePressed = false;
                mouseRotatePressed = false;
                mouseZoomPressed = true;
            }
        }
    }
    idle ();
}

void motion (int x, int y) {
    if (mouseRotatePressed == true) {
        camera.rotate (x, y);
    }
    else if (mouseMovePressed == true) {
        camera.move ((x-lastX)/static_cast<float>(SCREENWIDTH), (lastY-y)/static_cast<float>(SCREENHEIGHT), 0.0);
        lastX = x;
        lastY = y;
    }
    else if (mouseZoomPressed == true) {
        camera.zoom (float (y-lastZoom)/SCREENHEIGHT);
        lastZoom = y;
    }
}

void reshape(int w, int h) {
    camera.resize (w, h);
}

int main (int argc, char ** argv) {

    if (argc > 3) {
        printUsage ();
        exit (EXIT_FAILURE);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCREENWIDTH, SCREENHEIGHT);
    window = glutCreateWindow ("Rendering");

    init (argc >= 2 ? argv[1] : "models/rhino.off");

	//========================= Ajouts Sparse Voxel DAG Project ================

	//Mesh loading
	cout << "Mesh loaded : " << mesh.T.size() << " triangles." << endl;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Shere mesh loading only for BSHtree display
	sphereMesh.loadOFF(argc == 3 ? argv[2] : "models/sphere.off");
	cout << "Sphere mesh loaded." << endl;

	// BSHtree computing
	mesh.computeBSH(9);
	maxBSHlevel = mesh.BSHtree->maxCompleteLevel(); // Limite de l'affichage du BSH
	cout << "BSH done." << endl << endl;

	// Mesh to VoxelGrid
    voxGrid.fillSparseGridBSH(mesh);
    voxGrid.emptyInteriorVoxels();
    cout << "Mesh -> VoxelGrid done : size = " << voxGrid.getSize() << endl;

	// VoxelGrid to Octree 
	tree.fillOctreeWithVoxelGrid(voxGrid);
	cout << "VoxelGrid -> Octree done : " << tree.memorySize()/8 << " bytes." << endl;

	// Octree to Breadth First encoding
	vector<uint8_t> storage;
	tree.encodeBreadthFirst(storage);
	cout << "Octree -> Breadth First Encoding done : " << storage.size() << " bytes." << endl;
	tree = Octree();

	// (testing purposes) Breadth First encoding to Octree
	tree.loadFromBreadthFirst(storage);
	cout << "Breadth First Encoding -> Octree done." << endl;

	// Octree to Efficient Pointer encoding
	vector<uint8_t> masks;
    vector<int> pointers;
	tree.encodeWithPointers(masks, pointers);
	cout << "Octree -> Pointer Encoding done : " << masks.size() + 4*pointers.size() << " bytes." << endl;

	// (testing purposes) Efficient Pointer encoding to Octree
	tree = Octree();
	tree.loadFromPointerEncoding(masks, pointers);
	cout << "Pointer Encoding -> Octree done" << endl;

    // Octree to DAG
    dag.buildDAG(tree);
    cout << "Octree -> DAG done : " << dag.getSize() << " bytes." << endl;

	// (testing & display purposes) Octree to VoxelGrid
	tree.convertOctreeToVoxelGrid(voxGrid);
	cout << "Octree -> VoxelGrid done" << endl;

    // (testing & display purposes) DAG to VoxelGrid
    voxGrid.setAllGrid(false);
    dag.toVoxelGrid(voxGrid);
    cout << "DAG -> VoxelGrid done" << endl;

	// (testing & display purposes) VoxelGrid to Mesh
	voxGrid.convertToMesh(voxelMesh);
	cout << "VoxelGrid -> Mesh done : " << voxelMesh.Q.size() << " quads" << endl;

	//========================= Fin des ajouts =========================

    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    key ('?', 0, 0);   
    glutMainLoop ();
    return EXIT_SUCCESS;
}

