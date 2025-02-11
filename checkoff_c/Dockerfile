# arg included so we can copy an SSL certificate needed for Rust build on original developer's machine
ARG BUILD_ENV="copy"

# The seemingly hacky lines below do a conditional copy based on the `BUILD_ENV` build argument. Strategy uses multi-stage builds (inspiration: https://stackoverflow.com/a/54245466)
FROM debian:bookworm-slim AS build_no_copy
# no-op

FROM debian:bookworm-slim AS build_copy
COPY certs/ certs/
RUN apt-get update && apt-get install -y --no-install-recommends \
  ca-certificates \
  && rm -rf /var/lib/apt/lists/* \
  && apt-get autoremove -y && apt-get clean

ONBUILD RUN cat certs/ZscalerRootCertificate-2048-SHA256.pem > /etc/ssl/certs/ZscalerRootCertificate-2048-SHA256.pem

FROM build_${BUILD_ENV} AS build_intermediate

RUN apt-get update && apt-get install -y --no-install-recommends \
  libjansson-dev libcurl4-gnutls-dev libgnutls28-dev libgcrypt20-dev zlib1g-dev libsystemd-dev wget build-essential cmake libgcc-s1 musl-tools xz-utils m4 pkg-config\
  && rm -rf /var/lib/apt/lists/* \
  && apt-get autoremove -y && apt-get clean

WORKDIR /app

RUN wget --no-check-certificate https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-1.0.1.tar.gz \
  && tar zxvf libmicrohttpd-1.0.1.tar.gz \
  && cd libmicrohttpd-1.0.1 \
  && CC=musl-gcc ./configure \
  && make \
  && make install \
  && make clean

RUN wget --no-check-certificate https://github.com/babelouest/orcania/archive/refs/tags/v2.3.3.tar.gz \
  && tar zxvf v2.3.3.tar.gz \
  && cd orcania-2.3.3 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make CC=musl-gcc \
  && make install \
  && make clean

RUN wget --no-check-certificate https://github.com/babelouest/yder/archive/refs/tags/v1.4.20.tar.gz \
  && tar zxvf v1.4.20.tar.gz \
  && cd yder-1.4.20 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make CC=musl-gcc \
  && make install \
  && make clean

RUN wget --no-check-certificate https://ftp.gnu.org/gnu/nettle/nettle-3.10.tar.gz \
  && tar zxvf nettle-3.10.tar.gz \
  && cd nettle-3.10 \
  && CC=musl-gcc ./configure --host=x86_64-linux-musl --prefix=/usr/local --enable-static --disable-shared \
  && make \
  && make check \
  && make install \
  && make clean

RUN wget --no-check-certificate https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz \
  && tar xf gmp-6.3.0.tar.xz \
  && cd gmp-6.3.0 \
  && CC=musl-gcc ./configure --host=x86_64-linux-musl CFLAGS="-I/usr/local/include -L/usr/local/lib" \
  && make \
  && make check \
  && make install \
  && make clean

#RUN wget --no-check-certificate https://www.gnupg.org/ftp/gcrypt/gnutls/v3.7/gnutls-3.7.11.tar.xz \
#  && tar xf gnutls-3.7.11.tar.xz \
#  && cd gnutls-3.7.11 \
#  && CC=musl-gcc PKG_CONFIG_PATH="/app/nettle-3.10:$PKG_CONFIG_PATH" ./configure --host=x86_64-linux-musl --prefix=/usr/local --enable-static --disable-shared --with-included-libtasn1 --with-included-unistring --with-libgmp-prefix=/usr/local \
#  && make \
#  && make check \
#  && make install \
#  && make clean

RUN echo "/usr/local/lib" >> /etc/ld.so.conf \
  && ldconfig -v

RUN wget --no-check-certificate https://github.com/babelouest/ulfius/archive/refs/tags/v2.7.15.tar.gz \
  && tar zxvf v2.7.15.tar.gz \
  && cd ulfius-2.7.15 \
  && mkdir build \
  && cd build \
  && cmake -DWITH_CURL=off -DWITH_WEBSOCKET=off -DBUILD_UWSC=off -DCMAKE_MODULE_PATH=/orcania-2.3.3/cmake-modules\;/yder-1.4.20/cmake-modules .. \
  && make CC=musl-gcc \
  && make install \
  && make clean
  #&& cmake -DWITH_CURL=off -DWITH_WEBSOCKET=off -DBUILD_UWSC=off -DBUILD_STATIC=on -DCMAKE_MODULE_PATH=/orcania-2.3.3/cmake-modules\;/yder-1.4.20/cmake-modules .. \

# We install with musl-tools apt-get package instead
#RUN wget --no-check-certificate https://musl.libc.org/releases/musl-1.2.5.tar.gz \
#  && tar zxvf musl-1.2.5.tar.gz \
#  && cd musl-1.2.5 \
#  && ./configure --prefix="/app/musl" --exec-prefix="/usr/local" \
#  && make \
#  && make install \
#  && make clean

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

COPY main.c .
#RUN gcc -static -static-libgcc -static-libstdc++ -o main main.c -L/usr/local/lib -L/usr/lib/gcc/x86_64-linux-gnu/12/ -lulfius
RUN gcc -Wl,-verbose=99 -o main main.c -L/usr/local/lib -L/usr/lib/gcc/x86_64-linux-gnu/12/ -lulfius
#RUN musl-gcc -Wl,-verbose=99 -static -static-libgcc -static-libstdc++ -o main main.c -L/usr/local/lib -L/usr/lib/gcc/x86_64-linux-gnu/12/ -lulfius
#RUN gcc -Wl,-Bstatic -o main main.c -L/usr/local/lib -lulfius -L/usr/lib/gcc/x86_64-linux-gnu/12/ -lgcc_s
#CMD ["./main"]
CMD ["sleep", "1d"]
