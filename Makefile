all: gniolf

gniolf: 
	gcc -g main.c -o gniolf -lSDL2 -lSDL2_image

clean:
	rm gniolf