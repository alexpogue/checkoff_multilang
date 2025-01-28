# arg included so we can copy an SSL certificate needed for Rust build on original developer's machine
ARG BUILD_ENV="copy"
ARG UPX_FLAG="no_upx"

# can be "scratch" or "distroless"
ARG BUILD_TYPE="distroless"

# The seemingly hacky lines below do a conditional copy based on the `BUILD_ENV` build argument. Strategy uses multi-stage builds (inspiration: https://stackoverflow.com/a/54245466)
FROM rust:latest AS build_no_copy
# no-op

FROM rust:latest AS build_copy
COPY certs/ certs/
ONBUILD RUN cat certs/ZscalerRootCertificate-2048-SHA256.pem >> /usr/lib/ssl/certs/ca-certificates.crt

FROM build_${BUILD_ENV} AS build_intermediate
# no-op, collect copy vs no_copy builds

FROM build_intermediate AS prep_musl_vars_scratch
ENV EXECUTABLE_DIR="/app/target/x86_64-unknown-linux-musl/release"
ENV CARGO_BUILD_TARGET="x86_64-unknown-linux-musl"
RUN rustup target add ${CARGO_BUILD_TARGET}

FROM build_intermediate AS prep_musl_vars_distroless
ENV EXECUTABLE_DIR="/app/target/release"

FROM prep_musl_vars_${BUILD_TYPE} AS build
WORKDIR /app

# Set up sccache for caching rust deps, see https://medium.com/@dillen.dev/rusty-docker-never-again-fe24e262ab18
ENV SCCACHE_VERSION=0.4.1
RUN wget -O sccache.tar.gz https://github.com/mozilla/sccache/releases/download/v${SCCACHE_VERSION}/sccache-v${SCCACHE_VERSION}-x86_64-unknown-linux-musl.tar.gz && \
  tar xzf sccache.tar.gz && \
  mv sccache-v*/sccache /usr/local/bin/sccache && \
  chmod +x /usr/local/bin/sccache
ENV RUSTC_WRAPPER=/usr/local/bin/sccache

# install upx for Dockerfile cache despite only using for UPX_FLAG=upx builds
RUN wget --no-check-certificate https://github.com/upx/upx/releases/download/v4.2.4/upx-4.2.4-amd64_linux.tar.xz
RUN tar -xf upx-4.2.4-amd64_linux.tar.xz

RUN apt-get update && apt-get install -y --no-install-recommends \
  xz-utils musl-tools \
  && rm -rf /var/lib/apt/lists/* \
  && apt-get autoremove -y && apt-get clean

# Compile dependencies before main binary, see https://medium.com/@dillen.dev/rusty-docker-never-again-fe24e262ab18
RUN cargo init
COPY Cargo.toml Cargo.lock ./
RUN --mount=type=cache,target=/root/.cache cargo fetch && \
  cargo build --release && \
  rm src/*.rs

COPY src src

RUN --mount=type=cache,target=/root/.cache touch src/main.rs && \
  cargo build --release

FROM build AS build_upx
WORKDIR /app
RUN ./upx-4.2.4-amd64_linux/upx -9 -o ${EXECUTABLE_DIR}/checkoff_rust_small ${EXECUTABLE_DIR}/checkoff_rust
RUN mv ${EXECUTABLE_DIR}/checkoff_rust_small ${EXECUTABLE_DIR}/checkoff_rust

FROM build AS build_no_upx
# no-op

FROM build_${UPX_FLAG} AS final
RUN mv ${EXECUTABLE_DIR}/checkoff_rust /app/checkoff_rust

from scratch AS end_build_scratch
# no-op, used to select base image based on BUILD_TYPE

from gcr.io/distroless/cc as end_build_distroless
# no-op, used to select base image based on BUILD_TYPE

from end_build_${BUILD_TYPE}

WORKDIR /app
COPY --from=final /app/checkoff_rust ./checkoff_rust
ENV RUST_BACKTRACE=full

CMD ["/app/checkoff_rust"]
