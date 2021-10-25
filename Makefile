#
# Compile: make quick
# Compile: make develop
#
.PHONY: all quick develop clean realclean push

# common targets
all:;				./pudding.sh -D build 100
quick:;				./pudding.sh -D build 10
develop:;			./pudding.sh -f$@ -D build 100
sched:;				./pudding.sh -f$@ -D build 100
real: realclean;	./pudding.sh -DOPER build 1000

# maintainer targets
clean:
	./pudding.sh clean
realclean:
	-rm -rf out

# EOF
