#pragma once

#include "dof.h"
#include "element1D.h"
#include <armadillo>

using namespace arma;

class dashpot : public element1D
{
public:
	dashpot(const int id, dof *i, dof *j, const double c);
	~dashpot();

	void buildMatrix();
	void assembleDampingMatrix(mat & C);
	void getResponse(const bool update = false);

	double c;
	double u, f;

	mat::fixed<2, 2> C;
};

