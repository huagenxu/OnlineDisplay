ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lNew
ROOTGLIBS     = $(shell root-config --glibs)

NAMEHISPROD := hisprod
NAMEHISPLOT := hisplot
NAMECLIENT  := client
NAMESERVER  := server
NAMEDATASER := dataserv
NAMEDATACLI := datacli

CC = g++
INCLUDES = `root-config --incdir`
LIBROOT  := `root-config --libs`

LIBS = $(ROOTLIBS) $(ROOTGLIBS)

CPPFLAGS +=$(ROOTCFLAGS)

all: hisprod hisplot datacli dataserv

hisprod:
	$(CC) $(CPPFLAGS) -o $(NAMEHISPROD) $(NAMEHISPROD).C -I$(INLCUDES) $(LIBS)

hisplot:
	$(CC) $(CPPFLAGS) -o $(NAMEHISPLOT) $(NAMEHISPLOT).C -I$(INCLUDES) $(LIBS)

#client:
#	$(CC) $(CPPFLAGS) -o $(NAMECLIENT) client_demo.c -I$(INCLUDES) $(LIBS)

#server:
#	$(CC) $(CPPFLAGS) -o $(NAMESERVER) server_demo.c -I$(INCLUDES) $(LIBS)

datacli:
	$(CC) $(CPPFLAGS) -o $(NAMEDATACLI) client_data.c -I$(INCLUDES) $(LIBS)

dataserv:
	$(CC) $(CPPFLAGS) -o $(NAMEDATASER) server_data.c -I$(INCLUDES) $(LIBS)

clean:
	-rm hisprod hisplot datacli dataserv histprod.map

	

