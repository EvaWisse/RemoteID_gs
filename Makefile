CC = g++
CFLAGS = -O2
LIBS = include/miracl/core.a

cca2_desktop:
	$(CC) $(CFLAGS) src/*cpp  desktop/cca2.cpp $(LIBS) -o cca2_desktop.out
	
cpa_desktop:
	$(CC) $(CFLAGS) src/*cpp  desktop/cpa.cpp $(LIBS) -o cpa_desktop.out

setup_join:
	$(CC) $(CFLAGS) src/*cpp  drone/setup_join.cpp $(LIBS) -o setup_join.out

cca2_sign:
	$(CC) $(CFLAGS) src/*cpp  drone/cca2_sign.cpp $(LIBS) -o cca2_sign.out

cca2_sign_pre:
	$(CC) $(CFLAGS) src/*cpp  drone/cca2_sign_pre.cpp $(LIBS) -o cca2_sign_pre.out

cpa_sign:
	$(CC) $(CFLAGS) src/*cpp  drone/cpa_sign.cpp $(LIBS) -o cpa_sign.out

cpa_sign_pre:
	$(CC) $(CFLAGS) src/*cpp  drone/cpa_sign_pre.cpp $(LIBS) -o cpa_sign_pre.out

verify_open:
	$(CC) $(CFLAGS) src/*cpp  drone/verify_open.cpp $(LIBS) -o verify_open.out

clean:
	rm -f *.out
	rm -f *.txt
	rm -f *.h
	
header:
	$(CC) $(CFLAGS) src/*cpp  test/header_check.cpp $(LIBS) -o header_check.out
	
file:
	$(CC) $(CFLAGS) src/*cpp  test/file_check.cpp $(LIBS) -o file_check.out