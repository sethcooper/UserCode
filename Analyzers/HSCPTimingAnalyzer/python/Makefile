CC=gcc
CFLAGS=-Wall -O2 -g `root-config --cflags`
LIBS=`root-config --libs` -lRooFit -lRooFitCore -lMinuit -lFoam

#all : bgOnly sigOnly full
all : full

clean : 
	-rm multiD multiDback multiDsig

bgOnly : multiDlikelihoodBackgroundOnly.C
	$(CC) $(CFLAGS) $(LIBS) -o multiDback multiDlikelihoodBackgroundOnly.C 

sigOnly : multiDlikelihoodSignalOnly.C
	$(CC) $(CFLAGS) $(LIBS) -o multiDsig multiDlikelihoodSignalOnly.C

full : multiDlikelihood.C
	$(CC) $(CFLAGS) $(LIBS) -o multiD multiDlikelihood.C

