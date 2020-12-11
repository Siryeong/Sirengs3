all:
	gcc -shared -fPIC -o ddmon.so ddmon.c -ldl
	gcc ddchck.c -o ddchck
	gcc fifomaker.c -o fifo
	./fifo
	gcc test1.c -o test1 -pthread
	gcc test2.c -o test2 -pthread
	gcc test3.c -o test3 -pthread
	gcc test4.c -o test4 -pthread
	gcc test5.c -o test5 -pthread

clean:
	rm ddmon.so
	rm ddchck
	rm fifo
	rm .ddtrace
	rm test1 test2 test3 test4 test5

target1:
	LD_PRELOAD="./ddmon.so" ./test1

target2:
	LD_PRELOAD="./ddmon.so" ./test2
	
target3:
	LD_PRELOAD="./ddmon.so" ./test3

target4:
	LD_PRELOAD="./ddmon.so" ./test4

target5:
	LD_PRELOAD="./ddmon.so" ./test5

