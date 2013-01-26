#!/usr/bin/env python
#calculates the log likelyhood of producing a histogram from a 
#chi square distribution
#1) for each bin calculate most likely number of counts as A*exp(-x)
#2) calculate likelyhood assuming poisson statistics in that bin

import sys
import math

def load_histo(filename):
	retval={'x':[],'y':[]}
	thefile=open(filename,"r").readlines()
	for line in thefile:
		elems=line.split(' ')
		retval['x'].append(elems[0])
		retval['y'].append(elems[1])
	return retval

def sumhisto(histo):
	total=0
	for i in range(len(histo['y'])):
		total=total+float(histo['y'][i])
	return total

def pchisq2(A,x):
	return float(A)*math.exp(-float(x))

def main():
	histo=load_histo(sys.argv[1])
	binsize=float(histo['x'][1])-float(histo['x'][0])
	total=sumhisto(histo)
	llsum=0
	for i in range(len(histo['y'])):
		m=binsize*pchisq2(total,histo['x'][i])
		n=float(histo['y'][i])
		p=n*math.log(m)-m-math.lgamma(n+1)
		print i,p
		llsum=llsum+p
	result=llsum/float(len(histo['y']))
	print -result


if __name__=='__main__':
        main()

