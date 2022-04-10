#!/usr/bin/env bash
# -*-coding:raw-text-unix;-*-
#
# Unit test tool that compiles a bunch of C++ files, runs the
# executables and collects statistics.
#
{
  ##############################################
  # options and environment
  #
  # we use camel-case to denote global variables
  #
  Name=$(basename "$PWD")
  InDir='tests'
  BuildDir='build'
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
  -s            shy mode (stop if a run fails instead of counting)
  -S            run test targets under 'sudo'
  -v            verbose mode
  -h            help

Valid values for <CMD>:

  build     build tests
  list      lists tests
  stress    build whole test matrix and try a few runs.
  afk       dto. but do very many runs (yet not infinitely many)
  <NUMBER>  build and run this number of times

Global variables that can be overrided in '.testrc':

  BuildDir            $BuildDir
  CC                  ${CC[@]}
  Configuration       $Configuration
  InDir               $InDir
  Make                (${Make[@]})
  Optimizations       (${Optimizations[@]})
  Standards           (${Standards[@]})
  Tests               (<all C++ files under InDir>)

Valid values for <OPT> and valid entries for the 'Optimizations' array:

  NAME                EXTRA COMPILER FLAGS
  0|1|2|3|fast        -O<OPT>
  debug               create debug-info
  profile             profile time optimizations
  release             -Werror -DNEBDUG
  enlightened         'release' plus link time optimization

EOF
      exit ${1:-1}
    }

    optshy=0 optverbose=0 optrelax=0 optparallelize=0

    while getopts "DOPERc:o:j:vsSh" o; do
      case "$o" in
        D) Optimizations+=('debug');;
        O) Optimizations+=('O0' 'O1' 'O2' 'O3' 'Os' 'Ofast');;
        P) Optimizations+=('profile');;
        E) Optimizations+=('enlightened');;
        R) Optimizations+=('release');;
        c) Configuration=$OPTARG;;
        o) Optimizations+=($OPTARG);;
        j) Make+=(-j$OPTARG);;
        v) ((optverbose++));;
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
    MarchDir="$BuildDir/$March"

    find_tests() { find ${1:-$InDir} -maxdepth 1 -not -name '[._]*' -and \( -name "${2:-*.cpp}" -o -name "${2:-*.cc}" \); }
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
  cout() { echo -e "### $(hostname) $Name(${Configuration}) ***" "$@"; } #1>&2
  cerr() { cout "$@"; } >&2
  green() { local label=$1; shift; cout "${ANSI_DGR}$label:${ANSI_GRN} $@$ANSI_OFF"; }
  red() { local label=$1; shift; cout "${ANSI_DRD}$label:${ANSI_RED} $@$ANSI_OFF"; }
  info() { ((optverbose)) && cout "$@"; }
  warn() { cerr "${ANSI_DYL}WARNING:${ANSI_YLW}" "$@" "$ANSI_OFF"; }
  error() { cerr "${ANSI_DRD}ERROR:${ANSI_RED}" "$@" "$ANSI_OFF"; }
  die() { error "$@"; exit 1; }

  ##############################################
  # populate array Tests and Targets
  #
  # source .testrc and complete global variables
  #
  # loop over all Optimizations and create targets and rules
  #
  Tests=($(find_tests $InDir))
  [[ ! -f .testrc ]] || source .testrc || die 'bad .testrc'
  TestsSize=${#Tests[@]}

  ((${#Optimizations[@]})) || Optimizations=('default')
  info "$(uname -srm) ($(${CC[0]} --version | head -1))"
  info "${Standards[@]} ${Optimizations[@]}"
  {
    declare -A Targets Rules1 Rules2 Clusters
    declare -a Flags
    Targets=() Rules1=() Rules2=()

    get_target_basename() {
      local cpptest=$1 std=$2 opt=$3 ext
      [[ ${cpptest##*.} == "cpp" ]] && ext=.cpp
      [[ ${cpptest##*.} == "cxx" ]] && ext=.cxx
      [[ ${cpptest##*.} == "cc"  ]] && ext=.cc
      echo -n "$(basename $cpptest $ext).$std.$opt"
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
          Rules1["$Target"]="${CC[@]} -std=$Standard ${Flags[@]} -I$(realpath include) -o \$@ \$^"
          # Set CAP_SYS_NICE capability on the executable
          # https://manpages.debian.org/stretch/manpages/capabilities.7.en.html
          #Rules2["$Target"]="setcap cap_sys_nice=ep \$@"
        done
      done
    done
    TargetsSize=${#Targets[@]}
  }

  ##############################################
  # create $Makefile in $MarchDir
  #
  Makefile="$BuildDir/${March}.mak"

  function makemakefile {
    [[ -e $Makefile ]] || green CREATING "$Makefile"
    mkdir -p $BuildDir || exit -1
    x=() Pwd=$(pwd)
    for Target in "${!Targets[@]}"; do
      x+=("$March/$Target")
    done
    cat >$Makefile <<EOF
# -*- mode:makefile; eval:(auto-revert-mode); -*-
# Paths are relative to directory
#    $(realpath "$BuildDir")

all: build

build: ${x[@]}

clean:; rm -fr ${March}/

EOF
    x=()
    for f in $(ls src/*.cc); do
      x+=($(realpath "$f"))
    done
    for Target in "${!Targets[@]}"; do
      Test=${Targets[$Target]}
      cat >>$Makefile <<EOF
$March/$Target: $(realpath $Test) ${x[@]}
$(printf "\t")@mkdir -p ${March}/
$(printf "\t")${Rules1[$Target]}
$(printf "\t")${Rules2[$Target]}
EOF
    done
    for Test in "${!Targets[@]}"; do
      Target="$MarchDir/${Targets[$Test]}"
    done
    ln -fs ${March}.mak $BuildDir/Makefile
  }

  function makebuild {
    makemakefile
    green "BUILD" "$TestsSize tests => $TargetsSize executables"
    local mak=$(basename $Makefile)
    set -x
    ${Make[@]} "$@" --no-print-directory -C$BuildDir -f$mak build || exit -1
    { set +x; } 2>/dev/null
  }

  ##############################################
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
      #info "$Cmd: $(date -R)"
      case $Cmd in
        ########################################
        # list all tests
        #
        'list')
          for target in $(for x in ${!Targets[@]}; do echo $x; done | sort); do
            echo $MarchDir/$target
          done
          ;;

        ########################################
        # run all tests N times
        #
        [0-9]*)
          if makebuild; then
            info 'build successful'
            if pushd $MarchDir >/dev/null; then
              Summary=1
              green "RUN" "$TestsSize tests => $TargetsSize targets => $((Cmd * TargetsSize)) runs"
              ((RunningUnderVM)) && { info "running under VM"; }
              ((RunningUnderPREEMPT)) || { optrelax=1; warn "no PREEMPT_RT patches installed in kernel"; }
              if ((RunningAsRoot)); then
                :
              elif ((UserMaximumRTPriority==0)); then
                error "user '$USER' cannot start real-time threads"
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
                # Ungrouped. Current directory is $MarchDir. Run each
                # executable in a separate line. If bad runs occured
                # and shy mode is enabled stop the script.
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
                    if [[ $Type =~ 'executable' || $Type =~ ' ELF ' ]]; then
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
                      echo "$0: $i/$Cmd: exit code $Result" >>$Target.stderr
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
        # build
        # Rebuild and do [lots of] runs until CTRL-C is pressed or all
        # work is done.
        #
        'make')       makemakefile || CmdSuccess=0;;
        'build')      makebuild || CmdSucccess=0;;
        'rebuild')    makebuild -B || CmdSucccess=0;;
        'afk'|'stress')
          jiva="$0 -DOER -s -c${Configuration:-all}"
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
        # clean
        #
        'clean') rm -rf $MarchDir || CmdSucccess=0;;
        'realclean')  rm -rf $BuildDir || CmdSucccess=0;;
        *) die "unknown command '$Cmd'";;
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
    if ((Summary)); then
      cout "report written to '$Report'"
      if ((optrelax)); then
        : warn "expecting >=95% of tests to be ok (relax mode)"
      fi
      TotalBad=$((TotalRuns - TotalGood))
      if ((TotalRuns)); then
        TotalGoodPercent=$(bc <<<"scale=3; $TotalGood / $TotalRuns * 100")
      else
        TotalGoodPercent='100.000'
      fi
      green TOTAL $(printf "%u runs %u bad %u good\n" $TotalRuns $TotalBad $TotalGood)
      if ((optrelax)); then
        case $TotalGoodPercent in
          100*|9[5-9]*) green SUCCESS "$TotalGoodPercent% (expected >95%)";;
          *)            red ERROR "$TotalGoodPercent% (expected >95%)"; exit 1;;
        esac
      else
        case $TotalGoodPercent in
          100*) green SUCCESS "$TotalGoodPercent% good";;
          *)    red ERROR "$TotalGoodPercent% (expected 100%)"; exit 1;;
        esac
      fi
    fi
  }
}

# EOF
