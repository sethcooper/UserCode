#! /bin/bash


#preferred_dir="/home/daq/"
preferred_dir=`pwd`
log_dir=$preferred_dir/log/
conf_dir=$preferred_dir/conf/
#cmssw_dir="/home/daq/DUMP/CMSSW_1_3_1/src"
cmssw_dir=`pwd`

# in case you wanted to force  execution inside the preferred_dir
#if [ "$PWD" != $preferred_dir ]; then
#
# echo ""
# echo "this script should be executed from $preferred_dir"
# echo "please do:"
# echo "            cd $preferred_dir"
# echo ""
## exit
#
#fi

mkdir -p  $preferred_dir/log/
mkdir -p  $preferred_dir/conf/


if [ ! -n "$1" ]

then

echo ""
echo "This script produces Root histograms of ADC counts using the given samples, given supermodules, and given channels "
echo ""
echo "Options:"
echo ""
echo "      -p|--path_file        file_path       path to the data to be analyzed (default is /data/ecalod-22/daq-data/)"
echo "      -d|--data_file        file_name       data file to be analyzed"
echo ""
echo "      -f|--first_ev         f_ev            first (as written to file) event that will be analyzed; default is 1"
echo "      -l|--last_ev          l_ev            last  (as written to file) event that will be analyzed; default is 9999"
echo "      -fed|--fed_id         fed_id          selects FED id (601...654); default is all"
echo "      -cry|--cryGraph       ics             graphs from channel ic will be created"
echo "      -s|--sample(s)        samples         sample to be examine; defaults to samples 1-3"
echo ""
echo "To specify multiple fed_id's, crys, or samples, use a comma-separated list in between double quotes, e.g., \"1,2,3\" "
exit

fi


data_path="/data/ecalod-22/daq-data/"
data_file="none"

cfg_path="$conf_dir"


fed=-1

cry_ic=1;
cryString="false";

sample="1, 2, 3"

first_event=1
last_event=9999



  while [ $# -gt 0 ]; do    # while there are parameters available...
    case "$1" in

      -p|--path_file)
                data_path="$2"
                ;;

      -d|--data_file)
                data_file="$2"
                ;;


      -f|--first_ev)
                first_event="$2"
                ;;


      -l|--last_ev)
                last_event="$2"
                ;;


      -fed|--fed_id)
                fed=$2
                ;;

      -cry|--cryGraph)
                cry_ic=$2
                cryString="true"
                ;;

      -s|--sample)
                sample=$2
                ;;

    esac
    shift       # Verifica la serie successiva di parametri.

done

echo ""
echo ""
echo "data to be analyzed:                          $data_file"
echo "first event analyzed will be:                 $first_event"
first_event=$(($first_event-1))

echo "last event analyzed will be:                  $last_event"
echo "supermodules selected:                        ${fed}"


if [[  $cryString = "true"  ]]
then
        echo "channels selected for graphs:                 ${cry_ic}"
fi

        echo "sample(s):                                    ${sample}"


echo ""
echo ""




cat > "$cfg_path$data_file".graph.$$.cfg <<EOF



process TESTGRAPHDUMPER = { 

  include "EventFilter/EcalRawToDigiDev/data/EcalUnpackerMapping.cfi"

    module ecalEBunpacker = EcalRawToDigiDev{

      untracked string InputLabel="source"

#    by default whole ECAL fedId range is loaded, 600-670.
#    This is in case a selection was needed.
#    fedId==13 is special case for ECAL cosmic stand
#    example 1: all the feds allocated to ECAL DCC's are selected by providing this empty list
#        untracked vint32  FEDs    = { }
#    example 2: fedId used by the DCC at th cosmics stand
#            untracked vint32 FEDs = {13}
#    example 3: fedId used at the best beams (e.g. SM22)
#            untracked vint32 FEDs = {22}

        untracked vint32 FEDs = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35}

# By default these are true  
      untracked bool eventPut               = true 
        untracked bool memUnpacking     = true
        untracked bool headerUnpacking = true 
        untracked bool srpUnpacking       = true
#untracked bool tccUnpacking     = true
        untracked bool feUnpacking         = true
        untracked bool syncCheck           = false 

        untracked vint32 orderedFedList =   {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36}
      untracked vint32 orderedDCCIdList = {28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27}

      untracked bool DoRegional = false
        untracked string FedLabel = "listfeds"

    }

untracked PSet maxEvents = {untracked int32 input = $last_event}

# if getting data from a .root pool file
     source = PoolSource {
       untracked uint32 skipEvents = $first_event
       untracked vstring fileNames = { 'file:$data_path$data_file' }
       untracked bool   debugFlag     = true
     }

#     source = NewEventStreamFileReader{
#       untracked uint32 skipEvents = $first_event
#       untracked vstring fileNames = { 'file:$data_path$data_file' }
#       untracked uint32 debugVebosity = 10
#       untracked bool   debugFlag     = true
#     } 

     module graphDumperModule = EcalPedHistDumperModule {

       # selection on sm number in the barrel (1...36; 1 with tb unpacker)
       untracked vint32 listFEDs = {${fed}}
	
       # specify list of channels to be dumped
       untracked vint32  listChannels = {${cry_ic}}

       # sepecify list of samples to use
       untracked vint32 listSamples = {${sample}}

       untracked string fileName =  '$data_file.$$'
       string EBdigiCollection = 'ebDigis'
       string EEdigiCollection = 'eeDigis'
       string digiProducer = 'ecalEBunpacker'
     }
     
     path p = {ecalEBunpacker, graphDumperModule}

}




EOF


echo "initializing cmssw..."
export SCRAM_ARCH=slc3_ia32_gcc323
#. /nfshome0/cmssw/cmsset_default.sh
cd $cmssw_dir;
eval `scramv1 ru -sh`;
cd -;
echo "... running"
cmsRun "$cfg_path$data_file".graph.$$.cfg >& "$log_dir$data_file".$$.graph

echo ""
echo ""

mv *.graph.root log/
echo "File root with graphs was created:" 
ls -ltrFh $preferred_dir/log/*.graph.root | tail -1 | awk '{print $9}'

echo ""
echo ""
echo "Now you can look at the plots (TBrowser)..."
echo ""
echo ""

root -l `ls -ltrFh $preferred_dir/log/*.graph.root | tail -1 | awk '{print $9}'`
