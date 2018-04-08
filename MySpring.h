#pragma once
#include "MyBall.h"
#include "chai3d.h"
#include "Person.h"
class MySpring
{
public:
	MySpring(MyBall* a, Person* b);
	MySpring(MyBall* a, Person* b, double k);
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
	double n_length = 0.001;
	double b_spring = 1.0;				//Ns/m
	MyBall* ball_a;
	Person* ball_b;

};

