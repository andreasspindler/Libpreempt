# Compile: make quick
# Compile: make test
# Compile: make develop

quick:;  ./pudding.sh -D build 10

test:; ./pudding.sh test

all:; ./pudding.sh -DOPER build 100

sched:; ./pudding.sh -D -tsched -f build 10

std:; ./pudding.sh -D -tstd build 10

# clean required because dependency of base/*.h not defined by pudding.sh
develop:
	./pudding.sh -f$@ clean
	./pudding.sh -f$@ -DR build 10
