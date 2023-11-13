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

BOOL bDepth = TRUE;//深度测试开关
BOOL bCull = FALSE;//背面剔除开关
int color_faces[3000][3];//存储随机产生的各面颜色
int idx = 0;
int index = 0;

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

bool InitGL()
{
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
	return true;
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

	if (xRot > 356.0f) xRot = 0.0f;//此处不能用==360的操作，浮点数无法做出精确等于某个值得判断
	if (xRot < -1.0f) xRot = 355.0f;//此处不能用==-5的操作，理由同上。本质上是0、5、10、...、350、355这组值的轮换，像钟表那样
	if (yRot > 356.0f) yRot = 0.0f;
	if (yRot < -1.0f) yRot = 355.0f;
	if (zRot > 356.0f) zRot = 0.0f;
	if (zRot < -1.0f) zRot = 355.0f;

	//刷新窗口
	glutPostRedisplay();
}
void show(solid *s)
{
	face *fc = s->fc;
	while (fc != nullptr)
	{//依次绘制各面
		loop *temploop = fc->lp;
		unsigned char red = color_faces[index][0];
		unsigned char green = color_faces[index][1];
		unsigned char blue = color_faces[index][2];
		if (fc->id == -1)
		{
			red = 0;
			green = 0;
			blue = 0;
		}
		while (temploop != nullptr)
		{//遍历当前面的各点
			halfedge *hf = temploop->hfedge;
			vertex *sv = hf->start;
			glColor3b(red, green, blue);
			glBegin(GL_POLYGON);
			double coord[3];
			coord[0] = sv->x;
			coord[1] = sv->y;
			coord[2] = sv->z;
			glVertex3d(coord[0], coord[1], coord[2]);
			//gluTessVertex(obj, coord, coord);
			vertex *tmp = hf->next->start;
			hf = hf->next;
			while (tmp != sv)
			{
				coord[0] = tmp->x;
				coord[1] = tmp->y;
				coord[2] = tmp->z;
				glVertex3d(coord[0], coord[1], coord[2]);
				hf = hf->next;
				tmp = hf->start;
			}
			glEnd();
			temploop = temploop->next;
		}
		fc = fc->next;
		index++;
	}
}

void drawLoop(loop* outerLoop) {
	
	
	glBegin(GL_LINE_LOOP);
	halfedge* currentHalfEdge = outerLoop->hfedge;
	do {
		glVertex3d(currentHalfEdge->start->x, currentHalfEdge->start->y, currentHalfEdge->start->z);
		currentHalfEdge = currentHalfEdge->next;
	} while (currentHalfEdge != outerLoop->hfedge);
	glEnd();
	
}

void drawFace(face* currentFace) {
	// 生成随机颜色
	float red = static_cast<float>(rand()) / RAND_MAX;
	float green = static_cast<float>(rand()) / RAND_MAX;
	float blue = static_cast<float>(rand()) / RAND_MAX;

	glColor3f(red, green, blue);

	loop* outerLoop = currentFace->lp;
	drawLoop(outerLoop);

	// 绘制内环
	loop* innerLoop = outerLoop->next;
	while (innerLoop != nullptr) {
		drawLoop(innerLoop);
		innerLoop = innerLoop->next;
	}
}

void drawSolid(solid* mySolid) {
	face* currentFace = mySolid->fc;
	while (currentFace != nullptr) {
		drawFace(currentFace);
		currentFace = currentFace->next;
	}
}
void RenderScene(void)
{
	//清除颜色及深度缓冲区
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
	//指定逆时针绕法的多边形为多边形正面
	glFrontFace(GL_CW);

	drawSolid(S);

	glPopMatrix();
	glFlush();
	//刷新命令缓冲区
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
	double out_loop[][3]= { { 0.0, 0.0, 0.0 }, { 60.0, 0.0, 0.0 }, { 60.0, 100.0, 0.0 }, { 0.0, 100.0, 0.0 } };
	double in_loop1[][3]= { { 20.0, 20.0, 0.0 }, { 40.0, 20.0, 0.0 }, { 40.0, 40.0, 0.0 }, { 20.0, 40.0, 0.0 } };
	double in_loop2[][3]= { { 20.0, 60.0, 0.0 }, { 40.0, 60.0, 0.0 }, { 40.0, 80.0, 0.0 }, { 20.0, 80.0, 0.0 } };

	vector<double **>all_in_loop;
	vertex *outpoint[4];
	vertex *inpoint1[4];
	vertex *inpoint2[4];
	for (int i = 0; i < 4; i++)
	{
		outpoint[i] = new vertex(out_loop[i]);
		inpoint1[i] = new vertex(in_loop1[i]);
		inpoint2[i] = new vertex(in_loop2[i]);
	}
	S = mvfs(outpoint[0]);
	loop *out_lp = S->fc->lp;
	for (int i = 0; i < 3; i++)
	{
		mev(outpoint[i], outpoint[i+1], out_lp);
	}
	mef(outpoint[3], outpoint[0], out_lp);

	loop *now_lp = S->fc->next->lp;
	mev(outpoint[0], inpoint1[0], now_lp);
	mev(inpoint1[0], inpoint1[1], now_lp);
	mev(inpoint1[1], inpoint1[2], now_lp);
	mev(inpoint1[2], inpoint1[3], now_lp);
	now_lp = kemr(outpoint[0], inpoint1[0], now_lp);
	loop *in_lp = mef(inpoint1[3], inpoint1[0], now_lp);
	kfmrh(out_lp, in_lp);

	now_lp = S->fc->next->lp;
	mev(outpoint[0], inpoint2[0], now_lp);
	mev(inpoint2[0], inpoint2[1], now_lp);
	mev(inpoint2[1], inpoint2[2], now_lp);
	mev(inpoint2[2], inpoint2[3], now_lp);
	now_lp = kemr(outpoint[0], inpoint2[0], now_lp);
	loop *in_lp2 = mef(inpoint2[3], inpoint2[0], now_lp);
	kfmrh(out_lp, in_lp2);

	double d[3] = { 0.0,0.0,60.0 };
	sweep(S->fc, d);


	face *now_f = S->fc;
	int fc_cnt=0;
	while (now_f != nullptr)
	{
		fc_cnt++;
		now_f = now_f->next;
	}

	srand(time(NULL));//随机种子摇一摇
	//对每个面产生一个随机颜色
	for (int i = 0; i < fc_cnt; i++)
	{
		unsigned char red = rand() % 255;
		unsigned char green = rand() % 255;
		unsigned char blue = rand() % 255;
		color_faces[idx][0] = red;
		color_faces[idx][1] = green;
		color_faces[idx][2] = blue;
		idx++;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//打开双缓存，颜色缓存，深度缓存
	glutInitWindowPosition(100, 100);//窗口位置
	glutInitWindowSize(1000, 600);//窗口大小
	glutCreateWindow("Euler");//窗口标题
	InitGL();//环境初始化
	glutReshapeFunc(ChangeSize);//处理窗口改变
	glutSpecialFunc(SpecialKeys);//处理右击屏幕
	glutDisplayFunc(RenderScene);//处理绘图
	//创建右键菜单
	glutCreateMenu(ProcessMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("深度测试", 1);
	glutAddMenuEntry("剔除背面", 2);

	glutMainLoop();
	system("pause");
	return 0;
}