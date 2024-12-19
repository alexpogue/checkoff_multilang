# arg included so we can copy an SSL certificate needed for Rust build on original developer's machine
ARG BUILD_ENV="copy"

# The seemingly hacky lines below do a conditional copy based on the `BUILD_ENV` build argument. Strategy uses multi-stage builds (inspiration: https://stackoverflow.com/a/54245466)
FROM rust:latest AS build_no_copy
# no-op

FROM rust:latest AS build_copy
COPY certs/ certs/
ONBUILD RUN cat certs/ZscalerRootCertificate-2048-SHA256.pem >> /usr/lib/ssl/certs/ca-certificates.crt

FROM build_${BUILD_ENV} AS build
WORKDIR /app
COPY . .
RUN cargo build --release
#RUN cargo build
#CMD cargo run

from gcr.io/distroless/cc

WORKDIR /app
COPY --from=build /app/target/release/checkoff_rust ./
#COPY --from=build /app/target/debug/checkoff_rust ./
ENV RUST_BACKTRACE=full

CMD ["/app/checkoff_rust"]
