SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

#------------------------------------------------------------

CPLEXDIR      = /nfsd/rop/sw/ibm/cos221/cplex
CONCERTLIBDIR = /nfsd/rop/sw/ibm/cos221/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CC  = gcc

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

COPT  = -m64 -fPIC -I/nfsd/rop/sw/ibm/cos221/cplex/include

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

# For dynamic linking
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIB      = cplex$(dynamic:yes=2210)
run           = $(dynamic:yes=LD_LIBRARY_PATH=$(CPLEXBINDIR))

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CLNDIRS   = -L$(CPLEXLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CCLNFLAGS = -lconcert -lilocplex -l$(CPLEXLIB) -lm -lpthread -ldl
CLNFLAGS  = -l$(CPLEXLIB) -lm -lpthread -ldl


CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = /home/farrukujur/tesi
EXINC         = /nfsd/rop/sw/ibm/cos221/cplex/include
EXDATA        = /home/farrukujur/tesi/file_test
EXSRCC        = /home/farrukujur/tesi
EXSRCCX       = $(EXDIR)/src/c_x
EXSRCCPP      = $(EXDIR)/src/cpp
EXSRCJAVA     = $(EXDIR)/src/java

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR)
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 
JCFLAGS = $(JOPT)


#------------------------------------------------------------
#  make execute = esegue il programma
#------------------------------------------------------------

C_EX = prova2

execute: $(C_EX)  
	 $(run)./prova2 co-100.mps.gz
	 

# ------------------------------------------------------------
 # make clean = elimina i file creati dal make 

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf $(C_EX) $(CX_EX) $(CPP_EX)

# ------------------------------------------------------------

#
	
	
prova2: prova2.o
	$(CC) $(CFLAGS) $(CLNDIRS) -o prova2 prova2.o $(CLNFLAGS)
prova2.o: $(EXSRCC)/prova2.c
	$(CC) -c $(CFLAGS) $(EXSRCC)/prova2.c -o prova2.o




# Local Variables:
# mode: makefile
# End:
