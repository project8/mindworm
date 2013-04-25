#include "Histogram.hh"
#include <stdio.h>
#include <iostream>
#include <math.h>

Histogram::Histogram(int sz,float mmin,float mmax)
{
	init(sz,mmin,mmax);
}

void Histogram::init(int sz,float mmin,float mmax)
{
    size=sz;
    min=mmin;
    max=mmax;
    data=new double[size];
    for(int i=0;i<size;i++)
		data[i]=0;
}

Histogram::~Histogram()
{
    delete data;
}

void Histogram::saveToFile(string fname)
{
    FILE *f=fopen(fname.c_str(),"w");
    for(int i=0;i<size;i++) {
	float x=min+ (max-min)*((double)i)/((double)size);
	fprintf(f,"%f %g\n",x,data[i]);
    }
    fclose(f);
}

void Histogram::loadFromFile(string fname)
{
    FILE *f=fopen(fname.c_str(),"r");
	char buffer[512];
	int linecount=0;
	float fmin=0;
	float fmax=0;
	float a,b;
	//step one, count how big the damne thing is
	while(fgets(buffer,512,f)!=NULL) {
		if(buffer[0]!='#')  {
			fscanf(f,"%g %g",&a,&b);
			if(linecount==0) {
				fmin=a;
			}
			fmax=a;
			linecount++;
		}
	}
	rewind(f);
	init(linecount,fmin,fmax);
	int online=0;
	while(fgets(buffer,512,f)!=NULL) {
		if(buffer[0]!='#')  {
			fscanf(f,"%g %g",&a,&b);
//			cout << a << " " << b << endl;
			data[online]=b;
		}
		online++;
	}
}
    
void Histogram::increment(float x)
{
    int xx=(int)(((float)size)*((x-min)/(max-min)));
    if(xx<0) xx=0;
    if(xx>=size) xx=size-1;
    data[xx]+=1.0;
}

double Histogram::getx(int bin) 
{
    return min+(max-min)*((double)bin)/((double)size);
}
	
double Histogram::getbin(double x)
{
	return (int)(((double)size)*(x-min)/(max-min));
}

double Histogram::calc_mean()
{
    double sum=0;
    double x=0;
    for(int i=0;i<size;i++) {
	sum+=data[i];
	x+=getx(i)*data[i];
    }
    return x/sum;
}

double Histogram::calc_sigma() 
{
    double sum=0;
    double x=0;
    double xx=0;
    for(int i=0;i<size;i++) {
	sum+=data[i];
	x+=getx(i)*data[i];
	xx+=getx(i)*getx(i)*data[i];
    }
    double m=x/sum;
    double s=xx/sum;
    return sqrt(s-m*m);
}

