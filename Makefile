#
# Compile: sudo make build
# Compile: sudo make rebuild
#
.PHONY: all quick build refresh real clean realclean

#
# common targets
#
all:;	./pudding.sh -DR build 100
quick:;	./pudding.sh -DR build 10
build:  ./pudding.sh -DR build
rebuild: clean TAGS
	./pudding.sh -D build

#
# maintainer targets
#
commit: rebuild
	./pudding.sh -DOPER build 1000
parinama:
	./pudding.sh -f$@ -DR build 10
clean:
	rm -rf TAGS
	./pudding.sh clean
realclean:
	git gc --aggressive
	-rm -rf TAGS out
FORCE:
TAGS: FORCE
	ctags -f$@ -R --c++-kinds=+p --fields=+iaS --extra=+q include src

# EOF
