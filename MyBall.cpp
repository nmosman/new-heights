#include "MyBall.h"



//MyBall::MyBall()
//{
//}


MyBall::~MyBall()
{
}

chai3d::cVector3d MyBall::calcForceNet()
{
	if (hasForce) {
		return force_p_spring + force_p_gravity + force_p_damping + force_p_spring_damping + force_p_device;
	}
	else
	{
		return chai3d::cVector3d(0.0, 0.0, 0.0);
	}
}

void MyBall::integrate()
{
	acc_p = calcForceNet() / mass_p;
	vel_p = vel_p + deltaT * acc_p;
	pos_p = pos_p + deltaT * vel_p;

	//std::cout << vel_p << std::endl;
}



void MyBall::moveBall()
{
	force_p_damping = chai3d::cVector3d(0.0, 0.0, 0.0);
	force_p_gravity = chai3d::cVector3d(0.0, 0.0, 0.0);
	force_p_device = chai3d::cVector3d(0.0, 0.0, 0.0);

	pos_p = this->m_tool->getLocalPos();

	getForceDamping();
	//getSpringDamping();
	getForceGravity();
	getForceDevice();
	//integrate();
	this->m_tool->setDeviceLocalForce(this->calcForceNet());

	force_p_spring = chai3d::cVector3d(0.0, 0.0, 0.0);
	force_p_spring_damping = chai3d::cVector3d(0.0, 0.0, 0.0);

	//this->m_tool->setLocalPos(pos_p);
}

void MyBall::setBallPos(chai3d::cVector3d a_pos)
{
	this->m_tool->setLocalPos(a_pos);
	this->pos_p = a_pos;
}

void MyBall::resetBallPosAndForces()
{
	this->m_tool->setLocalPos(this->initPos);
	//this->moveBall();
}
void MyBall::getForceDevice()
{
	this->m_tool->computeInteractionForces();
	force_p_device = this->m_tool->getDeviceLocalForce();
}

void MyBall::getForceGravity()
{
	force_p_gravity = mass_p * chai3d::cVector3d(0.0, 0.0, -9.81);
}

void MyBall::getForceDamping()
{
	force_p_damping = -b_air * vel_p;
}

void MyBall::getSpringDamping()
{
	force_p_spring_damping = -b_spring * vel_p;
}
