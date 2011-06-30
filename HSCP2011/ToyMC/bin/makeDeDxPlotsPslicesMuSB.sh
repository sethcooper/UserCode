#!/bin/bash

usage='Usage: makeDeDxPlotsPslicesMuSB.sh -f fileNamei -d directoryName'
  
args=`getopt fd: -- "$@"`
if test $? != 0
     then
         echo $usage
         exit 1
fi

eval set -- "$args"
for i
  do
  case "$i" in
      -f) shift; file_name=$2;shift;;
      -d) shift; dir_name=$2;shift;;
  esac
done

if [ "X"${file_name} == "X" ]
    then
    echo "invalid root file name! Please give a valid one."
    echo $usage
    exit
fi
if [ "X"${dir_name} == "X" ]
    then
    echo "invalid directory name! Please give a valid one."
    echo $usage
    exit
fi

myPwd=`pwd`
mkdir -p ${dir_name}
cd ${dir_name}

# run root in batch
root -b $myPwd/$file_name <<!
.x $myPwd/makeDeDxPlotsPslicesMuSB.C
.q
!


cat > index.html <<EOF

<HTML>

<HEAD><TITLE>dE/dx Plots</TITLE></HEAD>
 
<BODY link="Red">
<FONT color="Black">
 
<Center>
<h2><FONT color="Blue"> TK E1 dE/dx distributions </FONT></h2>
</Center>

<table>
<tr>
<td></td>
<td>NoM 1-5</td>
<td>NoM 6-10</td>
<td>NoM 11-15</td>
<td>NoM 16-20</td>
<td>NoM 21+</td>
</tr>
<tr>
<td>P 15-50 GeV</td>
<td><a href="histForPDFNoMPBinsMuSBIndex1.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex1.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex2.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex2.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex3.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex3.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex4.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex4.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex5.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex5.png"></a></td>
</tr>
<tr>
<td>P 50-150 GeV</td>
<td><a href="histForPDFNoMPBinsMuSBIndex6.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex6.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex7.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex7.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex8.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex8.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex9.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex9.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex10.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex10.png"></a></td>
</tr>
<tr>
<td>P 150-300 GeV</td>
<td><a href="histForPDFNoMPBinsMuSBIndex11.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex11.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex12.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex12.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex13.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex13.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex14.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex14.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex15.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex15.png"></a></td>
</tr>
<tr>
<td>P 300-450 GeV</td>
<td><a href="histForPDFNoMPBinsMuSBIndex16.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex16.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex17.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex17.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex18.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex18.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex19.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex19.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex20.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex20.png"></a></td>
</tr>
<tr>
<td>P > 450 GeV</td>
<td><a href="histForPDFNoMPBinsMuSBIndex21.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex21.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex22.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex22.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex23.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex23.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex24.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex24.png"></a></td>
<td><a href="histForPDFNoMPBinsMuSBIndex25.png"><img height="200" src="histForPDFNoMPBinsMuSBIndex25.png"></a></td>
</tr>
</table>

</FONT>
</BODY>
</HTML>

EOF


exit

