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
rebuild: clean; $(PUDDING) -DR clean build 10

# maintainer targets
#
stress afk:; $(PUDDING) -c$@ -DOPER $@
list:; $(PUDDING) -DOPER $@
parinama:; $(PUDDING) -c$@ -D 10

clean: TAGS
	$(PUDDING) clean
	rm -f *.mak a.out *.gcno *-report.md TAGS
realclean:
	rm -rf out
	rm -f *.mak a.out *.gcno *-report.md TAGS
	find -name '.#-emacs*' -delete
	git gc
FORCE:
TAGS: FORCE
	ctags -f$@ -R --c++-kinds=+p --fields=+iaS --extra=+q include src

# EOF
