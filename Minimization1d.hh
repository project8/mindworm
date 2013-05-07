#pragma once
#include <string>
using namespace std;

class SingleParameterFunction
{
public:
	virtual ~SingleParameterFunction() {};
	virtual double evaluate(const double &param) = 0;
};

class Golden_Mean_Minimizer
{
public:
	Golden_Mean_Minimizer(SingleParameterFunction *f=NULL);
	void setFunction(SingleParameterFunction *f) {func=f; done=false; success=false;};

	bool isDone() {return done;};
	bool getSuccess() {return success;};

	string getInfo();

	//returns mininum value with a and b setting the scale of the
	//parameters, parameters outside will be searched
	double minimize(double aa,double ab);
	//returns mininum value for parameters between minval and maxval
	//parameters outside will not be searched
	double minimize_between(double minparam,double maxparam);
	//returns the minimum value for parameters bounded by minval
	//but undbounded in the other direction
	double minimize_onesided(double minparam,double ab);

	//subtasks
	  //unbounded bracketing of a minimum
	void bracket_minimum(double aa,double bb);
	  //bracketing of a minimum with one side fixed
	void bracket_minimum_onesided(double minparam,double bb);
	  //called to initialize thing when you already have a min/max bound
	void initialize_bracket(double minval,double maxval);
	  //bracket a bounded minimum when you have a good guess at where it is
	void bracket_minimum_bounded(double aa,double bb,double minval,double maxval);
	void minimizer_step();

	//status variables
	double a,b,c;     //three parameter points
	double fa,fb,fc;  //values at said points
	string message;   //status message
	bool success;     //did we successfully converge
	bool done;        //are no more steps needed/wanted
	double minimum_param; //parameter found that minimizes
	double minimum_value; //smallest value of function found

	//controls
	size_t max_iterations; //maximum iterations to run for
	double accuracy;  //maximum parameter resolution needed
	double epsilon;   //maximum function value resolution needed

	SingleParameterFunction *func;
};
