CC = g++
CFLAGS = -O2
LIBS = include/miracl/core.a

cca2_desktop:
	$(CC) $(CFLAGS) src/*cpp  demo/cca2/desktop/cca2.cpp $(LIBS) -o cca2_desktop.out
	
cpa_desktop:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/desktop/original/cpa.cpp $(LIBS) -o cpa_desktop.out

cpa_hybrid_pre:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/hybrid/precomp/setup/setup.cpp  $(LIBS) -o cpa_pre_setup.out

cca2_hybrid_pre:
	$(CC) $(CFLAGS) src/*cpp  demo/cca2/hybrid/precomp/setup/setup.cpp  $(LIBS) -o cca2_pre_setup.out

cpa_hybrid:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/hybrid/original/setup/setup.cpp  $(LIBS) -o cpa_setup.out

cca2_hybrid:
	$(CC) $(CFLAGS) src/*cpp  demo/cca2/hybrid/original/setup/setup.cpp  $(LIBS) -o cca2_setup.out

clean:
	rm -f *.out
	rm -f *.txt
	rm -f *.h