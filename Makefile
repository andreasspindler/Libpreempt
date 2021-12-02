#
# Compile: make TAGS quick
# Compile: make TAGS develop
#
.PHONY: all quick develop clean realclean push

# common targets
all:;					./pudding.sh -D build 100
quick:;					./pudding.sh -D build 10
develop: clean TAGS;	./pudding.sh -f$@ -D build 100
real: realclean;		./pudding.sh -DOPER build 1000

FORCE:
TAGS: FORCE
	ctags -f$@ -R --c++-kinds=+p --fields=+iaS --extra=+q include src

# maintainer targets
clean:
	rm -rf TAGS
	./pudding.sh clean
realclean:
	-rm -rf out

# EOF
