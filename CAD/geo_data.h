#pragma once

class solid;
class face;
class loop;
class vertex;
class halfedge;
class edge;

class solid
{
public:
	int id;
	face *fc;
	solid *next;
	solid *pre;
	edge *ed;
	vertex *ver;
	solid() : id(0), fc(NULL), ed(NULL), next(NULL), pre(NULL) {};
};


class face
{
public:
	int id;
	solid *Solid;
	face *next;
	face *pre;
	loop *lp;
public:
	face() : id(0), Solid(NULL), lp(NULL), next(NULL), pre(NULL) {};
};

class loop
{
public:
	int id;
	loop *next;
	loop *pre;
	face *fc;
	halfedge *hfedge;
	loop() : id(0), hfedge(nullptr), fc(nullptr), next(nullptr), pre(nullptr) 
	{};
};

class vertex
{
public:
	double x;
	double y;
	double z;
	double coord[3];
	vertex *next;
	vertex *pre;
	vertex() : next(nullptr), pre(nullptr)
	{};
	vertex(double *c) : next(nullptr), pre(nullptr)
	{
		coord[0]=x = c[0];
		coord[1]=y = c[1];
		coord[2]=z = c[2];
	};
};

class halfedge
{
public:
	edge *ed;
	vertex *start;
	vertex *end;
	loop *lp;
	halfedge *next;
	halfedge *pre;
	halfedge *bro;
	halfedge() : ed(NULL), start(NULL), end(NULL),lp(NULL), next(NULL), pre(NULL), bro(NULL) {};
};

class edge
{
public:
	halfedge *l_half;
	halfedge *r_half;
	edge *next;
	edge *pre;
	edge() : l_half(NULL), r_half(NULL), next(NULL), pre(NULL) {};
};
