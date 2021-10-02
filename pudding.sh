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
  TestFlavor='all'
  TestDir='tests'
  TestOutDir="out"
  TestMakefile='Makefile'
  TestOptimizations=()
  CC='g++'
  CCStandards=(c++14 c++17)
  CCBaseFlags=(-pthread -D_GNU_SOURCE -fmax-errors=5)
  Make=(make -k)
  MakeJobs=$(($(nproc) * 1))
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
  -q            silent mode (use twice to suppress warnings too)
  -s            shy mode (stop if a run fails instead of counting)
  -S            run test targets under 'sudo'
  -h            help

Valid values for <CMD>:

  list      Lists all tests

  <NUMBER>  Define the # of processes to spawn for each test. In shy mode (-s)
            the script will stop if a run failed.

  afk       dto. but do very many runs (yet not infinitely many)

  test      Compile all files on all optimization levels and try a few runs.

  info      Get system information.

The value of <FLAVOR> is stored in the global variable 'TestFlavor' and has to
be evaluated in '.puddingrc' by modifying other global variables to override
their defaults. These are:

  NAME                DEFAULT
  TestDir             $TestDir
  Tests               (<all .cpp-files in TestDir>)
  TestOptimizations   (${TestOptimizations[@]})
  CC                  $CC
  CCStandards         (${CCStandards[@]})
  Make                (${Make[@]})
  MakeJobs            $MakeJobs

Valid values for <OPT> and valid entries for the 'TestOptimizations'
array:

  NAME                COMPILER FLAGS
  default             ${CCBaseFlags[@]}
  0|1|2|3|fast        -O<OPT>
  debug               create debug-info
  profile             profile time optimizations
  release             -Werror -DNEBDUG
  enlightened         'release' plus link time optimization

EOF
      exit ${1:-1}
    }

    optshy=0 optquiet=0 optparallelize=0 RunningUnderVM=0 RunningUnderPREEMPT=0

    while getopts "DOPERf:o:j:qsSTh" o; do
      case "$o" in
        D) TestOptimizations+=('debug');;
        O) TestOptimizations+=('O0' 'O1' 'O2' 'O3' 'Os' 'Ofast');;
        P) TestOptimizations+=('profile');;
        E) TestOptimizations+=('enlightened');;
        R) TestOptimizations+=('release');;
        f) TestFlavor=$OPTARG;;
        o) TestOptimizations+=($OPTARG);;
        j) MakeJobs=$OPTARG;;
        q) ((optquiet++));;
        s) ((optshy++));;
        S) Sudo='sudo';;
        T) optassumepreempt=1;;
        h) usage 0;;
        *) usage 1;;
      esac
    done
    shift $((OPTIND-1))
    (($#)) && Cmds=("$@") || Cmds=()
    ((${#TestOptimizations[@]})) || TestOptimizations=('default')
    TestMarchDir="$TestOutDir/$(hostname)-$TestFlavor"
    Make+=(-j$MakeJobs)

    find_tests() { find ${1:-$TestDir} -maxdepth 1 -not -name '[._]*' -and -name "${2:-*.cpp}"; }
    readcommit() { git -C ${1:-.} rev-parse --short ${2:-HEAD} || echo ''; }
    cout() { echo "### $(hostname) $TestTarget(${TestFlavor}) ***" "$@"; }
    cerr() { cout "$@"; } >&2
    info() { ((optquiet==0)) && cout "$@"; }
    warn() { cerr "WARNING:" "$@"; }
    die() { cerr "ERROR:" "$@"; exit 1; }
    {
      missing=()
      for x in $Sudo $CC ${Make[0]}; do
        which $x 1>/dev/null 2>/dev/null || missing+=$x
      done
      ((${#missing[@]})) && die "system not ready: missing ${missing[@]}"
    }
  }

  ##############################################
  # populate array Tests
  #
  # source .puddingrc
  #
  case $TestFlavor in
    ''|all|thorough)
      Tests=($(find_tests $TestDir))
      ;;
  esac

  [[ ! -f .puddingrc ]] || source .puddingrc || die 'bad .puddingrc'
  TestCount=${#Tests[@]}

  ##############################################
  # populate array Targets
  #
  # loop over all TestOptimizations
  #    set CCStandards CCFlags
  #    depending on TestFlavor
  #       set Tests (evaluate .puddingrc in PWD)
  #       optionally overload global variables
  #    create makefile targets and rules
  #
  {
    declare -A Targets Rules Clusters
    declare -a Flags
    Targets=() Rules=() Flags=()

    get_target_basename() {
      local cpptest=$1 std=$2 opt=$3
      echo -n "$(basename $cpptest .cpp).$std.$opt"
    }

    for TestOptimization in ${TestOptimizations[@]}
    do
      CCFlags=("${CCBaseFlags[@]}")
      case $TestOptimization in
        default)      CCFlags+=();;
        debug)        CCFlags+=(-Og -ggdb -pedantic);;
        O*)           CCFlags+=(-$TestOptimization);;
        profile)      CCFlags+=(-fprofile-arcs -ftest-coverage -lgcov);;
        enlightened)  CCFlags+=(-flto -fno-fat-lto-objects);&
        release)      CCFlags+=(-DNDEBUG -Werror);;
      esac
      for CCTest in ${Tests[@]}; do
        for CCStandard in ${CCStandards[@]}; do
          Test=$(get_target_basename $CCTest $CCStandard $TestOptimization)
          Flags=("-std=$CCStandard" "${CCFlags[@]}")
          Targets["$Test"]="$CCTest"
          Rules["$Test"]="${CC} ${Flags[@]} -I../../include -o \$@ \$^"
        done
      done
    done
    TargetCount=${#Targets[@]}
  }

  ##############################################
  # create Makefile in $TestMarchDir
  #
  #info "creating: '$TestMarchDir/$TestMakefile'"
  {
    mkdir -p $TestMarchDir
    info "writing '$TestMarchDir/$TestMakefile'"
    cat >$TestMarchDir/$TestMakefile <<EOF
# -*- mode:makefile; eval:(auto-revert-mode); -*-
#
# $(basename "$(pwd)")/$TestMakefile
#    $(date -u)
#
all: build
build: ${!Targets[@]}
EOF
    for k in "${!Targets[@]}"
    do
      echo "$k: ../../${Targets[$k]} ../../src/*.cpp" >>$TestMarchDir/$TestMakefile
      echo -e "\t${Rules[$k]}" >>$TestMarchDir/$TestMakefile
    done
    cat >>$TestMarchDir/$TestMakefile <<EOF

clean:
$(printf "\t")find . -delete
EOF
  }

  ######################################################################
  # execute commands
  #
  grep -q "^flags.*hypervisor" /proc/cpuinfo && RunningUnderVM=1 || RunningUnderVM=0
  { uname -v | grep PREEMPT >/dev/null; } && RunningUnderPREEMPT=1 || RunningUnderPREEMPT=0

  {
    Summary=0 TotalRuns=0 TotalGood=0 TotalBad=0 TotalMissing=0
    for Cmd in ${Cmds[@]}; do
      CmdSuccess=1
      BgPids=()
      case $Cmd in
        'info')
          ((RunningUnderVM)) && warn "running under VM"
          ((RunningUnderPREEMPT)) || warn "no PREEMPT_RT patches installed in kernel '$(uname -s)'"
          ;;

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
          info "executing command '$Cmd' [$(date -R)]"
          info "${CCStandards[@]} ${TestOptimizations[@]}"
          if pushd $TestMarchDir >/dev/null; then
            Summary=1
            info "$TestCount tests => $TargetCount target(s) => $((Cmd * TargetCount)) total run(s)"
            if ((optparallelize)); then
              #
              # Grouped
              #
              # TBD: group all variations of a test in one line and
              #      execute parallely (this code is not working)
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
              # Run each executable in a separate line. If bad runs
              # occured and shy mode is enabled stop the script.
              #
              lno=1
              for t in $(for x in ${!Targets[@]}; do echo $x; done | sort)
              do
                Good=0 Bad=0 Missing=0 exists=0 x=
                t=$(basename $t)
                [[ -x $t ]] && ((exists=1)) || x=' !! MISSING'
                ((optquiet<=1)) && printf "#% 4u/% 4u: %-40s % 10u run(s) " $((lno++)) $TargetCount "$t$x" $Cmd
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
          [[ -n $TestFlavor ]] && jiva+=" -f $TestFlavor"
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
        'test')
          # same code as 'afk' only $jiva different
          jiva="$0 -DOER -q -q"
          ((optquiet>=1)) && jiva+=' -q'
          ((optquiet==2)) && jiva+=' -q'
          ((optshy)) && jiva+=' -s'
          [[ -n $TestFlavor ]] && jiva+=" -f $TestFlavor"
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
          info "removing '$TestMarchDir'"
          rm -rf $TestMarchDir && mkdir -pv $TestMarchDir || exit
          ;;
        'build'|'clean')
          if ${Make[@]} -C $TestMarchDir $Cmd; then
            CmdSuccess=1
          else
            CmdSuccess=0
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
        ((0)) && warn "command '$Cmd' failed"
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
      cout "$TotalGoodPercent% ($TotalRuns runs = $TotalGood good + $TotalBad bad + $TotalMissing missing)"
      case $TotalGoodPercent in
        100*)
          info "exit code 0"
          ;;
        *)
          exit 1
          ;;
      esac
    fi
  }
}

# EOF
