#
# Compile: make stress
# Compile: make quick
# Compile: make parinama
#
.PHONY: all quick rebuild stress afk list akut help clean realclean

BUILD=./testmatrix.sh

all:; $(BUILD) -DR 100
quick:; $(BUILD) -DR 10
build rebuild: clean; $(BUILD) -DR $@ 10
stress afk:; $(BUILD) -c$@ -DOPER $@
akut:; $(BUILD) -c$@ -D 10
list:; $(BUILD) -DOPER $@

help:
	$(BUILD) -h
clean: TAGS
	$(BUILD) $@
	find -name '.#-*' -delete
realclean: clean
	$(BUILD) $@
	rm -f TAGS BROWSE
	git gc
FORCE:
TAGS: FORCE
	ctags -f$@ -R --c++-kinds=+p --fields=+iaS --extra=+q include src

# EOF
