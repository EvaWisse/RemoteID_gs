CC = g++
CFLAGS = -O2
LIBS = include/miracl/core.a

cpa2_desktop:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa2/desktop/cpa2.cpp $(LIBS) -o cpa2_desktop.out

	
cpa_desktop:
	$(CC) $(CFLAGS) src/*cpp  demo/cpa/desktop/cpa.cpp $(LIBS) -o cpa_desktop.out

precomp_test:
	$(CC) $(CFLAGS) src/*cpp  test/precomp/precomp.cpp $(LIBS) -o precomp.out

header_test:
	$(CC) $(CFLAGS) src/*cpp  test/read_const/header.cpp $(LIBS) -o header.out

clean:
	rm -f *.out