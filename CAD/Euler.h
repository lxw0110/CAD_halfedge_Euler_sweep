#pragma once

#include "geo_data.h"

void solid_add_face(face *f, solid *s);
void solid_add_edge(edge *e, solid *s);
void solid_add_vertex(vertex *v, solid *s);
void loop_add_hf(halfedge *e, loop *l);
void face_add_loop(loop *l, face *f);
solid *mvfs(vertex *point);
halfedge *mev(vertex *exist_point, vertex *new_ver, loop *L);
loop *mef(vertex *start_point, vertex *end_point, loop *L);
loop *kemr(vertex *start_point, vertex *end_point, loop *L);
void kfmrh(loop *out_lp, loop *in_lp);
void sweep(face *f, double *d);