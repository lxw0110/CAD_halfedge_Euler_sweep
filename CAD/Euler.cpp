#include <iostream>
#include <algorithm>
#include "Euler.h"


void solid_add_face(face *f, solid *s)
{
	face *now_f = s->fc;
	if (now_f == nullptr)
	{
		s->fc = f;
	}
	else
	{
		while (now_f->next != nullptr)
		{
			now_f = now_f->next;
		}
		now_f->next = f;
		f->pre = now_f;
	}
	f->Solid = s;
}
void solid_add_edge(edge *e,solid *&s)
{
	edge *now_e = s->ed;
	if (now_e == nullptr)
	{
		s->ed = e;
	}
	else
	{
		while (now_e->next != nullptr)
		{
			now_e = now_e->next;
		}
		now_e->next = e;
		e->pre = now_e;
	}
}

void solid_add_vertex(vertex *v, solid *&s)
{
	vertex *now_v = s->ver;
	if (now_v == nullptr)
	{
		s->ver = v;
		
	}
	else
	{
		while (now_v->next != nullptr)
		{
			now_v = now_v->next;
		}
		now_v->next = v;
		v->pre = now_v;
	}

}


void loop_add_hf(halfedge *e, loop *&l)
{
	halfedge *now_e = l->hfedge;
	if (now_e == nullptr)
	{
		l->hfedge = e;
	}
	else
	{
		while (now_e->next != nullptr)
		{
			now_e = now_e->next;
		}
		now_e->next = e;
		e->pre = now_e;
	}
	e->lp = l;
}



void face_add_loop(loop *l, face *f)
{
	if (f->lp == nullptr)
	{
		f->lp = l;
	}
	else
	{
		loop *now_loop=f->lp;
		while (now_loop->next != nullptr)
		{
			now_loop = now_loop->next;
		}
		now_loop->next = l;
		l->pre = now_loop;
	}
	l->fc = f;
}


solid *mvfs(double *point)
{
	solid *s=new solid;
	loop *l = new loop;
	face *f = new face;
	vertex *v = new vertex(point);
	solid_add_face(f, s);
	face_add_loop(l, f);
	solid_add_vertex(v, s);
}

halfedge *mev(vertex *exist_point, double *new_point, loop *L)
{
	solid *sd = L->fc->Solid;
	edge *e = new edge;
	vertex *new_ver = new vertex(new_point);
	halfedge *he1 = new halfedge, *he2 = new halfedge;
	//CVertex* v1 = pSolid->FindVertex(point1);
	he1->start = exist_point;
	he1->end = new_ver;
	he2->start = new_ver;
	he2->end = exist_point;
	he1->lp = he2->lp = L;
	he1->ed = he2->ed = e;
	e->l_half = he1;
	e->r_half = he2;
	he1->bro = he2;
	he2->bro = he1;
	if (L->hfedge == nullptr)
	{
		he1->next = he2;
		he2->next = he1;
		he1->pre = he2;
		he2->pre = he1;
		L->hfedge = he1;
	}
	else
	{
		halfedge *now_he = L->hfedge;
		while (now_he->end != exist_point)
		{
			now_he = now_he->next;
		}
		he2->next = now_he->next;
		now_he->next->pre = he2;
		now_he->next = he1;
		he1->pre = now_he;
		he1->next = he2;
		he2->pre = he1;
	}
	solid_add_edge(e, sd);
	solid_add_vertex(new_ver, sd);
	return he1;
}

loop *mef(vertex *point1, vertex *point2, loop *L)
{
	solid *sd = L->fc->Solid;
	edge *e = new edge;
	halfedge *he1 = new halfedge, *he2 = new halfedge;
	face *f = new face;
	loop *l = new loop;
	he1->start = point1;
	he1->end = point2;
	he2->start = point2;
	he2->end = point1;
	he1->bro = he2;
	he2->bro = he1;
	he1->ed = e;
	he2->ed = e;
	e->l_half = he1;
	e->r_half = he2;
	halfedge *now_e = L->hfedge;
	halfedge *e_a, *e_b, *e_c;
	while (now_e->start != point1)
	{
		now_e = now_e->next;
	}
	e_a = now_e;
	e_c = e_a;
	now_e= L->hfedge;
	while (now_e->start!= point2)
	{
		now_e = now_e->next;
	}
	e_b = now_e;
	loop_add_hf(he2, l);
	while (e_c != nullptr)
	{
		if (e_c == e_b)
			break;
		e_c->lp = l;
		e_c = e_c->next;
	}
	he2->next = e_a;
	e_c = e_a->pre;
	e_a->pre = he2;
	e_b->pre->next = he2;
	he2->pre = e_b->pre;
	
	e_c->next = he1;
	he1->pre = e_c;
	he1->next = e_b;
	e_b->pre = he1;
	he1->lp = L;
	L->hfedge = he1;

	solid_add_edge(e, sd);
	face_add_loop(l, f);
	return l;
}

loop *kemr(vertex *start_point, vertex *end_point, loop *L)
{
	solid *s = L->fc->Solid;
	halfedge *he1 = L->hfedge;
	while (he1 != nullptr)
	{
		if ((*he1->start == *start_point) && (*he1->end == *end_point))
		{
			break;
		}
		he1 = he1->next;
	}
	halfedge *he2 = he1->bro;
	loop *l = new loop;
	halfedge *now_hf = he1->next;
	loop_add_hf(now_hf, l);
	now_hf = now_hf->next;
	while (now_hf != nullptr&&now_hf != he2)
	{
		now_hf->lp = l;
		now_hf = now_hf->next;
	}
	he1->next->pre = he2->pre;
	he2->pre->next = he1->next;
	face_add_loop(l, L->fc);
	L->hfedge = he1->pre;
	he1->pre->next = he2->next;
	he2->next->pre = he1->pre;
	edge *now_e = s->ed;
	while (now_e != nullptr)
	{
		if (now_e->l_half == he1 || now_e->r_half == he1)
			break;
		now_e = now_e->next;
	}
	if (now_e->pre == nullptr&&now_e->next!=nullptr)
	{
		s->ed = now_e->next;
		now_e->next->pre = nullptr;
	}
	else if (now_e->next == nullptr&&now_e->pre != nullptr)
	{
		now_e->pre->next = nullptr;
	}
	else if (now_e->next == nullptr&&now_e->pre==nullptr)
	{
		s->ed = nullptr;
	}
	else
	{
		now_e->pre->next = now_e->next;
		now_e->next->pre = now_e->pre;
	}
	return l;
}

void kfmrh(loop *out_lp, loop *in_lp)
{
	face *now_f = in_lp->fc;
	face_add_loop(in_lp, out_lp->fc);
}