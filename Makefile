IMG = pwn-kaname-messager
CTN = messager

build:
	docker build -t $(IMG) .

export: build
	mkdir -p export
	docker run --rm --entrypoint cat $(IMG) /srv/messager > export/messager
	cp src/messager.c export/messager.c

up: build
	docker run -d -p 5555:5555 --env-file flag.env --name $(CTN) $(IMG)

down:
	-docker rm -f $(CTN)

logs:
	docker logs -f $(CTN)

clean: down
	-docker rmi $(IMG)
	rm -rf export

.PHONY: build export up down logs clean
