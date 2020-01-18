// DDSYS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "dof.h"
#include "dsystem.h"
#include "solver.h"

constexpr bool FIXED = true;
constexpr bool NORM = true;

using namespace std;

void example_sdof()
{
	double m = 1.0;
	double k = 100.0;
	int ndof = 1;
	dsystem *ds = new dsystem();

	ds->addDof(0, m, FIXED);
	ds->addDof(1, m);

	ds->addSpring(0, 0, 1, k);

	ds->assembleMassMatrix();
	ds->assembleStiffnessMatrix();

	ds->solveEigen();
	ds->P.print("Natural Periods:");

	double zeta = 0.02;
	double c = 2.0*zeta*k / ds->omg(0);

	ds->addDashpot(1, 0, 1, c);

	ds->buildInherentDampingMatrix();
	ds->assembleDampingMatrix();

	ds->solveStochasticSeismicResponse();
	ds->dsp.print("StochasticSeismicResponse:");
	cout << "AnalyticalSolution: " << sqrt(PI / 4 / ds->omg(0) / zeta) / ds->omg(0) << endl;

	double dt = 0.02;
	int nsteps = 1000;
	vec t = linspace(0.0, dt*(nsteps - 1), nsteps);
	double Omg = 1*ds->omg(0);
	vec ag = arma::sin(Omg*t);
	ds->addTimeseries(1, dt, ag);

	char eq[] = "EQ-S-1.txt";
	ds->addTimeseries(2, 0.005, eq);

	int nrd = 2;
	int *dofIds = new int[nrd]{ 0,1 };
	char dispOutput[] = "disp.csv";
	ds->addDofRecorder(0, dofIds, nrd, DISP, dispOutput);

	int nre = 2;
	int *eleIds = new int[nre]{ 0,1 };
	char eleOutput[] = "force.csv";
	ds->addElementRecorder(0, eleIds, nre, FORCE, eleOutput);

	int ts = 2;
	ds->setDynamicSolver(Newmark_NL);
	ds->solveTimeDomainSeismicResponse(ts, 1.0, 1);
	system("python post.py");
}

void example_sdof_bl()
{
	double m = 1.0;
	double k = 100.0;
	double c = 1.0;
	double zeta = 0.0;

	double uy = 0.01;
	double alpha = 0.1;
	int ndof = 1;
	dsystem *ds = new dsystem(zeta);

	ds->addDof(0, m, FIXED);
	ds->addDof(1, m);
	ds->addSpringBL(0, 0, 1, k, uy, alpha);

	ds->assembleMassMatrix();
	ds->M.print("Mass Matrix:");

	ds->assembleStiffnessMatrix();
	ds->K.print("Stiffness Matrix:");

	ds->solveEigen();
	ds->P.print("Natural Periods:");
	ds->omg.print("Natural Circular Frequencies:");

	zeta = 0.02;
	c = 2.0*zeta*k / ds->omg(0);
	cout << "Damping Coefficient: " << c << endl;

	ds->addDashpot(0, 0, 1, c);

	cout << "Numbers of equations: " << ds->eqnCount << endl;

	ds->buildInherentDampingMatrix();
	ds->assembleDampingMatrix();
	ds->C.print("Damping Matrix:");

	ds->solveStochasticSeismicResponse();
	ds->dsp.print("StochasticSeismicResponse:");
	cout << "AnalyticalSolution: " << sqrt(PI / 4 / ds->omg(0) / zeta) / ds->omg(0) << endl;

	double dt = 0.02;
	int nsteps = 1000;
	vec t = linspace(0.0, dt*(nsteps - 1), nsteps);
	double Omg = 0.9*ds->omg(0);
	vec ag = arma::sin(Omg*t);
	ds->addTimeseries(1, dt, ag);
	ds->addTimeseries(2, 0.005, "EQ-S-1.txt");

	int ts = 2;
	ds->solveTimeDomainSeismicResponseStateSpaceNL(ts, 1.0, 10);
	rowvec tt = ds->tss[ts]->time.t();
	tt.save("t.csv", csv_ascii);
	ds->u.save("dsp.csv", csv_ascii);

	system("python post.py");

}

void example_shear_building()
{
	double m = 1.0;
	double k = 100.0;
	double c = 1.0;
	double zeta = 0.0;
	int ndof = 1;
	dsystem *ds = new dsystem(zeta);

	ds->addDof(0, m, FIXED);
	for (int i = 1; i <= ndof; i++)
	{
		ds->addDof(i, m);
		ds->addSpring(i, i - 1, i, k);
	}

	ds->assembleMassMatrix();
	ds->M.print("Mass Matrix:");

	ds->assembleStiffnessMatrix();
	ds->K.print("Stiffness Matrix:");

	ds->solveEigen();
	ds->P.print("Natural Periods:");
	ds->omg.print("Natural Circular Frequencies:");

	zeta = 0.02;
	c = 2.0*zeta*k/ds->omg(0);
	cout << "Damping Coefficient: " << c << endl;

	for (int i = 1; i <= ndof; i++)
	{
		ds->addDashpot(i, i-1, i, c);
	}

	cout << "Numbers of equations: " << ds->eqnCount << endl;

	ds->buildInherentDampingMatrix();
	ds->assembleDampingMatrix();
	ds->C.print("Damping Matrix:");

	//ds->solveComplexEigen();
	ds->solveStochasticSeismicResponse();
	ds->dsp.print("StochasticSeismicResponse:");
	cout << "AnalyticalSolution: " << sqrt(PI/4/ds->omg(0)/zeta)/ds->omg(0) << endl;

	double dt = 0.02;
	int nsteps = 1000;
	vec t = linspace(0.0, dt*(nsteps-1), nsteps);
	double Omg = 0.9*ds->omg(0);
	vec ag = arma::sin(Omg*t);
	ds->addTimeseries(1, dt, ag);
	ds->addTimeseries(2, 0.005, "EQ-S-1.txt");
	
	int ts = 2;
	ds->solveTimeDomainSeismicResponseStateSpace(ts,1,10);
	rowvec tt = ds->tss[ts]->time.t();
	tt.save("t.csv", csv_ascii);
	ds->u.save("dsp.csv", csv_ascii);

}

void example_shear_building_spis2()
{
	double m = 1.0;
	double k = 2025.0;
	double c = 1.0;
	double zeta = 0.0;
	int ndof = 3;
	dsystem *ds = new dsystem(zeta);

	double mu = 0.05;
	double xi = 0.005;
	double kp = 0.05;

	double m_in = mu*m;
	double c_d = 2.0*xi*sqrt(m*k);
	double k_s = kp*k;

	ds->addDof(0, m, FIXED);
	for (int i = 1; i <= ndof; i++)
	{
		ds->addDof(i*10, m);
		ds->addSpring(i, (i-1)*10, i*10, k);
		ds->addDof(i*10+1, 0.0);
		ds->addSPIS2(i, (i-1)*10, i*10, i*10+1, m_in, c_d, k_s);
	}

	ds->assembleMassMatrix();
	ds->M.print("Mass Matrix:");

	ds->assembleStiffnessMatrix();
	ds->K.print("Stiffness Matrix:");

	ds->solveEigen();
	ds->P.print("Natural Periods:");
	ds->omg.print("Natural Circular Frequencies:");
	ds->Phi.print("Eigen Vectors:");

	//mat M_d = ds->Phi.t()*ds->M*ds->Phi;
	//M_d.print();

	cout << "Numbers of equations: " << ds->eqnCount << endl;

	ds->buildInherentDampingMatrix();
	ds->assembleDampingMatrix();
	ds->C.print("Damping Matrix:");

	//ds->solveStochasticSeismicResponse();
	//ds->dsp.print();

}

int main()
{
	example_sdof();
	//example_sdof_bl();
	//example_shear_building();
	//example_shear_building_spis2();
	return 0;
}

extern "C" {
	dsystem *ds = new dsystem();

	__declspec(dllexport) void set_damping_ratio(const double z) {
		ds->zeta = z;
	}

	__declspec(dllexport) size_t add_dof(const int id, const double m) {
		ds->addDof(id, m);
		return ds->dofs.size();
	}

	__declspec(dllexport) size_t add_dof_fixed(const int id, const double m) {
		ds->addDof(id, m, FIXED);
		return ds->dofs.size();
	}

	__declspec(dllexport) size_t add_spring(const int id, const int i, const int j, const double k) {
		ds->addSpring(id, i, j, k);
		return ds->springs.size();
	}

	__declspec(dllexport) size_t add_dashpot(const int id, const int i, const int j, const double c) {
		ds->addDashpot(id, i, j, c);
		return ds->dashpots.size();
	}

	__declspec(dllexport) size_t add_timeseries(const int id, const double dt, char* fileName) {
		ds->addTimeseries(id, dt, fileName);
		return ds->tss.size();
	}

	__declspec(dllexport) size_t assemble_matrix() {
		ds->assembleMatrix();
		return ds->eqnCount;
	}

	__declspec(dllexport) size_t add_dof_recorder(const int id, int *dofIds, const int n, const int rtype, char * fileName) {
		ds->addDofRecorder(id, dofIds, n, response(rtype), fileName);
		return ds->drs.size();
	}

	__declspec(dllexport) size_t add_ele_recorder(const int id, int *eleIds, const int n, const int rtype, char * fileName) {
		ds->addElementRecorder(id, eleIds, n, response(rtype), fileName);
		return ds->ers.size();
	}

	__declspec(dllexport) size_t set_dynamic_solver(const int s) {
		ds->setDynamicSolver(dsolver(s));
		return ds->dynamicSolver;
	}

	__declspec(dllexport) size_t solve_seismic_response(const int tsId, const double s = 1.0, const int nsub = 1) {
		ds->solveTimeDomainSeismicResponse(tsId, s, nsub);
		return ds->cstep;
	}

}