CC = g++
CFLAGS = -g -lrt --std=c++14
DEPS = TCB.h uthread.h uthread_private.h Lock.h CondVar.h SpinLock.h
OBJ = TCB.o uthread.o Lock.o CondVar.o SpinLock.o
MAIN_OBJ = test.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

uthread-sync-demo: $(OBJ) $(MAIN_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o uthread-sync-demo
