CC = g++
CFLAGS = -O2
LIBS = include/miracl/core.a

cca2_desktop:
	$(CC) $(CFLAGS) src/*cpp  demo/cca2/desktop/cca2.cpp $(LIBS) -o cca2_desktop.out
	
cpa_desktop:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/desktop/cpa.cpp $(LIBS) -o cpa_desktop.out

precomp_test:
	$(CC) $(CFLAGS) src/*cpp  test/precomp/precomp.cpp $(LIBS) -o precomp.out

header_test:
	$(CC) $(CFLAGS) src/*cpp  toheader.cpp $(LIBS) -o header.out

cca2_orig:
	$(CC) $(CFLAGS) src/*cpp  demo/cca2/hybrid/original/setup/setup.cpp  $(LIBS) -o cca2_orig_setup.out

cpa_orig:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/hybrid/original/setup/setup.cpp  $(LIBS) -o cpa_orig_setup.out

cpa_pre:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/hybrid/precomp/setup/setup.cpp  $(LIBS) -o cpa_pre_setup.out

cpa_precalc:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/hybrid/precomp/sign/precomp/precomp.cpp  $(LIBS) -o cpa_pre_calc.out
clean:
	rm -f *.out
	rm -f *.txt
	rm -f *.h