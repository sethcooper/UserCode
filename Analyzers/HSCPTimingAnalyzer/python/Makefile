CC=gcc
CFLAGS=-Wall -O2 -g `root-config --cflags`
LIBS=`root-config --libs` -lRooFit -lRooFitCore -lMinuit -lFoam
# scramv1 tool info roofitcore
#INCLUDES=-I/afs/cern.ch/cms/sw/slc4_ia32_gcc345/lcg/roofit/5.25.02-cms/include
#LIBDIR=-L/afs/cern.ch/cms/sw/slc4_ia32_gcc345/lcg/roofit/5.25.02-cms/lib

#all : bgOnly sigOnly full
all : full trivial fitInput toyStudies

clean : 
	-rm multiD multiDback multiDsig

bgOnly : multiDlikelihoodBackgroundOnly.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiDback multiDlikelihoodBackgroundOnly.C 

sigOnly : multiDlikelihoodSignalOnly.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiDsig multiDlikelihoodSignalOnly.C

trivial : multiDlikelihood_trivial.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiDtrivial multiDlikelihood_trivial.C

<<<<<<< Makefile
fitInput : multiDlikelihood_fitInput.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiDfitInput multiDlikelihood_fitInput.C

toyStudies : multiDlikelihood_toyStudies.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiDtoyStudies multiDlikelihood_toyStudies.C

=======
toyStudies : multiDlikelihood_toyStudies.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiDtoyStudies multiDlikelihood_toyStudies.C

>>>>>>> 1.6
full : multiDlikelihood.C
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) $(LIBDIR) -o multiD multiDlikelihood.C

