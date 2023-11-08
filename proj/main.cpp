#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "MicroGlut.h"
#define MAIN
#include "VectorUtils4.h"
#include "LittleOBJLoaderX.h"
#include "GL_utilities.h"
// uses framework Cocoa
// uses framework OpenGL

// initial width and heights
#define W 512
#define H 512

#define NUM_LIGHTS 4

vec3 sphere_to_rect(float theta, float phi, float r, vec3 start) {
    float x_end = r * sin(theta) * cos(phi);
    float y_end = r * sin(theta) * sin(phi);
    float z_end = r * cos(theta);
    return {start.x + x_end, start.y + y_end, start.z + z_end};
}

vec3 rand_start() {
    return {float(std::rand() % 100), 20.0, float(std::rand() % 100)};
}

float find_angle(vec3 v1, vec3 v2) {
    float dotprod = dot(v1, v2);
    float l1 = Norm(v1);
    float l2 = Norm(v2);
    float angl = acos(dotprod / (l1 * l2));
    return angl;
}

struct lightning_seg {
    int no;
    vec3 start;
    vec3 end;
    float width;
    struct lightning_seg *parent;
    std::vector<struct lightning_seg*> children;
    std::vector<vec3> lights;
};

float get_theta(vec3 v) {
    return acos(v.z / Norm(v));
}

float get_phi(vec3 v) {
    if(v.y < 0)
        return -acos(v.x / sqrt(v.x * v.x + v.y * v.y));
    return acos(v.x / sqrt(v.x * v.x + v.y * v.y));
}

void generate_lightning(vec3 start, vec3 end, lightning_seg *lightning, vec3 tot_start) {
    std::cout << "bolt seg: " << lightning->no << std::endl;
    lightning->start = start;
    float length_r = (std::rand() % 100) / 10.0;
    std::cout << "bolt len: " << length_r << std::endl;
    if (length_r >= Norm(end - start)) {
        lightning->end = end;
        return;
    }

    float end_theta = get_theta(end - start);
    float end_phi = get_phi(end - start);

    float progress = Norm(end - start) / Norm(end - tot_start);

    float angle_theta = end_theta + progress * ((std::rand() % 60 - 30) * M_PI / 180.0);
    std::cout << "bolt theta: " << angle_theta << std::endl;
    float angle_phi = end_phi + progress * ((std::rand() % 60 - 30) * M_PI / 180.0);
    std::cout << "bolt phi: " << angle_phi << std::endl;
    vec3 seg_end = sphere_to_rect(angle_theta, angle_phi, length_r, start);
    lightning->end = seg_end;

    lightning_seg* main_child = new lightning_seg;
    main_child->width = lightning->width * 0.99;
    main_child->parent = lightning;
    main_child->no = lightning->no + 1;

    generate_lightning(lightning->end, end, main_child, tot_start);
    // if(std::rand() % 100 < 20.0 * (Norm(end - lightning->end))) { // TODO: adjust distance dependency
    //     lightning_seg* secondary_child = new lightning_seg;
    //     secondary_child->width = lightning->width * 0.95;
    //     secondary_child->parent = lightning;
    //     float end_r = (std::rand() % 300) / 10.0;
    //     float end_theta = (std::rand() % 60 - 30) * M_PI / 180.0;
    //     float end_phi = (std::rand() % 60 - 30) * M_PI / 180.0;

    //     generate_lightning(lightning->end, sphere_to_rect(end_theta, end_phi, end_r, lightning->end), secondary_child);
    // }
}

void delete_lightning(lightning_seg *l) {
    if(!l->children.empty()) {
        for(auto c : l->children) {
            delete_lightning(c);
        }
    }
    delete l;
    return;
}

void draw_seg(lightning_seg *l) {
    float ver[3][3] = { // start with polygon
        {l->start.x, l->start.y, l->start.z},
        {l->start.x + l->width, l->start.y, l->start.z},
        {l->end.x, l->end.y, l->end.z}
    };
}

mat4 projectionMatrix;
mat4 viewMatrix, modelToWorldMatrix;


GLfloat square[] = {
							-1,-1,0,
							-1,1, 0,
							1,1, 0,
							1,-1, 0};
GLfloat squareTexCoord[] = {
							 0, 0,
							 0, 1,
							 1, 1,
							 1, 0};
GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

Model* squareModel;

//----------------------Globals-------------------------------------------------
Model *model1;
FBOstruct *fbo1, *fbo2, *fbo3;
GLuint phongshader = 0, plaintextureshader = 0, lowpassshader = 0, truncateshader = 0, addfiltershader = 0;

void runfilter(GLuint shader, FBOstruct *in1, FBOstruct *in2, FBOstruct *out)
{
    glUseProgram(shader);

    // Many of these things would be more efficiently done once and for all

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glUniform1i(glGetUniformLocation(shader, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(shader, "texUnit2"), 1);

    useFBO(out, in1, in2);

    DrawModel(squareModel, shader, "in_Position", NULL, "in_TexCoord");

    glFlush();
}

//-------------------------------------------------------------------------------------

void init(void)
{
    lightning_seg *sl = new lightning_seg;
    sl->no = 0;

    vec3 start = rand_start();
    vec3 end = start;
    end.y = 0.0;
    generate_lightning(start, end, sl, start);

	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	// Load and compile shaders
	plaintextureshader = loadShaders("plaintextureshader.vert", "plaintextureshader.frag");  // puts texture on teapot
	phongshader = loadShaders("phong.vert", "phong.frag");  // renders with light (used for initial renderin of teapot)
	lowpassshader = loadShaders("plaintextureshader.vert", "lowpass.frag");
	truncateshader = loadShaders("plaintextureshader.vert", "truncate.frag");
	addfiltershader = loadShaders("plaintextureshader.vert", "addfilter.frag");

	printError("init shader");

	fbo1 = initFBO(W, H, 0);
	fbo2 = initFBO(W, H, 0);
	fbo3 = initFBO(W, H, 0);

	// load the model
	model1 = LoadModelPlus("stanford-bunny.obj");

	squareModel = LoadDataToModel(
			(vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL,
			squareIndices, 4, 6);

	vec3 cam = vec3(0, 5, 15);
	vec3 point = vec3(0, 1, 0);
	vec3 up = vec3(0, 1, 0);
	viewMatrix = lookAtv(cam, point, up);
	modelToWorldMatrix = IdentityMatrix();
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	mat4 vm2;

	// This function is called whenever it is time to render
	//  a new frame; due to the idle()-function below, this
	//  function will get called several times per second

	// render to fbo1!
	useFBO(fbo3, 0L, 0L);

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
	glUseProgram(phongshader);

	vm2 = viewMatrix * modelToWorldMatrix;
	// Scale and place bunny since it is too small
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * S(80,80,80);

	glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);


	glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	DrawModel(model1, phongshader, "in_Position", "in_Normal", NULL);

	runfilter(truncateshader, fbo3, 0L, fbo2);

	int count = 100;
	for (int i = 0; i < count; i++) {
		runfilter(lowpassshader, fbo2, 0L, fbo1);
		runfilter(lowpassshader, fbo1, 0L, fbo2);
	}

	// Done rendering the FBO! Set up for rendering on screen, using the result as texture!

	useFBO(0L, fbo2, fbo3);
	//useFBO(0L, fbo2, 0L);
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate second shader program
	// glUseProgram(plaintextureshader);
	glUseProgram(addfiltershader);

	glUniform1i(glGetUniformLocation(addfiltershader, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(addfiltershader, "texUnit2"), 1);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(squareModel, addfiltershader, "in_Position", NULL, "in_TexCoord");
	// DrawModel(squareModel, plaintextureshader, "in_Position", NULL, "in_TexCoord");

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	projectionMatrix = perspective(90, ratio, 1.0, 1000);
}

// Trackball

int prevx = 0, prevy = 0;

void mouseUpDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		prevx = x;
		prevy = y;
	}
}

void mouseDragged(int x, int y)
{
	vec3 p;
	mat4 m;

	// This is a simple and IMHO really nice trackball system:

	// Use the movement direction to create an orthogonal rotation axis

	p.y = x - prevx;
	p.x = -(prevy - y);
	p.z = 0;

	// Create a rotation around this axis and premultiply it on the model-to-world matrix
	// Limited to fixed camera! Will be wrong if the camera is moved!

	m = ArbRotate(p, sqrt(p.x*p.x + p.y*p.y) / 50.0); // Rotation in view coordinates
	modelToWorldMatrix = Mult(m, modelToWorldMatrix);

	prevx = x;
	prevy = y;

	glutPostRedisplay();
}


//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(W, H);

	glutInitContextVersion(3, 2);
	glutCreateWindow ("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseUpDown);
	glutMotionFunc(mouseDragged);
	glutRepeatingTimer(50);

	init();
	glutMainLoop();
	exit(0);
}
