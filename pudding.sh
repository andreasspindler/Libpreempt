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
  Name=$(basename "$PWD")
  InDir='tests'
  OutDir="out"
  Configuration='all'
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
  -c<NAME>      define test configuration (default: $Config)
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
  Configuration       $Configuration
  InDir               $InDir
  OutDir              $OutDir
  Tests               (<all .cpp-files under InDir>)
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

    optshy=0 optquiet=0 optrelax=0 optparallelize=0

    while getopts "DOPERc:o:j:qsSh" o; do
      case "$o" in
        D) Optimizations+=('debug');;
        O) Optimizations+=('O0' 'O1' 'O2' 'O3' 'Os' 'Ofast');;
        P) Optimizations+=('profile');;
        E) Optimizations+=('enlightened');;
        R) Optimizations+=('release');;
        c) Configuration=$OPTARG;;
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
    Cmds=()
    (($#)) && Cmds+=("$@") || Cmds+=()
    March=$(hostname)-$Configuration
    OutDir="$OutDir/$March"

    find_tests() { find ${1:-$InDir} -maxdepth 1 -not -name '[._]*' -and -name "${2:-*.cpp}"; }
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
  ANSI_YLW="\033[1;93m" ANSI_DYL="\033[1;33m"
  ANSI_RED="\033[1;91m" ANSI_DRD="\033[1;31m"
  ANSI_OFF="\033[0m"
  cout() { echo -e "### $(hostname) $Name(${Configuration}) ***" "$@"; } 1>&2
  cerr() { cout "$@"; }
  info() { ((optquiet<1)) && cout "$@"; }
  chat() { ((optquiet<2)) && cout "$@"; }
  warn() { cerr "${ANSI_DYL}WARNING:${ANSI_YLW}" "$@" "$ANSI_OFF"; }
  error() { cerr "${ANSI_DRD}ERROR:${ANSI_RED}" "$@" "$ANSI_OFF"; }
  die() { error "$@"; exit 1; }

  ##############################################
  # populate array Tests
  #
  # source .puddingrc and complete global variables
  #
  Tests=($(find_tests $InDir))
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
  }

  ##############################################
  # create Makefile
  #
  {
    Pwd=$(pwd)
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
  info "$TestsSize tests => $TargetsSize executables"

  runmake() {
    local args=("$@")
    chat "${Make[@]} -f $Makefile ${args[@]}"
    ${Make[@]} -f $Makefile ${args[@]} || exit -1
  }

  ######################################################################
  # execute commands
  #
  RunningUnderVM=0
  RunningUnderLinux=0
  RunningUnderPREEMPT=0
  RunningAsRoot=$((UID == 0))
  UserMaximumRTPriority=32

  grep -q "^flags.*hypervisor" /proc/cpuinfo && RunningUnderVM=1
  { uname -v | grep -w 'PREEMPT' >/dev/null; } && RunningUnderPREEMPT=1
  case $(uname -s) in Linux*) RunningUnderLinux=1;; esac
  ulimit -r &>/dev/null && UserMaximumRTPriority=$(ulimit -r)
  {
    Summary=0 TotalRuns=0 TotalGood=0 TotalBad=0 TotalMissing=0
    for Cmd in ${Cmds[@]}; do
      CmdSuccess=1
      BgPids=()
      #chat "$Cmd: $(date -R)"
      case $Cmd in
        ########################################
        # list all tests
        #
        'list')
          for target in $(for x in ${!Targets[@]}; do echo $x; done | sort); do
            echo $OutDir/$target
          done
          ;;

        ########################################
        # run tests N times
        #
        [0-9]*)
          if runmake build; then
            info 'build successful'
            if pushd $OutDir >/dev/null; then
              Summary=1
              info "$TestsSize tests => $TargetsSize targets => $((Cmd * TargetsSize)) runs"
              ((RunningUnderVM)) && { optrelax=1; warn "running under VM"; }
              ((RunningUnderPREEMPT)) || { optrelax=1; warn "no PREEMPT_RT patches installed in kernel"; }
              if ((RunningAsRoot)); then
                :
              elif ((UserMaximumRTPriority==0)); then
                error "user '$USER' can not start real-time threads"
              elif ((UserMaximumRTPriority<32)); then
                warn "user '$USER' can only start real-time threads up to priority $UserMaximumRTPriority"
                optrelax=1
              elif ((UserMaximumRTPriority < 99 && RunningUnderLinux)); then
                warn "user '$USER' can only start real-time threads up to priority $UserMaximumRTPriority"
                optrelax=1
              fi
              if ((optparallelize)); then
                #
                # Grouped
                #
                # TBD: group all variations of a test parallely
                #
                die "unimplemented"
              else
                #
                # Ungrouped
                #
                # Current directory is $OutDir. Run each executable in
                # a separate line. If bad runs occured and shy mode is
                # enabled stop the script.
                #
                lno=1
                Report="$Pwd/${March}-report.md"
                cat >$Report <<EOF
<!---*- mode:markdown; eval:(auto-revert-mode); -*--->
EOF
                for Target in $(for x in ${!Targets[@]}; do echo $x; done | sort)
                do
                  Good=0 Bad=0 Exists=0 Type=$(file $Target) Error=''
                  if [[ -x $Target ]]; then
                    if [[ $Type =~ 'executable' ]]; then
                      Exists=1
                    else
                      Error="${ANSI_RED}EXECUTABLE HAS INVALID FORMAT${ANSI_OFF}"
                    fi
                  else
                    Error="${ANSI_RED}MISSING${ANSI_OFF}"
                  fi
                  for ext in stderr stdout stackdump; do
                    rm -f $Target.$ext
                  done
                  for i in $(seq 1 $Cmd); do
                    if ((Exists)); then
                      ($Sudo ./$Target) 2>>$Target.stderr | tee -a $Target.stdout
                      Result=${PIPESTATUS[0]}
                      echo "$i/$Cmd: exit code $Result" >>$Target.stderr
                      ((Result==0 && Good++))
                    else        # missing
                      :
                    fi
                  done
                  cat >>$Report <<EOF

# $Target
$Error
$Type

EOF
                  for ext in stderr stdout stackdump; do
                    cat >>$Report <<EOF

## $ext

EOF
                    [[ -s $Target.$ext ]] && cat $Target.$ext >>$Report
                  done
                  Bad=$((Cmd - Good))
                  if ((optshy==0 || Bad)); then
                    if [[ -n $Error ]]; then
                      printf "% 5u/% 5u:  %-40s             $Error" \
                             $((lno++)) $TargetsSize $Target
                    elif ((Bad)); then
                      printf "% 5u/% 5u:  %-40s % 10u runs ${ANSI_RED}% 10u bad${ANSI_OFF} % 10u good" \
                             $((lno++)) $TargetsSize $Target $Cmd $Bad $Good
                    else
                      printf "% 5u/% 5u:  %-40s % 10u runs % 10u bad % 10u good" \
                             $((lno++)) $TargetsSize $Target $Cmd $Bad $Good
                    fi
                    printf "\n"
                  fi
                  ((TotalRuns+=Cmd)); ((TotalGood+=Good)); ((TotalBad+=Bad))
                  ((Bad && optshy > 1)) && die "bad runs in shy mode"
                done
              fi                # run
            else
              die
            fi                  # build
            popd >/dev/null
          else
            cout B
            die "build failed"
          fi
          ;;

        ########################################
        # Rebuild and do [lots of] runs until CTRL-C is pressed or all
        # work is done.
        #
        'afk'|'stress')
          jiva="$0 -DOER -sqq -c${Configuration:-all}"
          fnno=(0 1 1 2 3 5 8 12)
          case $Cmd in
            'afk')
              fnno+=(21 34 55 89 144 233 377 610 987 1597 2584 4181 6765 10946 17711 28657 46368)
              ;;
          esac
          for fn in ${fnno[@]}; do
            $Sudo $jiva $fn || die "$Cmd"
          done
          ;;

        ########################################
        # Unmatched commands are forwarded as Makefile targets.
        #
        'maintainer-clean')
          info "removing '$OutDir'"
          rm -rf tags TAGS BROWSE $OutDir
          exit                  # can't continue
          ;;
        'realclean')
          info "removing '$OutDir'"
          rm -rf $OutDir && mkdir -pv $OutDir || exit
          ;;
        'build'|'clean')
          make $Cmd || CmdSucccess=0
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
      TotalBad=$((TotalRuns - TotalGood))
      if ((TotalRuns))
      then
        TotalGoodPercent=$(bc <<<"scale=3; $TotalGood / $TotalRuns * 100")
      else
        TotalGoodPercent='100.000'
      fi
      if ((TotalBad)); then
        printf "              %-40s % 10u runs ${ANSI_RED}% 10u bad${ANSI_OFF} % 10u good\n" \
               "SUMMARY" $TotalRuns $TotalBad $TotalGood
      else
        printf "              %-40s % 10u runs % 10u bad % 10u good\n" \
               "SUMMARY" $TotalRuns $TotalBad $TotalGood
      fi
      info "SUCCESS = $TotalGoodPercent%"
      info "see '$(basename $Report)' for details"
      case $TotalGoodPercent in
        100*) ;;
        *) ((optrelax)) && exit 0 || exit 1;;
      esac
    fi
  }
}

# EOF
