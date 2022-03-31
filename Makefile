# all: systemsim 

# systemsim: systemsim.c
# 	gcc -Wall -o systemsim systemsim.c shareddefs.c -lpthread -lm
	
# clean: 
# 	rm -fr *~ systemsim 

systemsim: systemsim.o shareddefs.o
	gcc -Wall systemsim.o shareddefs.o -o systemsim -lpthread -lm

systemsim.o: systemsim.c systemsim.h
	gcc -Wall -c systemsim.c -lpthread -lm

shareddefs.o: shareddefs.c shareddefs.h
	gcc -Wall -c shareddefs.c -lpthread -lm
