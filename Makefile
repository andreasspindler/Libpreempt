#
# Compile: make stress
# Compile: make quick
# Compile: make parinama
#
# 1. chrono
# 2. selbststests base_01_self
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
parinama:; $(PUDDING) -f$@ -s -DR clean build 100
f1 p3: clean; $(PUDDING) -f$@ -s -DR build 10

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
