FROM python:3.12.4-slim AS base

WORKDIR /app
COPY requirements.txt .

RUN pip install --upgrade pip \
    && pip install --no-cache-dir -r requirements.txt

COPY docker-cmd.sh .
RUN chmod a+x docker-cmd.sh
COPY ./checkoff_python ./checkoff_python

EXPOSE 3000

CMD ["./docker-cmd.sh"]

FROM gcr.io/distroless/python3
WORKDIR /app

COPY --from=base /app /app
COPY --from=base /usr/local/lib/python3.12/site-packages /usr/local/lib/python3.12/site-packages
COPY standalone.py .

ENV PYTHONPATH=/usr/local/lib/python3.12/site-packages
CMD ["standalone.py"]
