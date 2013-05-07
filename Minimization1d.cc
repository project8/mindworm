#include "Minimization1d.hh"
#include <sstream>
#include <math.h>
#include <iostream>
using namespace std;

void swap(double &x,double &y) {double z=y; y=x; x=z;};
#define GOLDEN 0.38196601

Golden_Mean_Minimizer::Golden_Mean_Minimizer(SingleParameterFunction *f)
{
	a=b=c=0;
	fa=fb=fc=0;
	accuracy=1e-1;
	epsilon=1;
	done=false;
	max_iterations=500;
	setFunction(f);
}

void Golden_Mean_Minimizer::bracket_minimum_onesided(double minparam,double bb)
{
	done=false;
	success=false;

	a=minparam;
	b=bb;
	fa=func->evaluate(a);
	fb=func->evaluate(b);
	double step=b-a;
	c=b+step;
	fc=func->evaluate(c);
	while(fc<=fb)
	{
		swap(fb,fc);
		swap(b,c);
		c+=step;
		fc=func->evaluate(c);
	}
	if(fabs(c-b)<(b-a))
	{
		swap(a,c);
		swap(fa,fc);
	}
	if((fb<fa)&&(fb<fc)) {minimum_param=b; minimum_value=fb;}
	else if(fa<fc) {minimum_param=a; minimum_value=fa;}
	else {minimum_param=c; minimum_value=fc;}

}
	
void Golden_Mean_Minimizer::bracket_minimum(double aa,double bb)
{
	done=false;
	success=false;
	a=aa;
	b=bb;
	fa=func->evaluate(a);
	fb=func->evaluate(b);
	if(fb>fa) { swap(fa,fb); swap(a,b); }
	double step=b-a;
	c=b+step;
	fc=func->evaluate(c);
//	cout << "a " << a << " b " << b << " c " << c << endl;
//	cout << "fa " << fa << " bf " << fb << " fc " << fc << endl;

	size_t its=0;
	while(fc<=fb)
	{
		swap(fa,fb);
		swap(a,b);
		swap(fb,fc);
		swap(b,c);
		c=b+step;
		step*=2;
		fc=func->evaluate(c);
		//cout << "a " << a << " b " << b << " c " << c << endl;
		//cout << "fa " << fa << " bf " << fb << " fc " << fc << endl;
		its++;
		if(its>max_iterations)
		{
			stringstream ss;
			ss << "Max iteration exceeded (" << max_iterations << ") in bracketing";
			done=true;
			message=ss.str();
			break;
		}
	}
	if(fabs(c-b)<(b-a))
	{
		swap(a,c);
		swap(fa,fc);
	}
	minimum_value=fb;
	minimum_param=b;
}

void Golden_Mean_Minimizer::bracket_minimum_bounded(double aa,double bb,double minval,double maxval)
{
	done=false;
	success=false;
	a=aa;
	b=bb;
	fa=func->evaluate(a);
	fb=func->evaluate(b);
	if(fb>fa) { swap(fa,fb); swap(a,b); }
	double step=b-a;
	c=b+step;
	if(c>maxval) c=maxval;
	if(c<minval) c=minval;
	fc=func->evaluate(c);
//	cout << "a " << a << " b " << b << " c " << c << endl;
//	cout << "fa " << fa << " bf " << fb << " fc " << fc << endl;

	size_t its=0;
	while(fc<=fb)
	{
		swap(fa,fb);
		swap(a,b);
		swap(fb,fc);
		swap(b,c);
		c=b+step;
		if(c>maxval) c=maxval;
		if(c<minval) c=minval;
		step*=2;
		fc=func->evaluate(c);
		if((c==maxval)||(c==minval)) {minimum_value=fc; minimum_param=c; return;}
		//cout << "a " << a << " b " << b << " c " << c << endl;
		//cout << "fa " << fa << " bf " << fb << " fc " << fc << endl;
		its++;
		if(its>max_iterations)
		{
			stringstream ss;
			ss << "Max iteration exceeded (" << max_iterations << ") in bracketing";
			done=true;
			message=ss.str();
			break;
		}
	}
	if(fabs(c-b)<(b-a))
	{
		swap(a,c);
		swap(fa,fc);
	}
	minimum_value=fb;
	minimum_param=b;
}



void Golden_Mean_Minimizer::minimizer_step()
{
//	cout << "a " << a << " b " << b << " c " << c << endl;
//	cout << "fa " << fa << " bf " << fb << " fc " << fc << endl;

	double d=b+GOLDEN*(c-b);
	double fd=func->evaluate(d);
	if(fd<minimum_value) {minimum_param=d; minimum_value=fd;};
	if(fd<fb)
	{
		swap(fa,fb);
		swap(a,b);
		swap(fd,fb);
		swap(d,b);
	} else
	{
		swap(fc,fd);
		swap(c,d);
	}
	if(fabs(c-b)<fabs(b-a))
	{
		swap(a,c);
		swap(fa,fc);
	}
	if(fabs(a-c)<accuracy)
	{
		success=true;
		done=true;
		message="Desired Accuracy Achieved";
	} else
	if((fabs(fa-fb)<epsilon)&&(fabs(fb-fc)<epsilon))
	{
		success=true;
		done=true;
		stringstream ss;
		ss << "Function value within epsilon (" << epsilon << ")";
		message=ss.str();
	}
}
	
double Golden_Mean_Minimizer::minimize(double aa,double bb)
{
	done=false;
	success=false;
	bracket_minimum(aa,bb);
	size_t nsteps=0;
	while(!isDone())
	{
		minimizer_step();
		nsteps++;
		if(nsteps>max_iterations)
		{
			done=true;
			stringstream ss;
			ss << "Max iteration exceeded (" << max_iterations << ")";
			message=ss.str();
			break;
		}
	}
	return fb;
}

double Golden_Mean_Minimizer::minimize_onesided(double minparam,double bb)
{
	success=false;
	done=false;
	bracket_minimum_onesided(minparam,bb);
	size_t nsteps=0;
	while(!isDone())
	{
		minimizer_step();
		nsteps++;
		if(nsteps>max_iterations)
		{
			done=true;
			stringstream ss;
			ss << "Max iteration exceeded (" << max_iterations << ")";
			message=ss.str();
			break;
		}
	}
	return fb;
}

void Golden_Mean_Minimizer::initialize_bracket(double minval,double maxval)
{
	a=minval;
	c=maxval;
	b=minval+(maxval-minval)*GOLDEN;
	fa=func->evaluate(a);
	fb=func->evaluate(b);
	fc=func->evaluate(c);
	if((fa<fb)&&(fa<fc)) {minimum_value=fa; minimum_param=a;}
	else if(fb<fc) {minimum_value=fb; minimum_param=b;}
	else {minimum_value=fc; minimum_param=c;}
}
	
double Golden_Mean_Minimizer::minimize_between(double minval,double maxval)
{
	done=false;
	success=false;
	initialize_bracket(minval,maxval);
	size_t nsteps=0;
	while(!isDone())
	{
		minimizer_step();
		nsteps++;
		if(nsteps>max_iterations)
		{
			done=true;
			stringstream ss;
			ss << "Max iteration exceeded (" << max_iterations << ")";
			message=ss.str();
			break;
		}
	}
	return fb;
}
	
string Golden_Mean_Minimizer::getInfo()
{
	stringstream ss;
	ss << "-----Golden_Mean_Minimizer_Status-----" << endl;
	ss << " a: " << a << "  b: " << b << "  c: " << c << endl;
	ss << "fa: " << a << " fb: " << fb << " fc: " << fc << endl;
	ss << "Done? " << isDone() << " Success? " << getSuccess() << endl;
	ss << "Minimum Parameter: " << minimum_param << "  with value " << minimum_value << endl;
	ss << "Parameter Range: " << a << " -- " << c << endl;
	ss << "Value Range: " << fa << " -- " << fc << endl;
	ss << "Accuracy: " << accuracy << " Epsilon " << epsilon << endl;
	ss << "Message: " << message << endl;
	ss << "--------------------------------------" << endl;
	return ss.str();
}
