# checkoff_c

Currently this is only a "hello world" web service, but the Ulfius library has potential to be expanded.

It includes jansson for JSON support, and I assume we can use a straight c database library to contact mysql.

Currently the only complete docker image is a "large" glibc-based one. Smaller musl-based is only partially done.

# Building glibc version

`docker build -f Dockerfile.glibc -t checkoff_c:glibc .`

# Building alpine version

`docker build -f Dockerfile.scratch -t checkoff_c:scratch .`

# Running

`docker run --init -p8080:8080 -it checkoff_c:glibc`

or 

`docker run --init -p8080:8080 -it checkoff_c:scratch`

# Browsing hello world page

Navigate browser to http://localhost:8080/helloworld

# Json curl requesting

This command:

`curl -XPOST -H'Content-Type: application/json' localhost:8080/json -d'{"name": "hi"}'`

should return `{"status":"success","name":"hi"}`

### Note:

Smaller musl image is deprioritized because libmicrohttpd is slower in benchmarks than rust and rust is a nicer language. The C version might be smaller docker image size, but that is also unknown. If we need a tiny full-featured web server, this might be worth completing.

TODO: Continue compiling all Ulfius dependencies using the musl toolchain. The main Dockerfile has begun this process.
