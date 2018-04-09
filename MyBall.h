#pragma once
#include "chai3d.h"
class MyBall
{
public:
	MyBall(chai3d::cWorld* world)
	{
		m_tool = new chai3d::cToolCursor(world);
		pos_p = m_tool->getLocalPos();
		initPos = pos_p;
	};


	MyBall(chai3d::cWorld* world, chai3d::cVector3d pos)
	{
		m_tool = new chai3d::cToolCursor(world);
		//m_tool->setLocalPos(pos);
		initPos = pos;
	};



	virtual ~MyBall();

	//variables 
	double mass_p = 0.05;				//kg
	double b_air = 1.0;					//Ns/m
	double b_spring = 1.0;				//Ns/m
	double deltaT = 0.001;
	double k_contact = 1000; //N/m
	double b_contact = 100;
	bool hasForce = true;

	chai3d::cToolCursor* m_tool;
	//chai3d::cShapeSphere* m_sphere;

	chai3d::cVector3d initPos;
	chai3d::cVector3d pos_p, vel_p = chai3d::cVector3d(0.0, 0.0, 0.0), acc_p = chai3d::cVector3d(0.0, 0.0, 0.0);
	chai3d::cVector3d force_p_spring = chai3d::cVector3d(0.0, 0.0, 0.0), force_p_gravity, force_p_damping, force_p_spring_damping, force_p_device;

	//function
	void moveBall();
	void setBallPos(chai3d::cVector3d a_pos);
	void resetBallPosAndForces();
private:
	void getForceDevice();
	void getForceGravity();
	void getForceDamping();
	void getSpringDamping();
	
	chai3d::cVector3d calcForceNet();
	void integrate();
};

