#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "Linux/MicroGlut.h"
#define MAIN
#include "VectorUtils4.h"
// #include "VectorUtils3.h"
#include "LittleOBJLoaderX.h"
#include "GL_utilities.h"
// uses framework Cocoa
// uses framework OpenGL

// initial width and heights
#define W 512
#define H 512

float scale = 0.08;

struct lightning_seg {
    int bd;
    vec3 start;
    vec3 end;
    float width;
    struct lightning_seg *parent;
    std::vector<struct lightning_seg*> children;
    vec3 light;
	Model *m;
	bool last = false;
};

lightning_seg *sl;

float COS0 = cos(0);
float SIN0 = sin(0);
float COS2PIDIV3 = cos(2*M_PI/3);
float SIN2PIDIV3 = sin(2*M_PI/3);
float COS4PIDIV3 = cos(4*M_PI/3);
float SIN4PIDIV3 = sin(4*M_PI/3);

vec3 sphere_to_rect(float theta, float phi, float r, vec3 start) {
    float x_end = r * sin(theta) * cos(phi);
    float y_end = r * sin(theta) * sin(phi);
    float z_end = r * cos(theta);
    return {start.x + x_end, start.y + y_end, start.z + z_end};
}

vec3 rand_point(float y) {
    return {float(std::rand() % 1000 / 250) - 2.0, y, float(std::rand() % 1000 / 250) - 2.0};
}

float get_theta(vec3 v) {
    return acos(v.z / Norm(v));
}

float get_phi(vec3 v) {
    if(v.y < 0)
        return -acos(v.x / sqrt(v.x * v.x + v.y * v.y));
    return acos(v.x / sqrt(v.x * v.x + v.y * v.y));
}

Model* gen_seg_model(lightning_seg *l) {
	float top_width = l->width;
	float bot_width = 0;
	if(!l->children.empty()) {
		bot_width = l->children[0]->width;
		GLfloat ver[] = {
			(GLfloat)(l->start.x + l->width * COS0) * scale, (GLfloat)l->start.y * scale, (GLfloat)(l->start.z + l->width * SIN0) * scale, // 0
			(GLfloat)(l->start.x + l->width * COS2PIDIV3) * scale, (GLfloat)l->start.y * scale, (GLfloat)(l->start.z + l->width * SIN2PIDIV3) * scale, // 1
			(GLfloat)(l->start.x + l->width * COS4PIDIV3) * scale, (GLfloat)l->start.y * scale, (GLfloat)(l->start.z + l->width * SIN4PIDIV3) * scale, // 2
			(GLfloat)(l->end.x + bot_width * COS0) * scale, (GLfloat)l->end.y * scale, (GLfloat)(l->end.z + bot_width * SIN0) * scale, // 3
			(GLfloat)(l->end.x + bot_width * COS2PIDIV3) * scale, (GLfloat)l->end.y * scale, (GLfloat)(l->end.z + bot_width * SIN2PIDIV3) * scale, // 4
			(GLfloat)(l->end.x + bot_width * COS4PIDIV3) * scale, (GLfloat)l->end.y * scale, (GLfloat)(l->end.z + bot_width * SIN4PIDIV3) * scale, // 5
		};

		GLuint ind[] = {3,2,0,
						3,5,2,
						5,1,2,
						5,4,1,
						4,0,1,
						4,3,0};

		return LoadDataToModel(
				(vec3 *)ver, NULL, NULL, NULL,
				ind, 6, 18);
	} else {
		GLfloat ver[] = {
			(GLfloat)(l->start.x + l->width * COS0) * scale, (GLfloat)l->start.y * scale, (GLfloat)(l->start.z + l->width * SIN0) * scale, // 0
			(GLfloat)(l->start.x + l->width * COS2PIDIV3) * scale, (GLfloat)l->start.y * scale, (GLfloat)(l->start.z + l->width * SIN2PIDIV3) * scale, // 1
			(GLfloat)(l->start.x + l->width * COS4PIDIV3) * scale, (GLfloat)l->start.y * scale, (GLfloat)(l->start.z + l->width * SIN4PIDIV3) * scale, // 2
			(GLfloat)l->end.x * scale, (GLfloat)l->end.y * scale, (GLfloat)l->end.z * scale // 3
		};

		GLuint ind[] = {3,2,0,
						3,1,2,
						3,0,1};

		return LoadDataToModel(
				(vec3 *)ver, NULL, NULL, NULL,
				ind, 4, 9);
	}

}


void generate_lightning(vec3 start, vec3 end, lightning_seg *lightning, vec3 tot_start, vec3 main_end) {
    lightning->start = start;
    float length_r = (std::rand() % 2000) / 4000.0 + 0.5;
    if (length_r >= Norm(end - start)) {
        lightning->end = end;
    	lightning->light = normalize(lightning->end) * Norm(end - start) / 2 * scale;
		lightning->m = gen_seg_model(lightning);
		if(end.x == main_end.x && end.y == main_end.y && end.z == main_end.z)
			lightning->last = true;
        return;
    }

    float end_theta = get_theta(end - start);
    float end_phi = get_phi(end - start);

    float progress = Norm(end - start) / Norm(end - tot_start);

    float angle_theta = end_theta + (1 - (1-progress) * (1-progress)) * ((std::rand() % 160 - 80) * M_PI / 180.0);
    float angle_phi = end_phi + (1 - (1-progress) * (1-progress)) * ((std::rand() % 160 - 80) * M_PI / 180.0);
    lightning->end = sphere_to_rect(angle_theta, angle_phi, length_r, start);

    lightning->light = sphere_to_rect(angle_theta, angle_phi, length_r / 2, start) * scale;

    lightning_seg* main_child = new lightning_seg;
    main_child->width = lightning->width * 0.95;
    main_child->parent = lightning;
    main_child->bd = lightning->bd;
    main_child->light = lightning->light;

    generate_lightning(lightning->end, end, main_child, tot_start, main_end);
	lightning->children.push_back(main_child);
	lightning->m = gen_seg_model(lightning);

    if(std::rand() % 100 < 30.0 * progress) {
        lightning_seg* secondary_child = new lightning_seg;
        secondary_child->width = lightning->width * 0.50;
        secondary_child->parent = lightning;
        secondary_child->bd = lightning->bd + 1;
    	secondary_child->light = lightning->light;

        float child_end_r = (std::rand() % 7000) / 1000.0 * exp(-secondary_child->bd);
        float child_theta = end_theta + (std::rand() % 100 - 50) * M_PI / 180.0;
        float child_phi = end_phi + (std::rand() % 100 - 50) * M_PI / 180.0;
		vec3 child_end = sphere_to_rect(child_theta, child_phi, child_end_r, lightning->end);
		child_end.y = std::max(child_end.y, main_end.y);

        generate_lightning(lightning->end, child_end, secondary_child, tot_start, main_end);
		lightning->children.push_back(secondary_child);
    }
}

void delete_lightning(lightning_seg *l) {
    if(!l->children.empty()) {
        for(auto c : l->children) {
            delete_lightning(c);
			l->children.clear();
        }
    }
    delete l;
    return;
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

GLfloat floor_[] = {
							-1000,-5 * scale,-1000,
							-1000,-5 * scale, 1000,
							1000,-5 * scale, 1000,
							1000,-5 * scale, -1000};
GLfloat floor_Normals[] = {
							0,1,0,
							0,1,0,
							0,1,0,
							0,1,0};
GLuint floor_Indices[] = {0, 1, 2,
						 0, 2, 3};

Model* floor_Model;

//----------------------Globals-------------------------------------------------
Model *model1;
FBOstruct *fbo1, *fbo2, *fbo3;
GLuint depthshader = 0, normalshader = 0, ssaoshader = 0, phongshader = 0, litshader = 0, plaintextureshader = 0, lowpassshader = 0, truncateshader = 0, addfiltershader = 0, multfiltershader = 0;

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

GLfloat prevt = 0;

vec3 lights[70 * 3];
int num_lights = 0;

vec3 samples[64];

int mul = 15;
int modu = 700;

bool ANIMATE = false;

void draw_bolt(lightning_seg *start, GLfloat t, int d) {
	if((int)t % modu <= 50 && (int)prevt % modu > (int)t % modu && ANIMATE) {
		delete_lightning(sl);
		sl = new lightning_seg;

		sl->bd = 0;
		sl->width = 0.25;
		vec3 st = rand_point(5.0);
		vec3 en = rand_point(-5.0);
		generate_lightning(st, en, sl, st, en);

		num_lights = 0;
		prevt = t;
		return;
	}

	DrawModel(start->m, litshader, "in_Position", NULL, NULL);
	lights[num_lights] = start->light;
	num_lights++;
	lights[num_lights] = start->start * scale;
	num_lights++;
	lights[num_lights] = start->end * scale;
	num_lights++;

	if((int)t % modu > d * mul || !ANIMATE) {
		for(size_t i = 0; i < start->children.size(); i++) {
			if(start->children.size() == 0) {
				std::cout << "oh no" << std::endl;
				return;
			}
			draw_bolt(start->children[i], t, d + 1);
		}
	}

	prevt = t;
}

float rand_float() {
	int ran = std::rand();
	return (float(ran % 10000) / 10000);
}

void gen_samples() {
	for(int i = 0; i < 64; ++i) {
		vec3 sample = vec3(rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0, rand_float());
		sample = normalize(sample);
		sample *= rand_float();
		sample *= 0.0001;

		// float scale = float(i) / 64.0;

		
		// scale = 0.1 + scale * scale * (1.0 - 0.1);
		// sample *= scale;
		samples[i] = sample;
		printVec3(sample);
	}
}

//-------------------------------------------------------------------------------------

void init(void)
{
	std::cout << "enter seed: ";
	int seed;
	std::cin >> seed;
	std::srand(seed);

	sl = new lightning_seg;

    sl->bd = 0;
    sl->width = 0.25;
    vec3 st = rand_point(5.0);
    vec3 en = rand_point(-5.0);
    generate_lightning(st, en, sl, st, en);

	gen_samples();

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
	ssaoshader = loadShaders("phong.vert", "ssao.frag");  // renders with light (used for initial renderin of teapot)
	depthshader = loadShaders("phong.vert", "depth.frag");  // renders with light (used for initial renderin of teapot)
	normalshader = loadShaders("phong.vert", "normal.frag");  // renders with light (used for initial renderin of teapot)
	litshader = loadShaders("phong.vert", "lit.frag");  // renders with light (used for initial renderin of teapot)
	lowpassshader = loadShaders("plaintextureshader.vert", "lowpass.frag");
	truncateshader = loadShaders("plaintextureshader.vert", "truncate.frag");
	addfiltershader = loadShaders("plaintextureshader.vert", "addfilter.frag");
	addfiltershader = loadShaders("plaintextureshader.vert", "multfilter.frag");

	printError("init shader");

	fbo1 = initFBO(W, H, 0);
	fbo2 = initFBO(W, H, 0);
	fbo3 = initFBO(W, H, 0);

	// load the model
	model1 = LoadModelPlus("stanford-bunny.obj");

	squareModel = LoadDataToModel(
			(vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL,
			squareIndices, 4, 6);

	floor_Model = LoadDataToModel(
			(vec3 *)floor_, (vec3 *)floor_Normals, NULL, NULL,
			floor_Indices, 4, 6);

	vec3 cam = vec3(0.0, 0.0, 70.0);
	vec3 point = vec3(0.0, 0.0, 0.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
	viewMatrix = lookAtv(cam, point, up);
	modelToWorldMatrix = IdentityMatrix();
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	mat4 vm2;
	mat4 vm3;

	GLfloat t = (GLfloat) glutGet(GLUT_ELAPSED_TIME);

	// This function is called whenever it is time to render
	//  a new frame; due to the idle()-function below, this
	//  function will get called several times per second

	useFBO(fbo3, 0L, 0L);

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(depthshader);
	
	vm2 = viewMatrix * modelToWorldMatrix;
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * S(80,80,80);

	glUniformMatrix4fv(glGetUniformLocation(depthshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(depthshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);

	glUniform1i(glGetUniformLocation(depthshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	DrawModel(floor_Model, depthshader, "in_Position", NULL, NULL);
	// DrawModel(floor_Model, depthshader, "in_Position", "in_Normal", NULL);

	vm2 = viewMatrix * modelToWorldMatrix;
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * T(20.0, -35.0, 5.0);
	vm2 = vm2 * S(80,80,80);
	glUniformMatrix4fv(glGetUniformLocation(depthshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	DrawModel(model1, depthshader, "in_Position", NULL, NULL);
	// DrawModel(model1, depthshader, "in_Position", "in_Normal", NULL);

	useFBO(fbo1, 0L, 0L);

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(normalshader);
	
	vm2 = viewMatrix * modelToWorldMatrix;
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * S(80,80,80);

	glUniformMatrix4fv(glGetUniformLocation(normalshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(normalshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);

	glUniform1i(glGetUniformLocation(normalshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	DrawModel(floor_Model, normalshader, "in_Position", NULL, NULL);

	vm2 = viewMatrix * modelToWorldMatrix;
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * T(20.0, -35.0, 5.0);
	vm2 = vm2 * S(80,80,80);
	glUniformMatrix4fv(glGetUniformLocation(normalshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	DrawModel(model1, normalshader, "in_Position", NULL, NULL);

	useFBO(fbo2, fbo3, fbo1);

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(ssaoshader);
	
	glUniform3fv(glGetUniformLocation(phongshader, "samples"), 64 * 3, (GLfloat *)samples);
	
	vm2 = viewMatrix * modelToWorldMatrix;
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * S(80,80,80);

	glUniformMatrix4fv(glGetUniformLocation(ssaoshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(ssaoshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);

	glUniform1i(glGetUniformLocation(ssaoshader, "texUnit"), 0);
 	glUniform1i(glGetUniformLocation(ssaoshader, "texUnit2"), 1);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	DrawModel(floor_Model, ssaoshader, "in_Position", NULL, NULL);
	// DrawModel(floor_Model, ssaoshader, "in_Position", "in_Normal", NULL);

	vm2 = viewMatrix * modelToWorldMatrix;
	vm2 = vm2 * T(0, -8.5, 0);
	vm2 = vm2 * T(20.0, -35.0, 5.0);
	vm2 = vm2 * S(80,80,80);
	glUniformMatrix4fv(glGetUniformLocation(ssaoshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	DrawModel(model1, ssaoshader, "in_Position", NULL, NULL);
	// DrawModel(model1, ssaoshader, "in_Position", "in_Normal", NULL);

	
	// glUseProgram(litshader);

	// vm2 = viewMatrix * modelToWorldMatrix;
	// vm2 = vm2 * T(0, -8.5, 0);
	// vm2 = vm2 * S(80,80,80);

	// glUniformMatrix4fv(glGetUniformLocation(litshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	// glUniformMatrix4fv(glGetUniformLocation(litshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);

	// glUniform1i(glGetUniformLocation(litshader, "texUnit"), 0);

	// // Enable Z-buffering
	// glEnable(GL_DEPTH_TEST);
	// // Enable backface culling
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

	// num_lights = 0;
	// draw_bolt(sl, t, 0);

	// for(int i = 0; i < num_lights; i++) {
	// 	vm3 = viewMatrix * modelToWorldMatrix;
	// 	vm3 = vm3 * T(0, -8.5, 0);
	// 	vm3 = vm3 * S(80,80,80);
	// 	vm3 = vm3 * T(lights[i].x, lights[i].y, lights[i].z);
	// 	lights[i] =  vec3(vm3 * vec4(lights[i], 1.0));
	// 	glUniformMatrix4fv(glGetUniformLocation(litshader, "modelviewMatrix"), 1, GL_TRUE, vm3.m);
	// }

	// // Activate shader program
	// glUseProgram(phongshader);

	// glUniform3fv(glGetUniformLocation(phongshader, "lights"), 70 * 3, (GLfloat *)lights);
	// int tmp = num_lights;
	// glUniform1i(glGetUniformLocation(phongshader, "num_lights"), tmp);

	// glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	// glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);

	// glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

	// // Enable Z-buffering
	// glEnable(GL_DEPTH_TEST);
	// // Enable backface culling
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

	// DrawModel(floor_Model, phongshader, "in_Position", "in_Normal", NULL);

	// vm2 = viewMatrix * modelToWorldMatrix;
	// vm2 = vm2 * T(0, -8.5, 0);
	// vm2 = vm2 * T(20.0, -35.0, 5.0);
	// vm2 = vm2 * S(80,80,80);
	// glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	// DrawModel(model1, phongshader, "in_Position", "in_Normal", NULL);

	// runfilter(truncateshader, fbo3, 0L, fbo2);

	// int count = 100;
	// for (int i = 0; i < count; i++) {
	// 	runfilter(lowpassshader, fbo2, 0L, fbo1);
	// 	runfilter(lowpassshader, fbo1, 0L, fbo2);
	// }

	// // Done rendering the FBO! Set up for rendering on screen, using the result as texture!

	// useFBO(0L, fbo2, fbo3);
	useFBO(0L, fbo2, 0L);
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// // Activate second shader program
	glUseProgram(plaintextureshader);

	glUniform1i(glGetUniformLocation(plaintextureshader, "texUnit"), 0);
 	// glUniform1i(glGetUniformLocation(multfiltershader, "texUnit2"), 1);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(squareModel, plaintextureshader, "in_Position", NULL, "in_TexCoord");

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	projectionMatrix = perspective(90, ratio, 1.0, 10000000);
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
