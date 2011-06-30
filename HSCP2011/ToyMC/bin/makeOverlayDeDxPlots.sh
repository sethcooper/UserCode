#!/bin/bash

usage='Usage: makeDeDxPlots.sh -b fileNameBackground -s fileNameSignal -d directoryName'
  
args=`getopt bsd: -- "$@"`
if test $? != 0
     then
         echo $usage
         exit 1
fi

eval set -- "$args"
for i
  do
  case "$i" in
      -b) shift; back_name=$2;shift;;
      -s) shift; sig_name=$2;shift;;
      -d) shift; dir_name=$2;shift;;
  esac
done

if [ "X"${back_name} == "X" ]
    then
    echo "invalid background root file name! Please give a valid one."
    echo $usage
    exit
fi
if [ "X"${sig_name} == "X" ]
    then
    echo "invalid signal root file name! Please give a valid one."
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

# run root in batch -- make overlay plots
root -b $myPwd/$back_name $myPwd/$sig_name <<!
.x $myPwd/makeOverlayDeDxPlots.C
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
<td>Pt 15-50 GeV</td>
<td><a href="histForPDFNoMPtOverlayIndex1.png"><img height="200" src="histForPDFNoMPtOverlayIndex1.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex2.png"><img height="200" src="histForPDFNoMPtOverlayIndex2.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex3.png"><img height="200" src="histForPDFNoMPtOverlayIndex3.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex4.png"><img height="200" src="histForPDFNoMPtOverlayIndex4.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex5.png"><img height="200" src="histForPDFNoMPtOverlayIndex5.png"></a></td>
</tr>
<tr>
<td>Pt 50-150 GeV</td>
<td><a href="histForPDFNoMPtOverlayIndex6.png"><img height="200" src="histForPDFNoMPtOverlayIndex6.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex7.png"><img height="200" src="histForPDFNoMPtOverlayIndex7.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex8.png"><img height="200" src="histForPDFNoMPtOverlayIndex8.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex9.png"><img height="200" src="histForPDFNoMPtOverlayIndex9.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex10.png"><img height="200" src="histForPDFNoMPtOverlayIndex10.png"></a></td>
</tr>
<tr>
<td>Pt 150-300 GeV</td>
<td><a href="histForPDFNoMPtOverlayIndex11.png"><img height="200" src="histForPDFNoMPtOverlayIndex11.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex12.png"><img height="200" src="histForPDFNoMPtOverlayIndex12.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex13.png"><img height="200" src="histForPDFNoMPtOverlayIndex13.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex14.png"><img height="200" src="histForPDFNoMPtOverlayIndex14.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex15.png"><img height="200" src="histForPDFNoMPtOverlayIndex15.png"></a></td>
</tr>
<tr>
<td>Pt 300-450 GeV</td>
<td><a href="histForPDFNoMPtOverlayIndex16.png"><img height="200" src="histForPDFNoMPtOverlayIndex16.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex17.png"><img height="200" src="histForPDFNoMPtOverlayIndex17.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex18.png"><img height="200" src="histForPDFNoMPtOverlayIndex18.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex19.png"><img height="200" src="histForPDFNoMPtOverlayIndex19.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex20.png"><img height="200" src="histForPDFNoMPtOverlayIndex20.png"></a></td>
</tr>
<tr>
<td>Pt > 450 GeV</td>
<td><a href="histForPDFNoMPtOverlayIndex21.png"><img height="200" src="histForPDFNoMPtOverlayIndex21.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex22.png"><img height="200" src="histForPDFNoMPtOverlayIndex22.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex23.png"><img height="200" src="histForPDFNoMPtOverlayIndex23.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex24.png"><img height="200" src="histForPDFNoMPtOverlayIndex24.png"></a></td>
<td><a href="histForPDFNoMPtOverlayIndex25.png"><img height="200" src="histForPDFNoMPtOverlayIndex25.png"></a></td>
</tr>
</table>

</FONT>
</BODY>
</HTML>

EOF


exit

