#!/usr/bin/env python

import sys
import math

def load_histo(filename):
	retval={'x':[],'y':[]}
	thefile=open(filename,"r").readlines()
	for line in thefile:
		elems=line.split(' ')
		retval['x'].append(float(elems[0]))
		retval['y'].append(float(elems[1]))
	return retval

def main():
    histo=load_histo(sys.argv[1]);
    startx=float(sys.argv[2]);
    k=float(sys.argv[3]);
    A=float(sys.argv[4]);
#    print startx,"and",k,"and",A
    loglike=0
    for i in range(len(histo['y'])):
        if histo['x'][i]>=startx:
#            print A,k,histo['x'][i]
            m=A*math.exp(-k*histo['x'][i])
            n=histo['y'][i]
#            print m,n
            loglike+=n*math.log(m)-m-math.lgamma(n+1)
#	print -loglike/float(len(histo['y']))
    print -loglike

if __name__=='__main__':
        main()



