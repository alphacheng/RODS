#pragma once

#include "dof.h"
#include "element1D.h"
#include <armadillo>

using namespace arma;

class spring : public element1D
{
public:
	spring(const int id, dof *i, dof *j, const double k);
	~spring();

	void buildMatrix();
	void assembleStiffnessMatrix(mat &K);
	void getResponse(const bool update = false);

	double k;
	double u, f;

	mat::fixed<2,2> K;
};

