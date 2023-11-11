#pragma once

class solid
{
public:
	int id;
	face *fc;
	solid *next;
	solid *pre;
	edge *ed;
	vertex *ver;
};


class face
{
public:
	int id;
	solid *Solid;
	face *next;
	face *pre;
	loop *lp;
	
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
	int id;
	double x;
	double y;
	double z;
	vertex *next;
	vertex *pre;
	vertex() :id(0), next(nullptr), pre(nullptr)
	{};
	vertex(double *coord) :id(0), next(nullptr), pre(nullptr)
	{
		x = coord[0];
		y = coord[1];
		z = coord[2];
	};
	bool operator==(const vertex &b)
	{
		if ((x == b.x)&&(y == b.y)&&(z = b.z))
			return true;
		return false;
	}
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
};

class edge
{
public:
	halfedge *l_half;
	halfedge *r_half;
	edge *next;
	edge *pre;
};
