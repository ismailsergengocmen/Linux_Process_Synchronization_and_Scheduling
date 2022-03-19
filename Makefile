all: systemsim

systemsim: systemsim.c
	gcc -Wall -o systemsim systemsim.c shareddefs.c -lrt -lpthread

clean: 
	rm -fr *~ systemsim