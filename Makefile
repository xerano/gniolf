all: gniolf

gniolf: 
	gcc -g main.c -o gniolf -lSDL2 -lSDL2_image -lSDL2_ttf

clean:
	rm gniolf