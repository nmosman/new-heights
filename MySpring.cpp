#include "MySpring.h"



MySpring::MySpring(MyBall* a, MyBall* b)
{
	ball_a = a;
	ball_b = b;
	line_s = new chai3d::cShapeLine();
	setLine();
}
MySpring::MySpring(MyBall* a, MyBall* b, double k)
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
	double distance = (ball_a->m_tool->getLocalPos() - ball_b->m_tool->getLocalPos()).length();
	if (distance > n_length) {
		chai3d::cVector3d force_a = -k_spring * (distance - n_length) * chai3d::cNormalize(ball_a->m_tool->getLocalPos() - ball_b->m_tool->getLocalPos());
		chai3d::cVector3d force_b = -force_a;
		ball_a->force_p_spring += force_a;
		ball_b->force_p_spring += force_b;
		chai3d::cVector3d dir_b = cNormalize(ball_a->m_tool->getLocalPos() - ball_b->m_tool->getLocalPos());
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
	line_s->m_pointA = ball_a->m_tool->getLocalPos();
	line_s->m_pointB = ball_b->m_tool->getLocalPos();
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
