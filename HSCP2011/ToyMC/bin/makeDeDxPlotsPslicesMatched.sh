#!/bin/bash

usage='Usage: makeDeDxPlotsPslicesMatched.sh -f fileNamei -d directoryName'
  
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
.x $myPwd/makeDeDxPlotsPslicesMatched.C
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
<td><a href="histForPDFNoMPBinsMatchedIndex1.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex1.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex2.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex2.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex3.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex3.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex4.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex4.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex5.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex5.png"></a></td>
</tr>
<tr>
<td>P 50-150 GeV</td>
<td><a href="histForPDFNoMPBinsMatchedIndex6.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex6.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex7.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex7.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex8.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex8.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex9.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex9.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex10.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex10.png"></a></td>
</tr>
<tr>
<td>P 150-300 GeV</td>
<td><a href="histForPDFNoMPBinsMatchedIndex11.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex11.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex12.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex12.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex13.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex13.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex14.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex14.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex15.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex15.png"></a></td>
</tr>
<tr>
<td>P 300-450 GeV</td>
<td><a href="histForPDFNoMPBinsMatchedIndex16.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex16.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex17.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex17.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex18.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex18.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex19.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex19.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex20.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex20.png"></a></td>
</tr>
<tr>
<td>P > 450 GeV</td>
<td><a href="histForPDFNoMPBinsMatchedIndex21.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex21.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex22.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex22.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex23.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex23.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex24.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex24.png"></a></td>
<td><a href="histForPDFNoMPBinsMatchedIndex25.png"><img height="200" src="histForPDFNoMPBinsMatchedIndex25.png"></a></td>
</tr>
</table>

</FONT>
</BODY>
</HTML>

EOF


exit

