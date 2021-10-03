#
# Compile: make quick
# Compile: make test
# Compile: make develop
#
quick:;		./pudding.sh -D build 10
test:;		./pudding.sh test
all:;		./pudding.sh -DOPER build 100
1000:;		./pudding.sh -DOPER build 1000
develop:;	./pudding.sh -f$@ -DR build 10
clean:;		./pudding.sh clean
