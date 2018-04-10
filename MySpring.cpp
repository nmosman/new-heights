#include "MySpring.h"



MySpring::MySpring(MyBall* a, Person* b)
{
	ball_a = a;
	ball_b = b;
	line_s = new chai3d::cShapeLine();
	setLine();
}
MySpring::MySpring(MyBall* a, Person* b, double k)
{
	ball_a = a;
	ball_b = b;
	k_spring = k;
	line_s = new chai3d::cShapeLine();
	setLine();
}

MySpring::~MySpring()
{
}

void MySpring::getSpringForce()
{
	chai3d::cVector3d d_a = ball_a->m_tool->m_hapticPoint->getGlobalPosProxy();
	double distance = (d_a - ball_b->m_sphere->getLocalPos()).length();
	if (distance > n_length) {
		//chai3d::cVector3d force_a = -k_spring * (ball_a->getLocalPos() - ball_b->getLocalPos());
		chai3d::cVector3d force_a = -k_spring * (distance - n_length) * chai3d::cNormalize(d_a - ball_b->m_sphere->getLocalPos());
		chai3d::cVector3d force_b = -force_a;
		ball_a->force_p_spring += force_a;
		ball_b->force_p_spring += force_b;
		//std::cout << ball_a->getLocalPos() << std::endl;
		chai3d::cVector3d dir_b = cNormalize(d_a - ball_b->m_sphere->getLocalPos());
		chai3d::cVector3d dir_a = -dir_b;
		double mag_a1 = chai3d::cDot(ball_a->vel_p, dir_a);
		double mag_a2 = chai3d::cDot(ball_b->vel_p, dir_a);

		double mag_b1 = chai3d::cDot(ball_a->vel_p, dir_b);
		double mag_b2 = chai3d::cDot(ball_b->vel_p, dir_b);

		ball_a->force_p_spring_damping -= b_spring * (mag_a1 + mag_a2) * dir_a;
		ball_b->force_p_spring_damping -= b_spring * (mag_b1 + mag_b2) * dir_b;
	}
}

double MySpring::getSpringConstant()
{
	return k_spring;
}

void MySpring::setSpringConstant(double x)
{
	k_spring = x;
}

void MySpring::setLine()
{
	chai3d::cVector3d d_a = ball_a->m_tool->m_hapticPoint->getGlobalPosProxy();
	line_s->m_pointA = d_a;
	line_s->m_pointB = ball_b->m_sphere->getLocalPos();
}

void MySpring::setRestLength(double x)
{
	n_length = x;
}

void MySpring::setSpringDamping()
{
	//chai3d::cVector3d dir_b = cNormalize(ball_a->getLocalPos() - ball_b->getLocalPos());
	//chai3d::cVector3d dir_a = -dir_b;
	//double mag_a1 = chai3d::cDot(ball_a->vel_p, dir_a);
	//double mag_a2 = chai3d::cDot(ball_b->vel_p, dir_a);

	//double mag_b1 = chai3d::cDot(ball_a->vel_p, dir_b);
	//double mag_b2 = chai3d::cDot(ball_b->vel_p, dir_b);

	//ball_a->force_p_spring_damping -= b_spring * (mag_a1 + mag_a2) * dir_a;
	//ball_b->force_p_spring_damping -= b_spring * (mag_b1 + mag_b2) * dir_b;
	//std::cout << ball_a->force_p_spring_damping << "   " << ball_a->vel_p << std::endl;
	//std::cout << ball_a->force_p_spring_damping << "   " << ball_a->getLocalPos() << std::endl;
}

void MySpring::setSpringDampingCoefficient(double x)
{
	b_spring = x;
}
void MySpring::limitMovement()
{
	chai3d::cVector3d distance = ball_b->pos_p - ball_a->m_tool->m_hapticPoint->getGlobalPosProxy();
	if (distance.length() > n_length)
	{
		distance = chai3d::cNormalize(distance)*n_length;
		ball_b->pos_p = distance + ball_a->m_tool->m_hapticPoint->getGlobalPosProxy();
	}
}
