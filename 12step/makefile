TESTF +=  -O0 -g --coverage -std=gnu11 
TESTER = ./code/tester
TEST1 = ./code/test1
INTERF = ./modules/interface
DATA = ./modules/datatest
WAY = ./headers
TRASHFILE = ./trash/
FULL_COVER = ./full_cover_tests/

.PHONY: all clean 

all: makeTRASH obj compile 

makeTRASH: 
	mkdir -p $(TRASHFILE)

full_cover: clean makeTRASH obj compile
	./$(TRASHFILE)tester ./$(TRASHFILE)test1 < $(FULL_COVER)input1.txt > $(TRASHFILE)output.txt
	./$(TRASHFILE)tester ./$(TRASHFILE)test1 < $(FULL_COVER)input2.txt > $(TRASHFILE)output.txt
	./$(TRASHFILE)tester ./$(TRASHFILE)test1 < $(FULL_COVER)input3.txt > $(TRASHFILE)output.txt
	./$(TRASHFILE)tester ./$(TRASHFILE)test1 < $(FULL_COVER)input4.txt > $(TRASHFILE)output.txt
	./$(TRASHFILE)tester ./$(TRASHFILE)notexist < $(FULL_COVER)input4.txt > $(TRASHFILE)output.txt

obj:
	gcc $(INTERF).c -I $(WAY) -c -o $(TRASHFILE)interface.o $(TESTF)
	gcc $(TESTER).c -I $(WAY) -c -o $(TRASHFILE)tester.o $(TESTF) 
	gcc $(DATA).c -I $(WAY) -c -o $(TRASHFILE)datatest.o $(TESTF) 
	gcc $(DATA)2.c -I $(WAY) -c -o $(TRASHFILE)datatest2.o $(TESTF) 
	gcc $(TEST1).c -c -I $(WAY) -o $(TRASHFILE)test1.o $(TESTF) 

compile:
	gcc $(TRASHFILE)tester.o $(TRASHFILE)interface.o -o $(TRASHFILE)tester $(TESTF) 
	gcc $(TRASHFILE)test1.o $(TRASHFILE)datatest.o $(TRASHFILE)interface.o -o $(TRASHFILE)test1 $(TESTF)
	gcc $(TRASHFILE)test1.o $(TRASHFILE)datatest2.o $(TRASHFILE)interface.o -o $(TRASHFILE)test2 $(TESTF)

run1:
	./$(TRASHFILE)tester ./$(TRASHFILE)test1

run2:
	./$(TRASHFILE)tester ./$(TRASHFILE)test2

valgrind_1: compile
	valgrind --leak-check=full --leak-resolution=med ./$(TRASHFILE)tester ./$(TRASHFILE)test1 

valgrind_2: 
	valgrind --leak-check=full --leak-resolution=med ./$(TRASHFILE)tester ./$(TRASHFILE)test2 

lcov: 
	git -C $(TRASHFILE) clone https://github.com/linux-test-project/lcov.git

cover: full_cover lcov 
	$(TRASHFILE)lcov/bin/lcov  --capture --directory $(TRASHFILE) -output-file  $(TRASHFILE)trace_overflow.info  --rc lcov_branch_coverage=1
	$(TRASHFILE)lcov/bin/genhtml $(TRASHFILE)trace_overflow.info --branch-coverage -o $(TRASHFILE)site

op_site: 
	firefox $(TRASHFILE)site/index.html

clean:
	rm -r -f $(TRASHFILE)