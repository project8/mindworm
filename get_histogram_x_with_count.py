#!/usr/bin/env python
#finds the x of a histogram beyond which there are only N events

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

def main():
	histo=load_histo(sys.argv[1])
	number=float(sys.argv[2])
	sum=0
	for i in reversed(range(len(histo['y']))):
		sum+=float(histo['y'][i])
#		print histo['x'][i],histo['y'][i]
		if sum > number:
			print histo['x'][i],sum
			break


if __name__=='__main__':
        main()

