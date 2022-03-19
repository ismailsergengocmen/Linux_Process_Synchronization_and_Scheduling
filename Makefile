all: systemsim

systemism: systemsim.c
	gcc -Wall -o systemsim systemsim.c -lrt -lpthread

clean: 
	rm -fr *~ systemsim 