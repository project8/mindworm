#include "Monarch.hpp"
#include <fftw3.h>
#include <string>
#include <unistd.h>
#include <strings.h>
using namespace std;
#define uint unsigned int

//----------Knobs to turn---------------
//size of each fft'd autocorrelation
unsigned int fft_input_size=32768;
string input_eggname;


//------------
void print_usage(); //print out instructions
int handle_options(int argc,char *argv[]); //command line options

//------
//This produces a number of power spectra from a record
//fun fact: power spectrum is just the autocorrelation in freq. space
class PowerSpectrumProducer
{
public:
    //set up variables, initialize arrays
    void init(const Monarch *egg,uint myfftsize);
    //fill internal data with info from current record
    void process(const Monarch *egg);
    //clean up arrays
    ~PowerSpectrumProducer();

    float *fft_input;
    fftwf_complex *fft_output;
    float *power_spectra;
    fftwf_plan fft_plan;
    uint fft_input_size;
    uint fft_output_size;
    uint nffts_per_event;
    uint record_size;
    uint sampling_rate_mhz;
};

class PowerSpectrumToTimeSpace
{
public:
    ~PowerSpectrumToTimeSpace();

    void init(PowerSpectrumProducer *psp);
    void process(PowerSpectrumProducer *psp);

    fftwf_plan fft_plan;
    uint size;
    uint nffts_per_event;
    float *timespacedata;

    double *timespace_sum;
    uint sum_counts;
};

int main(int argc,char *argv[])
{
    //handle command line options
    int onindex;
    if(((onindex=handle_options(argc,argv))==-1)||(argc-onindex<0))
	{
		print_usage();
		return -1;
	}

    //--load the egg file-- prep arrays
    const Monarch *egg=Monarch::OpenForReading(std::string(input_eggname));
    if(egg==NULL) {cerr << "couldn't load egg file" << endl; return -1;}
    egg->ReadHeader();
    PowerSpectrumProducer power_producer;
    power_producer.init(egg,fft_input_size);

    PowerSpectrumToTimeSpace timespace_maker;
    timespace_maker.init(&power_producer);

    //--loop over all the records--
    while(egg->ReadRecord()) {
        //make power spectra
        power_producer.process(egg);
        //make time space autocorrelation
        timespace_maker.process(&power_producer);
    }

    //output result
    for(uint i=0;i<timespace_maker.size;i++) {
        cout << i << " " << timespace_maker.timespace_sum[i]/((double)timespace_maker.sum_counts) << endl;
    }

}

int handle_options(int argc,char *argv[])
{
	int c;
    const char *okopts="hi:";
    while((c=getopt(argc,argv,okopts))!=-1)
	switch(c)
	{
	    case 'i':
		input_eggname=string(optarg);
		break;
	    case 'h':
		print_usage();
		return -1;
		case '?':
			if(index(okopts,optopt)==NULL)
				fprintf(stderr,"{ error: \"unknown option: %c\n, aborting\"}",optopt);
			else
				fprintf(stderr,"{ error: \"option %c does not take an argument, aborting\"}",optopt);
			return -1;

	}
	return optind;
}

void print_usage()
{
    cout << "this program is well documented" << endl;
}

PowerSpectrumProducer::~PowerSpectrumProducer()
{
    fftwf_free(fft_input);
    fftwf_free(fft_output);
    fftwf_free(power_spectra);
    fftwf_destroy_plan(fft_plan);
}

void PowerSpectrumProducer::init(const Monarch *egg,uint myfftsize)
{
    fft_input_size=myfftsize;
    const MonarchHeader *eggheader=egg->GetHeader();
    sampling_rate_mhz=eggheader->GetAcquisitionRate();
    record_size=eggheader->GetRecordSize();
	//decide the optimal size for ffts and allocate memory
    if(record_size<fft_input_size) {
    	fprintf(stderr,"fft size larger than record size.  make fft size smaller. aborting");
    }
    nffts_per_event=record_size/fft_input_size;
    fft_output_size=fft_input_size/2+1;
    //allocate the memory
    fft_input=(float*)fftwf_malloc(sizeof(float)*nffts_per_event*fft_input_size);
    fft_output=(fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*fft_output_size*nffts_per_event);
    power_spectra=(float*)fftwf_malloc(sizeof(float)*nffts_per_event*fft_output_size);

    //create the fft plan
    int fft_input_size_i=fft_input_size;
    fft_plan=fftwf_plan_many_dft_r2c(1,&fft_input_size_i,nffts_per_event,fft_input,NULL,1,fft_input_size,fft_output,NULL,1,fft_output_size,FFTW_ESTIMATE);
}

void PowerSpectrumProducer::process(const Monarch *egg)
{
    const MonarchRecord *event=egg->GetRecordSeparateOne();
    //convert data to floats
   for(uint i=0;i<record_size;i++)
       fft_input[i]=(float)(event->fData[i])-128.0;
   //perform the fft
   fftwf_execute(fft_plan);
   //calculate the autocorrelation
   for(uint i=0;i<nffts_per_event;i++)
       for(uint j=0;j<fft_output_size;j++) {
	   uint k=i*fft_output_size+j;
	   power_spectra[k]=fft_output[k][0]*fft_output[k][0]+fft_output[k][1]*fft_output[k][1];
       }
   //done.  user can extract values from power spectra themself
}

PowerSpectrumToTimeSpace::~PowerSpectrumToTimeSpace()
{
    fftwf_free(timespacedata);
    fftwf_destroy_plan(fft_plan);
    delete timespace_sum;
}

void PowerSpectrumToTimeSpace::init(PowerSpectrumProducer *psp)
{
    size=psp->fft_output_size;
    nffts_per_event=psp->nffts_per_event;
    int sz=size;
    fftwf_r2r_kind kind=FFTW_REDFT10;
    timespacedata=(float*)fftwf_malloc(sizeof(float)*nffts_per_event*size);
    fft_plan=fftwf_plan_many_r2r(1,&sz,nffts_per_event,psp->power_spectra,NULL,1,sz,timespacedata,NULL,1,sz,&kind,FFTW_ESTIMATE);
    timespace_sum=new double[size];
    for(uint i=0;i<size;i++)
        timespace_sum[i]=0;
    sum_counts=0;
}

void PowerSpectrumToTimeSpace::process(PowerSpectrumProducer *psp)
{
    fftwf_execute(fft_plan);
    for(uint j=0;j<nffts_per_event;j++)
    for(uint i=0;i<size;i++) {
        timespace_sum[i]+=timespacedata[j*size+i];
        sum_counts++;
    }
}
