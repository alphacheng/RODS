#include "dofRecorder.h"

dofRecorder::dofRecorder(const int id, std::vector<dof *> dofs, response rtype, char * fileName)
{
	this->id = id;
	this->dofs = dofs;
	this->n = dofs.size();
	this->rtype = rtype;
	this->fileName = fileName;
}

dofRecorder::~dofRecorder()
{
}

void dofRecorder::record(const int cstep, const double ctime)
{
	Res(cstep, 0) = ctime;
	for (int i = 0; i < n; i++)
	{
		dof * d = dofs[i];
		switch (rtype)
		{
		case DISP:
			Res(cstep, i + 1) = d->dsp;
			break;
		case VEL:
			Res(cstep, i + 1) = d->vel;
			break;
		case ACC:
			Res(cstep, i + 1) = d->acc;
			break;
		case FORCE:
			break;
		case DEF:
			break;
		default:
			break;
		}
	}
}
