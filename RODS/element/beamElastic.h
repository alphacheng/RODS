#pragma once

#include "element2D.h"

class beamElastic :
	public element2D
{
public:
	beamElastic(const int id, node * nodeI, node * nodeJ, const double EI);
	~beamElastic();

	void buildMatrix();
	void getResponse(const bool update = false);
	void assembleStiffnessMatrix(mat &K);


	double EI;
	double *ue, *f;

	mat::fixed<2, 2> k;
	mat::fixed<2, 6> T;
	mat::fixed<6, 6> K;
	vec::fixed<6> u;
};