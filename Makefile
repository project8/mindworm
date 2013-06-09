#MONARCHLOC := /usr/local
MONARCHLOC := /home/grybka/projects/p8/monarch/cbuild
CFLAGS := -Wall -I $(MONARCHLOC)/include -O3
LIBS := -L$(MONARCHLOC)/lib -lMonarchCore -lfftw3f -lfftw3f_threads -lpthread -lm -lprotobuf -lyajl


all: autocorrelation_search

autocorrelation_search: autocorrelation_search.cc
	g++ $(CFLAGS) -c -o autocorrelation_search.o autocorrelation_search.cc
	g++ $(CFLAGS) -o autocorrelation_search autocorrelation_search.o $(LIBS)

powerline_elektronjager: powerline_elektronjager.cc Waterfall.o Histogram.o correline_utils.o
	g++ $(CFLAGS) -c powerline_elektronjager.cc -o powerline_elektronjager.o
	g++ $(CFLAGS) -o powerline_elektronjager  $(LIBS) powerline_elektronjager.o Waterfall.o Histogram.o correline_utils.o

Waterfall.o: Waterfall.cc Waterfall.hh
	g++ $(CFLAGS) -c Waterfall.cc -o Waterfall.o

correline_utils.o: correline_utils.cc correline_utils.hh
	g++ $(CFLAGS) -c correline_utils.cc -o correline_utils.o

correline: correline.cc Waterfall.o
	g++ $(CFLAGS) -c correline.cc -o correline.o
	g++ $(CFLAGS) -o correline  $(LIBS) correline.o Waterfall.o

Histogram.o: Histogram.cc Histogram.hh
	g++ $(CFLAGS) -c Histogram.cc -o Histogram.o

Minimization1d.o: Minimization1d.cc Minimization1d.hh
	g++ $(CFLAGS) -c Minimization1d.cc -o Minimization1d.o

fit_histo: fit_histo.cc Minimization1d.o Histogram.o
	g++ $(CFLAGS) -c fit_histo.cc -o fit_histo.o
	g++ $(CFLAGS) -o fit_histo fit_histo.o Minimization1d.o Histogram.o


clean:
	rm -f *.o
	rm -f fit_histo
	rm -f autocorrelation_search
