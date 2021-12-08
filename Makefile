#
# Compile: make stress
# Compile: make parinama
# Compile: make quick
#
.PHONY: all quick build refresh real clean realclean

#
# common targets
#
all:;	./pudding.sh -DR build 100
quick:;	./pudding.sh -DR build 10
build:  ./pudding.sh -DR build
rebuild: clean TAGS;
	./pudding.sh -D build

#
# maintainer targets
#
stress: rebuild;	./pudding.sh -DOPER $@
parinama:;			./pudding.sh -f$@ -s -DR build # 10

clean:
	./pudding.sh clean
	rm -f *.mak a.out
realclean: clean
	rm -rf out
	find -name '.#-emacs*' -delete
	git gc
FORCE:
TAGS: FORCE
	ctags -f$@ -R --c++-kinds=+p --fields=+iaS --extra=+q include src

# EOF
