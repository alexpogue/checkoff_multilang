ARG SSL_CERT=""

FROM rust:latest AS build
WORKDIR /app
COPY . .
RUN cat certs/ZscalerRootCertificate-2048-SHA256.pem >> /usr/lib/ssl/certs/ca-certificates.crt
RUN cargo build
CMD cargo run
