#include "correline_utils.hh"
#include <math.h>
#include <string.h>


//-------------Background------------

bool Background::load(string fname)
{
	ifstream fin(fname.c_str());
	if(!fin.good()) {
		cerr << "unable to open file: " << fname << endl;
		return false;
	}
	//count bg size
	bg_size=0;
	string line;
	while(getline(fin,line))
		if(line[0]!='#')
			bg_size++;
	background_avg=new fftwf_complex[bg_size];
	background_stdev=new fftwf_complex[bg_size];
	background_stdev_invert=new fftwf_complex[bg_size];
	fin.clear();
	fin.seekg(ios_base::beg);
	int on_bg=0;
	double scale=1;
	double f;
	while(getline(fin,line)) {
		if(line[0]!='#')
		{
			stringstream ss(line);
			ss >> f;
			ss >> background_avg[on_bg][0];
			ss >> background_avg[on_bg][1];
			ss >> background_stdev[on_bg][0];
			ss >> background_stdev[on_bg][1];
			on_bg++;
		} else {
//			cerr << "line: |" << line << "|" << endl;
			if(line.compare(0,7,"#scale:")==0) {
				stringstream ss(line);
				string blah;
				ss >> blah >> scale;
//				cerr << "scale is " << scale << endl;
			}
		}
	}
	fin.close();
	for(int i=0;i<bg_size;i++) {
		for(int j=0;j<2;j++)
			if(background_stdev[i][j]!=0)
				background_stdev_invert[i][j]=1/background_stdev[i][j];
			else
				background_stdev_invert[i][j]=1;
	}

	//remove normalization 
	for(int i=0;i<bg_size;i++)
		for(int j=0;j<2;j++) {
			background_avg[i][j]/=scale;
			background_stdev[i][j]/=scale;
			background_stdev_invert[i][j]*=scale;
		}
	return true;
}

Background::~Background()
{
	delete background_avg;
	delete background_stdev;
	delete background_stdev_invert;
}

//------------------ConvolutionMap-------------
int ConvolutionMap::get_t_extent()
{
	int tmax=0;
	for(int i=0;i<conv_length;i++)
		if(conv_ts[i]>tmax)
			tmax=conv_ts[i];
	return tmax;
}

int ConvolutionMap::get_f_extent()
{
	int fmax=0;
	for(int i=0;i<conv_length;i++)
		if(conv_fs[i]>fmax)
			fmax=conv_fs[i];
	return fmax;
}

bool ConvolutionMap::load(string fname) 
{
	ifstream fin(fname.c_str());
	if(!fin.good()) {
		cerr << "unable to open file: " << fname << endl;
		return false;
	}
	conv_length=0;
	string line;
	while(getline(fin,line)) {
		if(line[0]!='#')
		{
			stringstream ss(line);
			ss >> conv_ts[conv_length] >> conv_fs[conv_length];
			conv_length++;
		}
	}
	fin.close();
	return true;
}

void ConvolutionMap::convolve(Waterfall *input,Waterfall *output)
{
	float normalization=1/sqrt((float)conv_length);
	int tl=get_t_extent();
	int fl=get_f_extent();
	for(int i=0;i<input->npoints_t-tl;i++) {
		for(int j=0;j<input->npoints_f-fl;j++) {
			int oindex=output->getIndex(j,i);
			output->data[oindex][0]=0;
			output->data[oindex][1]=0;
			for(int k=0;k<conv_length;k++) {
				int index=output->getIndex(j+conv_fs[k],i+conv_ts[k]);
				output->data[oindex][0]+=input->data[index][0];
				output->data[oindex][1]+=input->data[index][1];
			}
			output->data[oindex][0]*=normalization;
			output->data[oindex][1]*=normalization;
		}
	}
}


//------------------Candidate------------

vector<Candidate> loadCandidateFile(string fname)
{
    long long npoints;
    return loadCandidateFile(fname,npoints);
}

vector<Candidate> loadCandidateFile(string fname,long long &npoints)
{
	vector<Candidate> ret;
	ifstream fin(fname.c_str());
	if(!fin.good()) {
		cerr << "error opening candidate file " << fname << endl;
	}
	string line;
	while(getline(fin,line)) {
		stringstream ls(line);
		if(line[0]=='#') {
		    if(line.compare(0,8,"#npoints")==0) {
			string dummy;
			ls >> dummy >> npoints;
		    }
		    continue;
		}
		Candidate toadd;
		ls >> toadd.frequency >> toadd.time >> toadd.magnitude >> toadd.probability;
		ret.push_back(toadd);
	}
	fin.close();
	return ret;
}

int Candidate::getEventNo(int sampling_rate_mhz,int event_size)
{
	double event_length=((double)event_size)/(sampling_rate_mhz*1e6);
	double event_frac=time/event_length;
	return (int)(floor(event_frac));
}
	

	
bool SortCandidatesByPower::operator()(const Candidate &a,const Candidate &b)
{
	if(a.magnitude<b.magnitude) return true;
	if(a.magnitude>b.magnitude) return false;
	if(a.time<b.time) return true;
	if(a.time>b.time) return false;
	return a.frequency<b.frequency;
}


void PowerCandidateList::push(const Candidate &c)
{
	insert(c);
	if(size()>max_length) erase(begin());
}
	
void PowerCandidateList::saveToFile(string fname,long long npoints)
{
	ofstream fout(fname.c_str());
	fout << "#npoints " << npoints << endl;
	for(PowerCandidateList::iterator it=begin();it!=end();it++) {
		fout << (*it).frequency << " " << (*it).time << " " << (*it).magnitude << " " <<  (*it).probability << endl;
	}
	fout.close();
}

void PowerCandidateList::saveToJSON(string fname,long long npoints,string eggname,int recordsize,double record_time,double time_uncert)
{
    yajl_gen candjson=yajl_gen_alloc(NULL);
    yajl_gen_config(candjson,yajl_gen_beautify,1);
    yajl_gen_map_open(candjson);
    make_json_string_entry(candjson,"eggname",eggname.c_str());
    make_json_integer_entry(candjson,"recordsize",recordsize);
    yajl_gen_string(candjson,(unsigned char*)("candidates"),strlen("candidates"));
    yajl_gen_array_open(candjson);
	for(PowerCandidateList::iterator it=begin();it!=end();it++) {
        yajl_gen_map_open(candjson);
        yajl_gen_string(candjson,(unsigned char*)("support"),strlen("support"));
        yajl_gen_array_open(candjson);
        int startrec=(int)((*it).time/record_time);
        int startsample=(int)(((double)recordsize)*(((*it).time-((double)startrec*record_time)))/record_time);
        int endrec=startrec;
        int endsample=startsample+(*it).duration*recordsize/record_time;
        while(endsample>recordsize) {
            endsample-=recordsize;
            endrec++;
        }
        yajl_gen_integer(candjson,startrec); //start record
        yajl_gen_integer(candjson,startsample); //start sample
        yajl_gen_integer(candjson,endrec); //end record
        yajl_gen_integer(candjson,endsample); //end sample
        yajl_gen_array_close(candjson);
        yajl_gen_string(candjson,(unsigned char*)("frequency"),strlen("frequency"));
        yajl_gen_array_open(candjson);
        yajl_gen_number_forreals(candjson,(*it).frequency); //start 
        yajl_gen_number_forreals(candjson,(*it).frequency); //end
        yajl_gen_number_forreals(candjson,0); //uncertainty
        yajl_gen_number_forreals(candjson,0); //end uncertainty
        yajl_gen_array_close(candjson);
        yajl_gen_string(candjson,(unsigned char*)("timing"),strlen("timing"));
        yajl_gen_array_open(candjson);
        yajl_gen_number_forreals(candjson,(*it).time); //start 
        yajl_gen_number_forreals(candjson,(*it).time+(*it).duration); //end
        yajl_gen_number_forreals(candjson,time_uncert); //uncertainty
        yajl_gen_number_forreals(candjson,time_uncert); //end uncertainty
        yajl_gen_array_close(candjson);


        make_json_numeric_entry(candjson,"magnitude",(*it).magnitude);
        make_json_numeric_entry(candjson,"confidence",(*it).probability);
        

//		fout << (*it).frequency << " " << (*it).time << " " << (*it).magnitude << " " <<  (*it).probability << endl;
        yajl_gen_map_close(candjson);
	}
    yajl_gen_array_close(candjson);
    yajl_gen_map_close(candjson);
    const unsigned char *buf;
    size_t len;
    yajl_gen_get_buf(candjson,&buf,&len);
    FILE *file=fopen(fname.c_str(),"w");
    fwrite(buf,1,len,file);
    fclose(file);
    yajl_gen_clear(candjson);
    yajl_gen_free(candjson);
}
	
PowerCandidateList PowerCandidateList::condenseByDistance(double freqscale,double timescale)
{
    PowerCandidateList tosearch=(*this);
	PowerCandidateList ret;
    size_t old_size=size();
    size_t new_size=size()-1;
    while(new_size!=old_size) {
        old_size=new_size;

	for(reverse_iterator it=tosearch.rbegin();it!=tosearch.rend();it++) {
        Candidate toadd=(*it);
        reverse_iterator comp=it;
        comp++;
        for(;comp!=tosearch.rend();comp++) {
			if(fabs((*it).frequency-(*comp).frequency)<freqscale)
			if(fabs((*it).time-(*comp).time)<timescale) {
                toadd.frequency=0.5*(toadd.frequency+(*comp).frequency);
                toadd.time=0.5*(toadd.time+(*comp).time);
                toadd.duration+=(*comp).duration;
                tosearch.erase(*comp);
            }
        }
        ret.insert(toadd);
	}

        new_size=size();
    }
	return ret;
}

//-----some json helper functions---
//
void make_json_string_entry(yajl_gen gen,const char *key,const char *value)
{
    yajl_gen_string(gen,(unsigned char*)key,strlen(key));
    yajl_gen_string(gen,(unsigned char*)value,strlen(value));
}

void make_json_numeric_entry(yajl_gen gen,const char *key,double value)
{
    yajl_gen_string(gen,(unsigned char*)key,strlen(key));
    yajl_gen_number_forreals(gen,value);
}

void make_json_integer_entry(yajl_gen gen,const char *key,int value)
{
    yajl_gen_string(gen,(unsigned char*)key,strlen(key));
    yajl_gen_integer(gen,value);
}
void yajl_gen_number_forreals(yajl_gen gen,double num)
{
    char buffer[256];
    sprintf(buffer,"%g",num);
    yajl_gen_number(gen,buffer,strlen(buffer));
}
