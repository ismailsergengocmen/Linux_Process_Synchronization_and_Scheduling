all: systemsim

systemsim: systemsim.c
	gcc -Wall -o systemsim systemsim.c shareddefs.c -lrt -lpthread -lm

clean: 
	rm -fr *~ systemsim