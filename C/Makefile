
CPPFLAGS=-UCL_VERSION_2_0
CFLAGS=-Wall -O0 -std=c11 -g
LDFLAGS=-lOpenCL 

hellocl: hellocl.c query.c names.c
	gcc $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f hellocl *.o
