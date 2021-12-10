#
# Compile: make stress
# Compile: make quick
# Compile: make parinama
#
.PHONY: all quick build refresh real clean realclean

PUDDING=./pudding.sh

#
# common targets
#
all:; $(PUDDING) -DR 100
quick:; $(PUDDING) -DR 10
build:; $(PUDDING) -DR
rebuild: clean; $(PUDDING) -DR

# maintainer targets
#
stress afk:; $(PUDDING) -f$@ -DOPER $@
parinama:; $(PUDDING) -f$@ -s -DR clean 100
f1 p3: clean; $(PUDDING) -f$@ -s -D 10

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
