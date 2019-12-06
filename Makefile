SRCS:=main.c collison.c

all: gniolf

gniolf: 
	gcc -g $(SRCS) -o gniolf -lSDL2 -lSDL2_image -lSDL2_ttf

.PHONY: all clean

clean:
	rm gniolf
