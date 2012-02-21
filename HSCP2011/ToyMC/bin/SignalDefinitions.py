class SignalSample:
  def __init__(self,name,crossSection,ptCut,iasCut,massCut,file):
    self.name = name
    self.crossSection = crossSection
    self.ptCut = ptCut
    self.iasCut = iasCut
    self.massCut = massCut
    self.file = file



# Gluinos
Gluino300 = SignalSample('Gluino300','65.8',60,'0.400',180,'file:/hdfs/cms/.../')
Gluino400 = SignalSample('Gluino400','11.2',80,'0.400',230,'')
Gluino500 = SignalSample('Gluino500','2.54',50,'0.375',290,'')
Gluino600 = SignalSample('Gluino600','0.693',50,'0.325',360,'')
Gluino700 = SignalSample('Gluino700','0.214',50,'0.300',410,'')
Gluino800 = SignalSample('Gluino800','0.0725',50,'0.275',460,'')
Gluino900 = SignalSample('Gluino900','0.0262',100,'0.250',500,'')
Gluino1000 = SignalSample('Gluino1000','0.00987',120,'0.225',540,'')
Gluino1100 = SignalSample('Gluino1100','0.00386',120,'0.225',570,'')
Gluino1200 = SignalSample('Gluino1200','0.00154',155,'0.200',600,'')
# GluinoN
GluinoN300 = SignalSample('Gluino300','65.8',60,'0.400',180,'')
GluinoN400 = SignalSample('Gluino400','11.2',85,'0.400',230,'')
GluinoN500 = SignalSample('Gluino500','2.54',50,'0.375',290,'')
GluinoN600 = SignalSample('Gluino600','0.693',50,'0.350',330,'')
GluinoN700 = SignalSample('Gluino700','0.214',50,'0.325',270,'')
GluinoN800 = SignalSample('Gluino800','0.0725',50,'0.300',400,'')
GluinoN900 = SignalSample('Gluino900','0.0262',95,'0.300',380,'')
GluinoN1000 = SignalSample('Gluino1000','0.00987',50,'0.300',410,'')
GluinoN1100 = SignalSample('Gluino1100','0.00386',50,'0.275',460,'')
GluinoN1200 = SignalSample('Gluino1200','0.00154',50,'0.300',400,'')
# Stop
Stop130 = SignalSample('Stop130','120.0',70,'0.400',70,'')
Stop200 = SignalSample('Stop200','13.0',60,'0.400',130,'')
Stop300 = SignalSample('Stop300','1.31',70,'0.400',190,'')
Stop400 = SignalSample('Stop400','0.218',65,'0.375',250,'')
Stop500 = SignalSample('Stop500','0.0478',50,'0.350',310,'')
Stop600 = SignalSample('Stop600','0.0125',50,'0.325',360,'')
Stop700 = SignalSample('Stop700','0.00356',50,'0.300',410,'')
Stop800 = SignalSample('Stop800','0.00114',50,'0.275',450,'')
# StopN
StopN130 = SignalSample('StopN130','120.0',70,'0.400',70,'')
StopN200 = SignalSample('StopN200','13.0',70,'0.400',120,'')
StopN300 = SignalSample('StopN300','1.31',70,'0.400',170,'')
StopN400 = SignalSample('StopN400','0.218',60,'0.400',240,'')
StopN500 = SignalSample('StopN500','0.0478',50,'0.375',280,'')
StopN600 = SignalSample('StopN600','0.0125',50,'0.350',320,'')
StopN700 = SignalSample('StopN700','0.00356',50,'0.325',350,'')
StopN800 = SignalSample('StopN800','0.00114',50,'0.325',370,'')
# GMSB Stau
GMStau100 = SignalSample('GMStau100','1.3398',65,'0.400',20,'')
GMStau126 = SignalSample('GMStau126','0.274591',70,'0.400',50,'')
GMStau156 = SignalSample('GMStau156','0.0645953',85,'0.400',80,'')
GMStau200 = SignalSample('GMStau200','0.0118093',70,'0.400',120,'')
GMStau247 = SignalSample('GMStau247','0.00342512',60,'0.400',150,'')
GMStau308 = SignalSample('GMStau308','0.00098447',70,'0.400',190,'')
GMStau370 = SignalSample('GMStau370','0.000353388',60,'0.400',230,'')
GMStau432 = SignalSample('GMStau432','0.000141817',55,'0.375',260,'')
GMStau494 = SignalSample('GMStau494','0.00006177',65,'0.350',300,'')
# PP Stau
PPStau100 = SignalSample('PPStau100','0.0382',70,'0.400',40,'')
PPStau126 = SignalSample('PPStau126','0.0161',80,'0.400',60,'')
PPStau156 = SignalSample('PPStau156','0.00704',85,'0.400',80,'')
PPStau200 = SignalSample('PPStau200','0.00247',120,'0.400',120,'')
PPStau247 = SignalSample('PPStau247','0.00101',110,'0.400',150,'')
PPStau308 = SignalSample('PPStau308','0.000353',70,'0.400',190,'')
# DiChamps - TODO
