#!/bin/bash

usage='Usage: makeDeDxPlotsPslices.sh -f fileNamei -d directoryName'
  
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
.x $myPwd/makeDeDxPlotsPslices.C
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
<td><a href="histForPDFNoMPBinsIndex1.png"><img height="200" src="histForPDFNoMPBinsIndex1.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex2.png"><img height="200" src="histForPDFNoMPBinsIndex2.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex3.png"><img height="200" src="histForPDFNoMPBinsIndex3.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex4.png"><img height="200" src="histForPDFNoMPBinsIndex4.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex5.png"><img height="200" src="histForPDFNoMPBinsIndex5.png"></a></td>
</tr>
<tr>
<td>P 50-150 GeV</td>
<td><a href="histForPDFNoMPBinsIndex6.png"><img height="200" src="histForPDFNoMPBinsIndex6.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex7.png"><img height="200" src="histForPDFNoMPBinsIndex7.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex8.png"><img height="200" src="histForPDFNoMPBinsIndex8.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex9.png"><img height="200" src="histForPDFNoMPBinsIndex9.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex10.png"><img height="200" src="histForPDFNoMPBinsIndex10.png"></a></td>
</tr>
<tr>
<td>P 150-300 GeV</td>
<td><a href="histForPDFNoMPBinsIndex11.png"><img height="200" src="histForPDFNoMPBinsIndex11.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex12.png"><img height="200" src="histForPDFNoMPBinsIndex12.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex13.png"><img height="200" src="histForPDFNoMPBinsIndex13.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex14.png"><img height="200" src="histForPDFNoMPBinsIndex14.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex15.png"><img height="200" src="histForPDFNoMPBinsIndex15.png"></a></td>
</tr>
<tr>
<td>P 300-450 GeV</td>
<td><a href="histForPDFNoMPBinsIndex16.png"><img height="200" src="histForPDFNoMPBinsIndex16.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex17.png"><img height="200" src="histForPDFNoMPBinsIndex17.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex18.png"><img height="200" src="histForPDFNoMPBinsIndex18.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex19.png"><img height="200" src="histForPDFNoMPBinsIndex19.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex20.png"><img height="200" src="histForPDFNoMPBinsIndex20.png"></a></td>
</tr>
<tr>
<td>P > 450 GeV</td>
<td><a href="histForPDFNoMPBinsIndex21.png"><img height="200" src="histForPDFNoMPBinsIndex21.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex22.png"><img height="200" src="histForPDFNoMPBinsIndex22.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex23.png"><img height="200" src="histForPDFNoMPBinsIndex23.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex24.png"><img height="200" src="histForPDFNoMPBinsIndex24.png"></a></td>
<td><a href="histForPDFNoMPBinsIndex25.png"><img height="200" src="histForPDFNoMPBinsIndex25.png"></a></td>
</tr>
</table>

</FONT>
</BODY>
</HTML>

EOF


exit

