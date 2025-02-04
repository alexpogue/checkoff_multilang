# we use 3.11 because that's the latest one in the debian-bookworm apt-get packages
# could change to use ubuntu and add deadsnakes ppa instead.. That ppa doesn't
# support debian though. We only updated to 3.13 to get mimalloc for musl, so
# 3.11 is probably okay here
FROM python:3.11-slim-bookworm AS builder

WORKDIR /app
RUN apt-get update && \
    apt-get install -y --no-install-recommends default-libmysqlclient-dev gcc pkg-config binutils && \
    apt-get autoremove -y && apt-get clean

COPY requirements.txt .

RUN pip install --upgrade pip && \
    pip install --no-cache-dir -r requirements.txt

FROM debian:bookworm-slim

COPY --from=builder /usr/local/lib/python3.11/site-packages /usr/local/lib/python3.11/dist-packages

RUN apt-get update && \
    apt-get install -y --no-install-recommends python3.11 default-libmysqlclient-dev && \
    apt-get autoremove -y && apt-get clean

COPY gunicorn.conf.py .
COPY ./checkoff_python ./checkoff_python
COPY ./standalone.py .

EXPOSE 3000

RUN addgroup nonroot && \
    adduser --ingroup nonroot --uid 1001 --shell /bin/false nonroot
USER nonroot

CMD ["python3.11", "standalone.py"]
