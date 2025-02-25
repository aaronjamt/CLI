build:
	g++ -o cli.out -std=c99 -Wall *.c canvas/*.cpp

clean:
	rm cli.bin
