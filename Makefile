SOURCES = src/main.o src/line.o src/script.o src/table.o src/cluster.o

all: oat

oat: $(SOURCES)
	@ echo " LD	" $@
	@ $(CC) $(SOURCES) -o $@

%.o: %.c
	@ echo " CC	" $@
	@ $(CC) $(CFLAGS) -c $< -o $@

.PHONY: all

src/main.o: src/oatmeal.h
src/line.o: src/oatmeal.h
src/script.o: src/oatmeal.h
src/table.o: src/oatmeal.h
src/cluster.o: src/oatmeal.h
