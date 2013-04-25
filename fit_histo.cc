#include "Histogram.hh"
#include "Minimization1d.hh"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

class ExpLogLike : public SingleParameterFunction
{
public:
	double evaluate(const double &param);

	double getA(const double &param);
	int hstart;
	Histogram thehisto;
};

int main(int argc,char *argv[])
{
	ExpLogLike myfunc;
	myfunc.thehisto.loadFromFile(string(argv[1]));
	double thecut=atof(argv[2]);
//	cout << "size " << myfunc.thehisto.size << endl;
//	cout << "min " << myfunc.thehisto.min << endl;
//	cout << "max " << myfunc.thehisto.max << endl;
//	cout << "the cut is " << thecut << endl;
	myfunc.hstart=myfunc.thehisto.getbin(thecut);
//	cout << "hstart is " << myfunc.hstart << endl;
//	cout << "ll is " << myfunc.evaluate(-1) << endl;
	
	Golden_Mean_Minimizer minimizer(&myfunc);
	minimizer.accuracy=0.001;
	minimizer.epsilon=0.00001;
	minimizer.minimize_between(0,10);
	printf("%g %g\n",minimizer.minimum_param,myfunc.getA(minimizer.minimum_param));
}
	
double ExpLogLike::getA(const double &param)
{
	double sum=0;
	double sumn=0;
	for(int i=hstart;i<thehisto.size;i++) {
		double x=thehisto.getx(i);
		sum+=thehisto.data[i];
		sumn+=exp(-param*x);
	}
	return sum/sumn;
}

double ExpLogLike::evaluate(const double &param)
{
	double A=getA(param);
	double k=param;
	double ll=0;
	for(int i=hstart;i<thehisto.size;i++) {
		double x=thehisto.getx(i);
		double lambda=A*exp(-x*k);
		double lla=thehisto.data[i]*log(lambda)-lambda-lgamma(thehisto.data[i]+1);
//		cout << x << " " << lambda << " " << lla << endl;
		ll+=lla;
	}
//	cout << param << " " << -ll << endl;
	return -ll;
}
