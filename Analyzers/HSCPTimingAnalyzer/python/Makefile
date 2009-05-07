all : LandauGauss.lib bgOnly sigOnly multiDlikelihood.C
	gcc -Wall -o multiD -g `root-config --libs --cflags` -lRooFit -lRooFitCore multiDlikelihood.C LandauGauss.lib

LandauGauss.lib : LandauGauss.o
	gcc -shared -g -o LandauGauss.lib LandauGaussDict.C LandauGauss.o `root-config --libs --cflags` -lRooFit -lRooFitCore -lMinuit

LandauGauss.o : LandauGaussDict.C
	gcc -fPIC -g -c LandauGauss.cxx `root-config --cflags --libs` -lRooFit -lRooFitCore -lMinuit

LandauGaussDict.C : LandauGauss.h LandauGauss.cxx
	rootcint -f LandauGaussDict.C -c LandauGauss.h

clean : 
	-rm LandauGaussDict.* LandauGauss.lib LandauGauss.o multiD


bgOnly: LandauGauss.lib multiDlikelihoodBackgroundOnly.C
	gcc -Wall -o multiDback -g `root-config --libs --cflags` -lRooFit -lRooFitCore multiDlikelihoodBackgroundOnly.C LandauGauss.lib

sigOnly: multiDlikelihoodSignalOnly.C
	gcc -Wall -o multiDsig -g `root-config --libs --cflags` -lRooFit -lRooFitCore multiDlikelihoodSignalOnly.C
