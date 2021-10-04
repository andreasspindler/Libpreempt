#
# Compile: make quick
# Compile: make test
# Compile: make develop
#
all:;		./pudding.sh -D build 100
quick:;		./pudding.sh -D build 10
real:;		./pudding.sh -DOPER build 1000
develop:;	./pudding.sh -f$@ -D build 30
clean:;		./pudding.sh clean
realclean:
	rm -rf out
