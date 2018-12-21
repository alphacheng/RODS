// DDSYS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "dof.h"
#include "spring.h"
#include "dsystem.h"

constexpr bool FIXED = true;
constexpr bool NORM = true;

using namespace std;

int main()
{
	double m = 1.0;
	double k = 2025.0;
	double c = 1.0;
	double zeta = 0.0;
	int ndof = 3;
	dsystem *ds = new dsystem(zeta);

	ds->addDof(0, m, FIXED);
	for (int i = 1; i <= ndof; i++)
	{
		ds->addDof(i, m);
	}

	for (int i = 1; i <= ndof; i++)
	{
		ds->addSpring(i, i-1, i, k);
	}

	ds->assembleMassMatrix();
	ds->M.print();

	ds->assembleStiffnessMatrix();
	ds->K.print();

	ds->solveEigen();
	ds->P.print();
	ds->omg.print();

	zeta = 0.02;
	c = 2.0*zeta*k/ds->omg(0);
	for (int i = 1; i <= ndof; i++)
	{
		ds->addDashpot(i, i-1, i, c);
	}

	cout << ds->eqnCount << endl;

	ds->buildInherentDampingMatrix();
	ds->assembleDampingMatrix();
	ds->C.print("Damping Matrix:");

	mat M_d = ds->Phi.t()*ds->M*ds->Phi;
	mat K_d = ds->Phi.t()*ds->K*ds->Phi;
	mat C_d = ds->Phi.t()*ds->C*ds->Phi;

	ds->solveComplexEigen();

	ds->solveStochasticResponse();

}