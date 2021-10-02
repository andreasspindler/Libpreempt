# Compile: make quick
# Compile: make test

quick:;  ./pudding.sh -D -o O0 build 10

test:; ./pudding.sh test

all:; ./pudding.sh -DOPER build 100

sched:; ./pudding.sh -D -tsched -f build 10

std:; ./pudding.sh -D -tstd build 10

x:; ./pudding.sh -D -tx build 10
