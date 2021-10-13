#
# Compile: make quick
# Compile: make test
# Compile: make develop
#
.PHONY: all quick develop commit clean realclean

# common targets
all:;		./pudding.sh -D build 100
quick:;		./pudding.sh -D build 10
develop:;	./pudding.sh -f$@ -D build 100

# maintainer targets
commit: realclean
	./pudding.sh -DOPER build 1000
clean:;		./pudding.sh clean
realclean:
	rm -rf out

# EOF
