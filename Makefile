all: task1 task2 
	
task1: task1.c
	gcc -static task1.c -o $@

task2: task2.c
	gcc -static task2.c -o $@

clean:
	rm -f task1 task2
