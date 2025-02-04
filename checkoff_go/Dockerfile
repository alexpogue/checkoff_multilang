FROM golang:1.24rc2 as builder

WORKDIR /app
COPY . .
RUN CGO_ENABLED=0 GOOS=linux go build -ldflags="-s -w" -a -installsuffix cgo -o go-api /app/src/main.go

FROM scratch
WORKDIR /app
COPY --from=builder /app/go-api .
EXPOSE 8080
CMD ["./go-api"]


