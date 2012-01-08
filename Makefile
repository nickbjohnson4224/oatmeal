SOURCES = main.o line.o script.o table.o cluster.o

all: oat

oat: $(SOURCES)
	@ echo " LD	" $@
	@ $(CC) $(SOURCES) -o $@

%.o: %.c
	@ echo " CC	" $@
	@ $(CC) $(CFLAGS) -c $< -o $@

.PHONY: all

main.o: oatmeal.h
line.o: oatmeal.h
script.o: oatmeal.h
table.o: oatmeal.h
cluster.o: oatmeal.h
