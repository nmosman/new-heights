#pragma once
#include "chai3d.h"
class Person
{
public:
	Person(double radius)
	{
		m_sphere = new chai3d::cShapeSphere(radius);
		m_sphere->m_material->setGreen();
		pos_p = m_sphere->getLocalPos();
	};
	virtual ~Person();

	//variables 
	double mass_p = 1;				//kg
	double b_air = 1.0;					//Ns/m
	double b_spring = 1.0;				//Ns/m
	double deltaT = 0.001;
	double k_contact = 1000; //N/m
	double b_contact = 100;
	bool hasForce = true;

	chai3d::cShapeSphere* m_sphere;

	chai3d::cVector3d pos_p, vel_p = chai3d::cVector3d(0.0, 0.0, 0.0), acc_p = chai3d::cVector3d(0.0, 0.0, 0.0);
	chai3d::cVector3d force_p_spring = chai3d::cVector3d(0.0, 0.0, 0.0), force_p_gravity, force_p_damping, force_p_spring_damping, force_p_device = chai3d::cVector3d(0.0, 0.0, 0.0);

	//function
	void moveBall();
	void setBallPos(chai3d::cVector3d a_pos);
private:
	void getForceDevice();
	void getForceGravity();
	void getForceDamping();
	void getSpringDamping();
	chai3d::cVector3d calcForceNet();
	void integrate();
};

