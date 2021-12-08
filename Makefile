#
# Compile: make stress
# Compile: make parinama
# Compile: make quick
#
.PHONY: all quick build refresh real clean realclean

PUDDING=./pudding.sh

#
# common targets
#
all:; $(PUDDING) -DR build 100
quick:; $(PUDDING) -DR build 10
build:; $(PUDDING) -DR build
rebuild: clean; $(PUDDING) -D build

#
# maintainer targets
#
stress: clean; $(PUDDING) -DOPER $@
parinama: clean; $(PUDDING) -f$@ -s -DR build # 10

clean: TAGS
	$(PUDDING) clean
	rm -f *.mak a.out
realclean: clean
	rm -rf out
	rm -f TAGS
	find -name '.#-emacs*' -delete
	git gc
FORCE:
TAGS: FORCE
	ctags -f$@ -R --c++-kinds=+p --fields=+iaS --extra=+q include src

# EOF
