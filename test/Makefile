

test: test.c test.h
	cd .. && $(CC) -o __test test/test.c -Iinclude $(OBJS)
	../__test
	rm -rf ../__test
	./test.sh $(PWD)/usfs.so

debug: test.c test.h
	cd .. && $(CC) -g -o __test test/test.c -Iinclude $(OBJS)
	lldb ../__test
	rm -rf ../__test

clean:
	rm -rf dev_zero.so reads_dev_zero