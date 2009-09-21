CC=gcc
CFLAGS=-Wall -O2 -g `root-config --cflags`
LIBS=`root-config --libs` -lRooFit -lRooFitCore -lMinuit

all : LandauGauss.lib bgOnly sigOnly multiDlikelihood.C
	$(CC) $(CFLAGS) $(LIBS) -o multiD multiDlikelihood.C LandauGauss.lib

LandauGauss.lib : LandauGauss.o
	$(CC) $(CFLAGS) $(LIBS) -shared -o LandauGauss.lib LandauGaussDict.C LandauGauss.o 

LandauGauss.o : LandauGaussDict.C
	$(CC) $(CFLAGS) $(LIBS) -fPIC -c LandauGauss.cxx 

LandauGaussDict.C : LandauGauss.h LandauGauss.cxx
	rootcint -f LandauGaussDict.C -c LandauGauss.h

clean : 
	-rm LandauGaussDict.* LandauGauss.lib LandauGauss.o multiD


bgOnly: LandauGauss.lib multiDlikelihoodBackgroundOnly.C
	$(CC) $(CFLAGS) $(LIBS) -o multiDback multiDlikelihoodBackgroundOnly.C LandauGauss.lib

sigOnly: multiDlikelihoodSignalOnly.C
	$(CC) $(CFLAGS) $(LIBS) -o multiDsig multiDlikelihoodSignalOnly.C
