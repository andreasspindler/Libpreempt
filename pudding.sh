#!/usr/bin/env bash
# -*-coding:raw-text-unix;-*-
#
# pudding.sh -- unit test tool that compiles a bunch of C++ files,
#               runs the executables and collects statistics
#
# Because the proof is in the pudding.
#
{
  ##############################################
  # options and environment
  #
  # we use camel-case to denote global variables
  #
  TestTarget=$(basename "$PWD")
  TestDir='tests'
  TestOutDir="out"
  Flavor='all'
  CC=(g++ -pthread -D_GNU_SOURCE)
  Standards=(c++14 c++17)
  Optimizations=()
  Make=(make -j$(($(nproc) * 1)))
  Sudo=''
  {
    usage() {
      cat <<EOF >&2
Usage:
   $0 [OPTIONS]... [<CMD>]...

Options:
  -f<FLAVOR>    set test flavor
  -o<OPT>       add optimization level
  -D            shortcut for '-o debug'
  -O            shortcut for '-o O0 -o O1 -o O2 -o O3 -o Os -o Ofast'
  -P            shortcut for '-o profile'
  -E            shortcut for '-o enlightened'
  -R            shortcut for '-o release'
  -j<JOBS>      number of make jobs
  -q            increase silent mode
  -s            shy mode (stop if a run fails instead of counting)
  -S            run test targets under 'sudo'
  -h            help

Valid values for <CMD>:

  list      Lists all tests

  <NUMBER>  Define the # of processes to spawn for each test. In shy
            mode (-s) the script will stop if a run failed.

  afk       dto. but do very many runs (yet not infinitely many)

  stress    Compile all files on all optimization levels and try a few
            runs.

Global variables that can be overrided in '.puddingrc':

  NAME                DEFAULT
  Flavor              $Flavor
  TestDir             $TestDir
  Tests               (<all .cpp-files under TestDir>)
  Standards           (${Standards[@]})
  Optimizations       (${Optimizations[@]})
  CC                  ${CC[@]}
  Make                (${Make[@]})

Valid values for <OPT> and valid entries for the 'Optimizations'
array:

  NAME                EXTRA COMPILER FLAGS
  0|1|2|3|fast        -O<OPT>
  debug               create debug-info
  profile             profile time optimizations
  release             -Werror -DNEBDUG
  enlightened         'release' plus link time optimization

EOF
      exit ${1:-1}
    }

    optshy=0 optquiet=0 optverbose=0 optparallelize=0

    while getopts "DOPERf:o:j:qsSh" o; do
      case "$o" in
        D) Optimizations+=('debug');;
        O) Optimizations+=('O0' 'O1' 'O2' 'O3' 'Os' 'Ofast');;
        P) Optimizations+=('profile');;
        E) Optimizations+=('enlightened');;
        R) Optimizations+=('release');;
        f) Flavor=$OPTARG;;
        o) Optimizations+=($OPTARG);;
        j) Make+=(-j$OPTARG);;
        q) ((optquiet++));;
        s) ((optshy++));;
        S) Sudo='sudo';;
        h) usage 0;;
        *) usage 1;;
      esac
    done
    shift $((OPTIND-1))
    (($#)) && Cmds=("$@") || Cmds=()
    March=$(hostname)-$Flavor
    OutDir="$TestOutDir/$March"

    find_tests() { find ${1:-$TestDir} -maxdepth 1 -not -name '[._]*' -and -name "${2:-*.cpp}"; }
    readcommit() { git -C ${1:-.} rev-parse --short ${2:-HEAD} || echo ''; }
    {
      missing=()
      for x in $Sudo ${CC[0]} ${Make[0]}; do
        which $x 1>/dev/null 2>/dev/null || missing+=$x
      done
      ((${#missing[@]})) && {
        echo "system not ready: missing ${missing[@]}"
        exit 1
      }
    }
  }
  ANSI_GRN="\033[1;92m" ANSI_DGR="\033[1;32m"
  ANSI_YLW="\033[1;33m"
  ANSI_RED="\033[1;91m" ANSI_DRD="\033[1;31m"
  ANSI_OFF="\033[0m"
  cout() { echo -e "### $(hostname) $TestTarget(${Flavor}) ***" "$@"; }
  cerr() { cout "$@"; } >&2
  info() { ((optquiet<1)) && cout "${ANSI_YLW}$@${ANSI_OFF}"; }
  chat() { ((optquiet<2 && optverbose>0)) && cout "$@"; }
  warn() { cerr "${ANSI_DGR}WARNING:${ANSI_GRN}" "$@" "$ANSI_OFF"; }
  error() { cerr "${ANSI_DRD}ERROR:${ANSI_RED}" "$@" "$ANSI_OFF"; }
  die() { cerr "ERROR:" "$@"; exit 1; }

  ##############################################
  # populate array Tests
  #
  # source .puddingrc and complete global variables
  #
  case $Flavor in
    ''|all|thorough)
      Tests=($(find_tests $TestDir))
      ;;
  esac

  [[ ! -f .puddingrc ]] || source .puddingrc || die 'bad .puddingrc'
  TestsSize=${#Tests[@]}

  ((${#Optimizations[@]})) || Optimizations=('default')
  info "$(uname -srm) ($(${CC[0]} --version | head -1))"
  info "${Standards[@]} ${Optimizations[@]}"

  ##############################################
  # populate array Targets
  #
  # loop over all Optimizations and create makefile targets and rules
  #
  {
    declare -A Targets Rules Clusters
    declare -a Flags
    Targets=() Rules=()

    get_target_basename() {
      local cpptest=$1 std=$2 opt=$3
      echo -n "$(basename $cpptest .cpp).$std.$opt"
    }


    for Optimization in ${Optimizations[@]}; do
      Flags=()
      case $Optimization in
        default)      Flags+=();;
        debug)        Flags+=(-Og -ggdb -pedantic);;
        O*)           Flags+=(-$Optimization);;
        profile)      Flags+=(-fprofile-arcs -ftest-coverage -lgcov);;
        enlightened)  Flags+=(-flto -fno-fat-lto-objects);&
        release)      Flags+=(-DNDEBUG -Werror);;
      esac
      for Test in ${Tests[@]}; do
        for Standard in ${Standards[@]}; do
          Target=$(get_target_basename $Test $Standard $Optimization)
          Targets["$Target"]=$Test
          Rules["$Target"]="${CC[@]} -std=$Standard ${Flags[@]} -Iinclude -o \$@ \$^"
        done
      done
    done
    TargetsSize=${#Targets[@]}
    info "$TestsSize tests => $TargetsSize targets"
  }

  ##############################################
  # create Makefile
  #
  {
    Makefile="${March}.mak"
    chat "$Makefile => $OutDir/"
    mkdir -p $OutDir

    x=()
    for Target in "${!Targets[@]}"; do
      x+=("$OutDir/$Target")
    done
    cat >$Makefile <<EOF
# -*- mode:makefile; eval:(auto-revert-mode); -*-
#
# $(basename "$(pwd)")/Makefile
#    $(date -u)
#
.PHONY: all build clean

build: ${x[@]}
EOF

    for Target in "${!Targets[@]}"; do
      Test=${Targets[$Target]}
      cat >>$Makefile <<EOF
$OutDir/$Target: $Test \$(wildcard src/*.cpp)
$(printf "\t")${Rules[$Target]}

EOF
    done
    for Test in "${!Targets[@]}"; do
      Target="$OutDir/${Targets[$Test]}"
    done
    cat >>$Makefile <<EOF
clean:
$(printf "\t")find $OutDir -type f -executable -delete
EOF
  }

  ######################################################################
  # execute commands
  #
  RunningUnderVM=0
  grep -q "^flags.*hypervisor" /proc/cpuinfo && RunningUnderVM=1

  RunningUnderPREEMPT=0
  { uname -v | grep -w 'PREEMPT' >/dev/null; } && RunningUnderPREEMPT=1

  RunningAsRoot=$((UID == 0))

  RunningUnderLinux=0
  case $(uname -s) in
    Linux*) RunningUnderLinux=1;;
  esac

  UserMaximumRTPriority=$(ulimit -r)
  {
    Summary=0 TotalRuns=0 TotalGood=0 TotalBad=0 TotalMissing=0
    for Cmd in ${Cmds[@]}; do
      CmdSuccess=1
      BgPids=()
      chat "executing command '$Cmd' [$(date -R)]"
      case $Cmd in
        ########################################
        # list all tests
        #
        'list')
          for target in $(for x in ${!Targets[@]}; do echo $x; done | sort); do
            echo $target
          done
          ;;

        ########################################
        # run tests N times
        #
        [0-9]*)
          if pushd $OutDir >/dev/null; then
            Summary=1
            info "$TestsSize tests => $TargetsSize targets => $((Cmd * TargetsSize)) runs"
            ((RunningUnderVM)) && warn "running under VM"
            ((RunningUnderPREEMPT)) || warn "no PREEMPT_RT patches installed in kernel"
            if ((RunningAsRoot)); then
              :
            elif ((UserMaximumRTPriority==0)); then
              error "user '$USER' cannot start real-time threads"
            elif ((UserMaximumRTPriority<32)); then
              warn "user '$USER' can only start real-time threads up to priority $UserMaximumRTPriority"
            elif ((UserMaximumRTPriority < 99 && RunningUnderLinux)); then
              warn "user '$USER' can only start real-time threads up to priority $UserMaximumRTPriority"
            fi

            if ((optparallelize)); then
              #
              # Grouped
              #
              # TBD: group all variations of a test in one line and
              #      execute parallely (THIS CODE IS YET NOT WORKING)
              #
              declare -A chained
              for target in $(for x in ${!Targets[@]}; do echo $x; done | sort)
              do
                tbase=$(basename ${Targets[$target]} .cpp)
                chained[$tbase]+="./$target & "
              done
              for tbase in $(for x in ${!chained[@]}; do echo $x; done | sort)
              do
                Good=0 Bad=0 Missing=0
                t=${chained[$tbase]}
                printf "%-32s % 10u run(s) " $tbase $Cmd
                for i in $(seq 1 $Cmd); do
                  if ($t) 2>>$tbase.stderr | tee -a $tbase.stdout; then
                    test ${PIPESTATUS[0]} -eq 0 && ((Good++))
                  else
                    ((Missing++))
                  fi
                done
                Bad=$((Cmd - Good - Missing))
                printf "% 10u bad % 10u good\n" $Bad $Good
              done
            else
              #
              # Ungrouped
              #
              # Current directory is $OutDir. Run each executable in a
              # separate line. If bad runs occured and shy mode is
              # enabled stop the script.
              #
              lno=1
              for t in $(for x in ${!Targets[@]}; do echo $x; done | sort)
              do
                Good=0 Bad=0 Missing=0 exists=0 x=
                [[ -x $t ]] && ((exists=1)) || x=' !! MISSING'
                ((optquiet<=1)) && printf "#% 4u/% 4u: %-40s % 10u run(s) " $((lno++)) $TargetsSize "$t$x" $Cmd
                for ext in stderr stdout stackdump; do
                  rm -f $t.$ext
                done
                for i in $(seq 1 $Cmd); do
                  if ((exists)); then
                    #BgPids+=($!)
                    if ($Sudo ./$t) 2>>$t.stderr | tee -a $t.stdout; then
                      test ${PIPESTATUS[0]} -eq 0 && ((Good++))
                    fi
                  else
                    ((Missing++))
                  fi
                done
                for ext in stderr stdout stackdump; do
                  [[ -s $t.$ext ]] || rm -f $t.$ext # remove if empty
                done
                Bad=$((Cmd - Good - Missing))
                ((optquiet<=1)) && printf "% 10u bad % 10u good\n" $Bad $Good
                ((TotalRuns+=Cmd)); ((TotalGood+=Good)); ((TotalBad+=Bad)); ((TotalMissing+=Missing))
                if ((optshy && TotalBad)); then
                  die "bailing out"
                fi
              done
            fi
            popd >/dev/null
          fi
          ;;

        ########################################
        # Rebuild and do lots of runs until CTRL-C is pressed or all
        # work is done.
        #
        'afk')
          jiva="$0 -DOER"
          fnno=(0 1 1 2 3 5 8 12 21 34 55 89 144 233 377 610 987 1597 2584 4181 6765 10946 17711 28657 46368)
          ((optquiet>=1)) && jiva+=' -q'
          ((optquiet==2)) && jiva+=' -q'
          ((optshy)) && jiva+=' -s'
          [[ -n $Flavor ]] && jiva+=" -f $Flavor"
          if $jiva build info; then
            for fn in ${fnno[@]}; do
              $Sudo $jiva $fn && CmdSuccess=1 || CmdSuccess=0
            done
          else
            exit
          fi
          ;;

        ########################################
        # Compile all files on all optimization levels and try a few
        # runs.
        #
        'stress')
          jiva="$0 -DOER -q -q -s"
          fnno=(0 1 1 2 3 5 8 12 21)
          if $jiva build info; then
            for fn in ${fnno[@]}; do
              $Sudo $jiva $fn && CmdSuccess=1 || CmdSuccess=0
            done
          fi
          ;;

        ########################################
        # Unmatched commands are forwarded as Makefile targets.
        #
        'maintainer-clean')
          info "removing '$TestOutDir'"
          rm -rf tags TAGS BROWSE $TestOutDir
          exit # can't continue with commands if the output directory
               # is gone
          ;;
        'realclean')
          info "removing '$OutDir'"
          rm -rf $OutDir && mkdir -pv $OutDir || exit
          ;;
        'build'|'clean')
          info ${Make[@]} -f $Makefile $Cmd
          if ${Make[@]} -f $Makefile $Cmd; then
            CmdSuccess=1
          else
            CmdSuccess=0
            exit
          fi
          ;;
        *)
          die "unknown command '$Cmd'"
          ;;
      esac

      # wait for background processes
      for pid in ${BgPids[@]}; do
        wait $pid || {
          status=$?
          kill -TERM ${BgPids[@]} 2>/dev/null
          kill -KILL ${BgPids[@]} 2>/dev/null
          echo "$(basename $0) $Cmd: stop!"
          exit $status
        }
      done
      wait

      if ((CmdSuccess)); then
        ((0)) && cerr "command '$Cmd' succeeded"
        :
      else
        die "command '$Cmd' failed"
      fi
    done

    #
    # Summary of all <NUMBER> commands
    #
    if ((Summary))
    then
      TotalBad=$((TotalRuns - TotalGood - TotalMissing))
      if ((TotalRuns))
      then
        TotalGoodPercent=$(bc <<<"scale=3; $TotalGood / $TotalRuns * 100")
      else
        TotalGoodPercent='100.000'
      fi
      info "$TotalGoodPercent% ($TotalRuns runs = $TotalGood good + $TotalBad bad + $TotalMissing missing)"
      case $TotalGoodPercent in
        100*)
          info "exit code 0"
          ;;
        *)
          info "exit code 1"
          exit 1
          ;;
      esac
    fi
  }
}

# EOF
