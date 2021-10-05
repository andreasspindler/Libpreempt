#
# Compile: make quick
# Compile: make test
# Compile: make develop
#

# common targets
all:;		./pudding.sh -D build 100
clean:;		./pudding.sh clean
quick:;		./pudding.sh -D build 10
develop:;	./pudding.sh -f$@ -D build 100

# maintainer targets
commit: realclean
	./pudding.sh -DOPER build 1000
realclean:
	rm -rf out

# EOF
