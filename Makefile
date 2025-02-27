all: build run

build:
	g++ -o cli.out -std=c++11 -Wall -lcurl *.c canvas/*.cpp

clean:
	rm cli.bin

run:
	# This access token is for a private self-hosted Canvas instance. Don't bother trying to hack me.
	./cli.out 83M9TV3fvCheJQMPCVcJYRYWeJ9QTu8cWKRu6m2ZZE7ryXPukNzCuEChKaDVErUC
