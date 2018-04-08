#include "Person.h"



//Person::Person()
//{
//}


Person::~Person()
{
}

chai3d::cVector3d Person::calcForceNet()
{
	if (hasForce) {
		return force_p_spring + force_p_gravity + force_p_damping + force_p_spring_damping + force_p_device;
	}
	else
	{
		return chai3d::cVector3d(0.0, 0.0, 0.0);
	}
}

void Person::integrate()
{
	acc_p = calcForceNet() / mass_p;
	vel_p = vel_p + deltaT * acc_p;
	pos_p = pos_p + deltaT * vel_p;

	//std::cout << vel_p << std::endl;
}



void Person::moveBall()
{
	force_p_damping = chai3d::cVector3d(0.0, 0.0, 0.0);
	force_p_gravity = chai3d::cVector3d(0.0, 0.0, 0.0);
	force_p_device = chai3d::cVector3d(0.0, 0.0, 0.0);

	//pos_p = this->m_sphere->getLocalPos();

	getForceDamping();
	//getSpringDamping();
	getForceGravity();
	//getForceDevice();
	integrate();

	force_p_spring = chai3d::cVector3d(0.0, 0.0, 0.0);
	force_p_spring_damping = chai3d::cVector3d(0.0, 0.0, 0.0);

	this->m_sphere->setLocalPos(pos_p);
}

void Person::setBallPos(chai3d::cVector3d a_pos)
{
	this->m_sphere->setLocalPos(a_pos);
	this->pos_p = a_pos;
}

void Person::getForceDevice()
{
	//this->computeInteractionForces();
	//force_p_device = this->m_sphere->getDeviceLocalForce();
}

void Person::getForceGravity()
{
	force_p_gravity = mass_p * chai3d::cVector3d(0.0, 0.0, -9.81);
}

void Person::getForceDamping()
{
	force_p_damping = -b_air * vel_p;
}

void Person::getSpringDamping()
{
	force_p_spring_damping = -b_spring * vel_p;
}
