#pragma once
#include "MyBall.h"
#include "chai3d.h"
class MySpring
{
public:
	MySpring(MyBall* a, MyBall* b);
	MySpring(MyBall* a, MyBall* b, double k);
	virtual ~MySpring();

	chai3d::cShapeLine* line_s;

	//functions
	void getSpringForce();
	double getSpringConstant();
	void setSpringConstant(double x);
	void setLine();
	void setRestLength(double x);
	void setSpringDamping();
	void setSpringDampingCoefficient(double x);

private:
	//variables
	double k_spring = 200;				//N/ m
	double n_length = 0.0;
	double b_spring = 1.0;				//Ns/m
	MyBall* ball_a;
	MyBall* ball_b;

};

