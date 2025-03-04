all: build run

build:
	g++ -o cli.out -std=c++23 -Wall *.c canvas/*.cpp -lcurl

clean:
	rm cli.bin

run:
	# This access token is for a private self-hosted Canvas instance. Don't bother trying to hack me.
	./cli.out https://canvas.tail7fcea.ts.net 83M9TV3fvCheJQMPCVcJYRYWeJ9QTu8cWKRu6m2ZZE7ryXPukNzCuEChKaDVErUC
