#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <gl/glut.h>
#include <windows.h>
#include <time.h>
#include <vector>
#include "geo_data.h"
#include "Euler.h"

using namespace std;
solid *S;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;

bool bDepth =true;
bool bCull = false;
float colors[500][3];
int color_idx = 0;

GLUtesselator* obj = gluNewTess();

void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}
void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *estring;
	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}
void CALLBACK endCallback(void)
{
	glEnd();
}
void CALLBACK vertexCallback(GLvoid *vertex)
{
	const GLdouble *pointer;
	pointer = (GLdouble *)vertex;
	glVertex3dv(pointer);
}

void ChangeSize(int w, int h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat nRange = 150.0f;

	if (w <= h)
		glOrtho(-nRange, nRange, -nRange * h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange * w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN) xRot += 5.0f;
	if (key == GLUT_KEY_LEFT) yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT) yRot += 5.0f;
	if (key == GLUT_KEY_PAGE_UP) zRot -= 5.0f;
	if (key == GLUT_KEY_PAGE_DOWN) zRot += 5.0f;

	if (xRot > 356.0f) xRot = 0.0f;
	if (xRot < -1.0f) xRot = 355.0f;
	if (yRot > 356.0f) yRot = 0.0f;
	if (yRot < -1.0f) yRot = 355.0f;
	if (zRot > 356.0f) zRot = 0.0f;
	if (zRot < -1.0f) zRot = 355.0f;

	glutPostRedisplay();
}


void drawLoop(loop* outerLoop) {
	gluTessBeginContour(obj);
	
	halfedge* now_hf = outerLoop->hfedge;
	do {
		vertex *now_v = now_hf->start;
		gluTessVertex(obj, now_v->coord, now_v->coord);
		now_hf = now_hf->next;
	} while (now_hf != outerLoop->hfedge);
	gluTessEndContour(obj);
}

void drawFace(face* currentFace) {

	gluTessBeginPolygon(obj, NULL);
	glColor3f(colors[color_idx][0], colors[color_idx][1], colors[color_idx][2]);

	loop* now_lp = currentFace->lp;
	drawLoop(now_lp);
	now_lp = now_lp->next;
	while (now_lp != nullptr) {
		drawLoop(now_lp);
		now_lp = now_lp->next;
	}

	gluTessEndPolygon(obj);
	color_idx++;
	
}

void drawSolid(solid* mySolid) {
	color_idx = 0;
	face* now_f = mySolid->fc;
	while (now_f != nullptr) {
		drawFace(now_f);
		now_f = now_f->next;
	}
}
void RenderScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//是否开启深度测试
	if (bDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	//是否打开背面剔除
	if (bCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	//旋转图形
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CCW);

	drawSolid(S);
	glPopMatrix();
	glutSwapBuffers();
}
void ProcessMenu(int value)
{
	switch (value)
	{
	case 1:
		bDepth = !bDepth;
		break;
	case 2:
		bCull = !bCull;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	int chos;
	cout << "choose input pattern or  show the example:  1:input pattern  |  2:example :";
	cin >> chos;
	double d[3];
	if (chos == 1)
	{
		int in_loop_num;
		cout << "input the number of inner loop:";
		cin >> in_loop_num;
		int num_vertex_out_lp;
		cout << "input the number of vertex of out loop:";
		cin >> num_vertex_out_lp;
		std::vector<std::vector<double>>out_lp_vector;
		std::vector<vertex*>out_point_ls;
		std::vector<std::vector<vertex*>>in_point_ls;
		for (int i = 0; i < num_vertex_out_lp; i++)
		{
			double xyz[3];
			cout << "input the 3dcoord of " << i + 1 << " vertex of out loop: ";
			cin >> xyz[0] >> xyz[1] >> xyz[2];
			vertex* now_v = new vertex(xyz);
			out_point_ls.push_back(now_v);
		}
		for (int i = 0; i < in_loop_num; i++)
		{
			int num_vertex_in_lp;
			cout << "input the number of vertex of " << i + 1 << " inner loop : ";
			cin >> num_vertex_in_lp;
			std::vector<vertex*>now_in_point_ls;
			for (int j = 0; j < num_vertex_in_lp; j++)
			{
				double xyz[3];
				cout << "input the 3dcoord of " << j + 1 << " vertex of " << i + 1 << " inner loop : ";
				cin >> xyz[0] >> xyz[1] >> xyz[2];
				vertex* now_v = new vertex(xyz);
				now_in_point_ls.push_back(now_v);
			}
			in_point_ls.push_back(now_in_point_ls);
		}
		cout << "input the sweeping vector: ";
		cin >> d[0] >> d[1] >> d[2];

		S = mvfs(out_point_ls[0]);
		loop* out_lp = S->fc->lp;
		for (int i = 0; i < out_point_ls.size() - 1; i++)
		{
			mev(out_point_ls[i], out_point_ls[i + 1], out_lp);
		}
		mef(out_point_ls[out_point_ls.size() - 1], out_point_ls[0], out_lp);

		loop* now_lp;

		for (int i = 0; i < in_point_ls.size(); i++)
		{
			now_lp = S->fc->next->lp;//增加内环开始
			mev(out_point_ls[0], in_point_ls[i][0], now_lp);
			for (int j = 0; j < in_point_ls[i].size() - 1; j++)
			{
				mev(in_point_ls[i][j], in_point_ls[i][j + 1], now_lp);
			}
			now_lp = kemr(out_point_ls[0], in_point_ls[i][0], now_lp);
			loop* in_lp1 = mef(in_point_ls[i][in_point_ls[i].size() - 1], in_point_ls[i][0], now_lp);
			kfmrh(out_lp, in_lp1);//增加内环结束
		}
	}
	else
	{
		double out_loop[][3] = { { 0.0, 0.0, 0.0 }, { 100.0, 0.0, 0.0 }, { 100.0, 100.0, 0.0 },{50.0,120.0,0.0}, { 0.0, 100.0, 0.0 } };
		double in_loop1[][3]= { { 20.0, 10.0, 0.0 }, { 40.0, 10.0, 0.0 }, { 40.0, 30.0, 0.0 }, { 20.0, 30.0, 0.0 } };
		double in_loop2[][3]= { { 30.0, 70.0, 0.0 }, { 50.0, 70.0, 0.0 }, { 50.0, 90.0, 0.0 }, { 30.0, 90.0, 0.0 } };
		double in_loop3[][3] = { { 20.0, 40.0, 0.0 }, { 40.0, 40.0, 0.0 }, { 40.0, 60.0, 0.0 }, { 20.0, 60.0, 0.0 } };
		double in_loop4[][3] = { { 70.0, 45.0, 0.0 }, { 90.0, 45.0, 0.0 }, { 90.0, 65.0, 0.0 }, { 70.0, 65.0, 0.0 } };
		
		d[0] = 10.0;
		d[1] = 20.0;
		d[2] = 30.0;
		vertex *outpoint[5];
		vertex *inpoint1[4];
		vertex *inpoint2[4];
		vertex *inpoint3[4];
		vertex *inpoint4[4];
		for (int i = 0; i < 4; i++)
		{
			outpoint[i] = new vertex(out_loop[i]);
			inpoint1[i] = new vertex(in_loop1[i]);
			inpoint2[i] = new vertex(in_loop2[i]);
			inpoint3[i] = new vertex(in_loop3[i]);
			inpoint4[i] = new vertex(in_loop4[i]);
		}
		outpoint[4] = new vertex(out_loop[4]);
		S = mvfs(outpoint[0]);
		loop *out_lp = S->fc->lp;
		for (int i = 0; i < 4; i++)
		{
			mev(outpoint[i], outpoint[i+1], out_lp);
		}
		mef(outpoint[4], outpoint[0], out_lp);
		loop* now_lp;

		now_lp = S->fc->next->lp;//增加第一个内环开始
		mev(outpoint[0], inpoint1[0], now_lp);
		mev(inpoint1[0], inpoint1[1], now_lp);
		mev(inpoint1[1], inpoint1[2], now_lp);
		mev(inpoint1[2], inpoint1[3], now_lp);
		now_lp = kemr(outpoint[0], inpoint1[0], now_lp);
		loop *in_lp1 = mef(inpoint1[3], inpoint1[0], now_lp);
		kfmrh(out_lp, in_lp1);//增加内环结束

		now_lp = S->fc->next->lp;//增加第二个内环
		mev(outpoint[0], inpoint2[0], now_lp);
		mev(inpoint2[0], inpoint2[1], now_lp);
		mev(inpoint2[1], inpoint2[2], now_lp);
		mev(inpoint2[2], inpoint2[3], now_lp);
		now_lp = kemr(outpoint[0], inpoint2[0], now_lp);
		loop *in_lp2 = mef(inpoint2[3], inpoint2[0], now_lp);
		kfmrh(out_lp, in_lp2);

		now_lp = S->fc->next->lp;//增加第三个内环
		mev(outpoint[0], inpoint3[0], now_lp);
		mev(inpoint3[0], inpoint3[1], now_lp);
		mev(inpoint3[1], inpoint3[2], now_lp);
		mev(inpoint3[2], inpoint3[3], now_lp);
		now_lp = kemr(outpoint[0], inpoint3[0], now_lp);
		loop *in_lp3 = mef(inpoint3[3], inpoint3[0], now_lp);
		kfmrh(out_lp, in_lp3);

		now_lp = S->fc->next->lp;//增加第四个内环
		mev(outpoint[0], inpoint4[0], now_lp);
		mev(inpoint4[0], inpoint4[1], now_lp);
		mev(inpoint4[1], inpoint4[2], now_lp);
		mev(inpoint4[2], inpoint4[3], now_lp);
		now_lp = kemr(outpoint[0], inpoint4[0], now_lp);
		loop *in_lp4 = mef(inpoint4[3], inpoint4[0], now_lp);
		kfmrh(out_lp, in_lp4);
	}
	sweep(S->fc, d);


	face *now_f = S->fc;
	int fc_cnt=0;
	while (now_f != nullptr)
	{
		fc_cnt++;
		now_f = now_f->next;
	}

	for (int i = 0; i < fc_cnt; i++)
	{
		colors[i][0] = static_cast<float>(rand()) / RAND_MAX;
		colors[i][1] = static_cast<float>(rand()) / RAND_MAX;
		colors[i][2] = static_cast<float>(rand()) / RAND_MAX;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Euler");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluTessCallback(obj, GLU_TESS_VERTEX, (void (CALLBACK *)())vertexCallback);
	gluTessCallback(obj, GLU_TESS_BEGIN, (void (CALLBACK *)())beginCallback);
	gluTessCallback(obj, GLU_TESS_END, (void (CALLBACK *)())endCallback);
	gluTessCallback(obj, GLU_TESS_ERROR, (void (CALLBACK *)())errorCallback);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	glutCreateMenu(ProcessMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("深度测试", 1);
	glutAddMenuEntry("剔除背面", 2);

	glutMainLoop();
	return 0;
}