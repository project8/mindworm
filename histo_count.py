#!/usr/bin/env python
#sums total number of counts above a certain number

import sys

def load_histo(filename):
	retval={'x':[],'y':[]}
	thefile=open(filename,"r").readlines()
	for line in thefile:
		elems=line.split(' ')
		retval['x'].append(elems[0])
		retval['y'].append(elems[1])
	return retval

def main():
	histo=load_histo(sys.argv[1]);
	minnum=float(sys.argv[2]);
	total=0
	for i in range(len(histo['y'])):
		if float(histo['x'][i])>=minnum:
			total=total+float(histo['y'][i])
	print total

if __name__=='__main__':
        main()

