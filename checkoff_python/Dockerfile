FROM python:3.12.4-slim AS base

WORKDIR /app
COPY requirements.txt .
COPY docker-cmd.sh .

RUN pip install --upgrade pip \
    && pip install --no-cache-dir -r requirements.txt \
    && chmod a+x docker-cmd.sh

COPY ./checkoff_python ./checkoff_python

EXPOSE 3000

CMD ["./docker-cmd.sh"]
