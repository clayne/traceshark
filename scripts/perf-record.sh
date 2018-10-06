#!/bin/sh

cmd=$(basename $0)
SAMPLE_RATE_PROCFILE="/proc/sys/kernel/perf_event_max_sample_rate"

# These are the default events that we will try to use
default_events="power:cpu_frequency power:cpu_idle sched:sched_kthread_stop sched:sched_kthread_stop_ret sched:sched_migrate_task sched:sched_move_numa sched:sched_pi_setprio sched:sched_process_exec sched:sched_process_exit sched:sched_process_fork sched:sched_process_free sched:sched_process_wait sched:sched_stick_numa sched:sched_swap_numa sched:sched_switch sched:sched_wait_task sched:sched_wake_idle_without_ipi sched:sched_wakeup sched:sched_wakeup_new sched:sched_waking cpu-cycles"

usage()
{
    echo "Usage:\n"$cmd" [<options>]"
    echo "\t-h|--help\t\tDisplay this help information"
    echo "\t-s|--sleep <N>\t\tSleep N seconds before terminating trace"
    echo "\t-m|--memory <N>\t\tUse a buffer of N megatbytes]"
    echo "\t-n|--no-callgraph\tDo not record callgraphs"
    echo "\t-f|--frame-ponter\tUse frampointer callgraphs"
    echo "\t-x|--max-freq <N>\tSet the max sample rate to N"
    echo "\t-e|--event E\t\tAdd event E to the events to be traced"
}

recording_msg()
{
    echo "Recording with:\n"$perf_cmd
}

timeout_stop_msg()
{
    echo "Timeout reached, stopped recording"
}

perf_cmd="perf record"

bufsize_opt=""
timeout_opt=""
callgraph_opt=""
sample_freq_opt="100000"

while [ "$1" != "" ]; do
    case $1 in
	-h | --help )
	    usage
	    exit 0
	    ;;
	-s | --sleep )
	    if [ $# -lt 2 ];then
		usage
		exit 0
	    fi
	    timeout_opt=$2
	    shift
	    ;;
	-m | --memory )
	    if [ $# -lt 2 ];then
		usage
		exit 0
	    fi
	    bufsize_opt=$2
	    shift
	    ;;
	-n | --no-callgraph )
	    callgraph_opt="none"
	    ;;
	-f | --frame-pointer )
	    callgraph_opt="fp"
	    ;;
	-x | --max-freq )
	    if [ $# -lt 2];then
		usage
		exit 0
	    fi
	    sample_freq_opt=$2
	    shift
	    ;;
	-e | --event )
	    if [ $# -lt 2];then
		usage
		exit 0
	    fi
	    event=$2
	    default_events="$default_events $event"
	    shift
	    ;;
	* )
	    usage
	    exit 0
	    ;;
    esac
    shift
done

# Check that each of the default events are supported and add them to the perf
# command
perflist=$(perf list|awk '{print $1}')
for event in $default_events
do
    for levent in $perflist
    do
	if [ "$event" = "$levent" ];then
	    perf_cmd="$perf_cmd -e $event"
	fi
    done
done

if [ "$bufsize_opt" = "" ];then
    memtotal=$(cat /proc/meminfo|grep MemTotal:|awk '{print $2}')
    nrcpus=$(cat /proc/cpuinfo|awk '$1=="processor" {nrcpus+=1} END {print nrcpus}')

    wantedmem=$(expr $nrcpus '*' 65536)
    machine=$(uname -m)

    # I only deal with two architectures, x86 and ARM. Using 25% of the buffer
    # size ought to be enough for ARM
    if [ $machine != "x86_64" -a $machine != "x86" ];then
	wantedmem=$(expr $wantedmem '/' 4)
    fi

    halfmem=$(expr $memtotal '/' 2)

    if [ $wantedmem -gt $halfmem ];then
	perfmem=$halfmem
    else
	perfmem=$wantedmem
    fi
else
    perfmem=$(expr $bufsize_opt '*' 1024)
fi

case $callgraph_opt in
    "" )
	perf_cmd="$perf_cmd --call-graph=dwarf,20480"
	;;
    "fp" )
	perf_cmd="$perf_cmd --call-graph=fp"
	;;
    "none" )
	;;
esac

perf_cmd="$perf_cmd -m $perfmem""K"

if [ -e $SAMPLE_RATE_PROCFILE ];then
    echo "Setting "$SAMPLE_RATE_PROCFILE" to "$sample_freq_opt
    echo $sample_freq_opt > $SAMPLE_RATE_PROCFILE
fi

if [ "$timeout_opt" != "" ];then
    $perf_cmd&
    pid=$!
    recording_msg
    sleep $timeout_opt
    kill -2 $pid
    timeout_stop_msg
else
    recording_msg
    $perf_cmd
    echo "Press Ctrl-C to stop recording"
fi