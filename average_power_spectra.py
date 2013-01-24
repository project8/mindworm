#!/usr/bin/env python
import sys

def load_powerspec(filename):
	retval={'f':[],'p1':[],'p2':[],'s1':[],'s2':[],'comments':0}
	myfile=open(filename,"r")
	thefile=myfile.readlines()
	for line in thefile:
		elems=line.split(' ')
		if elems[0][0]=='#':
			retval['comments']=retval['comments']+1
			continue
		retval['f'].append(float(elems[0]));
		retval['p1'].append(float(elems[1]));
		retval['p2'].append(float(elems[2]));
		retval['s1'].append(float(elems[3]));
		retval['s2'].append(float(elems[4]));
	myfile.close()
	return retval

def normalize_spectrum(spectrum):
	sum1=0
	sum2=0
	for i in range(len(spectrum['f'])):
		if i==0:
			continue; #first bin is just offset, ignore
		sum1+=spectrum['p1'][i]
		sum2+=spectrum['p2'][i]
	sum1=sum1/float(len(spectrum['f']))
	sum2=sum2/float(len(spectrum['f']))
	for i in range(len(spectrum['f'])):
		spectrum['p1'][i]=spectrum['p1'][i]/sum1
		spectrum['p2'][i]=spectrum['p2'][i]/sum2
		

def main():
	#open the file that has the files
	listfile=open(sys.argv[1]).readlines()
	on_first=True
	for line in listfile:
		#load the spectrum from the file
		pspec=load_powerspec(line.rstrip())
		#normalize it
		normalize_spectrum(pspec)
		#and sum
		if on_first:
			specsum=pspec
			on_first=False
		else:
			for i in range(len(pspec['f'])):
				specsum['p1'][i]=specsum['p1'][i]+pspec['p1'][i]
				specsum['p2'][i]=specsum['p2'][i]+pspec['p2'][i]
	for i in range(specsum['comments']):
		print "#comment line"
	for i in range(len(specsum['f'])):
		print specsum['f'][i],specsum['p1'][i],specsum['p2'][i]

if __name__=='__main__':
        main()


