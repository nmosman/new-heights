#pragma once
#include "chai3d.h"
class MyBall //: public chai3d::cShapeSphere
{
public:
	//MyBall(const double& a_radius,
	//	chai3d::cMaterialPtr a_material = chai3d::cMaterialPtr())
	//	: cShapeSphere(a_radius)
	//{};

	MyBall(chai3d::cWorld* world)
	{
		m_tool = new chai3d::cToolCursor(world);
		pos_p = this->m_tool->getLocalPos();
	};

	virtual ~MyBall();

	//variables 
	double mass_p = 0.05;				//kg
	double b_air = 1.0;					//Ns/m
	double b_spring = 1.0;				//Ns/m
	double deltaT = 0.001;
	double k_contact = 1000;			//N/m
	double b_contact = 100;
	bool hasForce = true;

	//cshapesphere
	chai3d::cToolCursor* m_tool;

	chai3d::cVector3d pos_p, vel_p = chai3d::cVector3d(0.0, 0.0, 0.0), acc_p = chai3d::cVector3d(0.0, 0.0, 0.0);
	chai3d::cVector3d force_p_spring, force_p_gravity, force_p_damping, force_p_spring_damping, force_p_device;

	//function
	void moveBall(chai3d::cVector3d device_pos, double device_radius);
	chai3d::cVector3d forcesBall();
	void setBallPos(chai3d::cVector3d a_pos);
private:
	void getForceDevice(chai3d::cVector3d device_pos, double device_radius);
	void getForceGravity();
	void getForceDamping();
	void getSpringDamping();
	chai3d::cVector3d calcForceNet();
	void integrate();
};

