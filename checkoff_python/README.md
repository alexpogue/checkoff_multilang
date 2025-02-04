## Setup

### Setup docker network for MySql and main container to connect

`docker network create --driver bridge checkoff_network`

### Setup MySql container
MySql database needed. Use this tutorial to get mysql database running on localhost using docker: https://www.datacamp.com/tutorial/set-up-and-configure-mysql-in-docker

`docker run -d --network=checkoff_network --name checkoff-mysql -e MYSQL_ROOT_PASSWORD=strong_password -p 3307:3306 mysql`
`docker exec -it checkoff-mysql bash`
`mysql -u root -p` and enter `strong_password` as password

in mysql command line exec, run:

`create database checkoff_py;` # Create database
`quit` # quit mysql
`ctrl-D` # leave docker shell

## Running 

### Running on local OS

#### Setup

From project root directory:

```
virtualenv venv
pip install -r requirements.txt
. venv/bin/activate
```

#### Running

Make sure database url is set to connect to "127.0.0.1" in `checkoff_python/config.py`

From project root directory:
`python main.py`

###  Running locally on docker

Make sure database url is set to connect to "checkoff-mysql" in `checkoff_python/config.py`

```
docker build -t checkoff_python:latest .
docker run -it --network=checkoff_network -p 3000:3000 checkoff_python:latest
```

### Running standalone tiny docker image

```
docker build -f Dockerfile.standalone -t checkoff_python:standalone .
docker run -it --network=checkoff_network -p 3000:3000 --name checkoff_python checkoff_python:standalone
```

#### Optional: Building and pushing a new python_custom image

Only need to do this when we need to change Python version or optimize the build's base image.

```
docker build -f Dockerfile.python-bookworm-slim-disable-dtags -t python_custom:bookworm-slim-disable-dtags .
docker tag python_custom:bookworm-slim-disable-dtags alexpogue/python_custom:bookworm-slim-disable-dtags
docker push alexpogue/python_custom:bookworm-slim-disable-dtags
```

#### Optional: Building and pushing a new checkoff_python_base image

Only need to do this when we need to change our dependencies (mysqlclient is notable), PyInstaller, or staticx.

```
docker build -f Dockerfile.base -t checkoff_python_base:latest .
docker tag checkoff_python_base:latest alexpogue/checkoff_python_base:latest
docker push alexpogue/checkoff_python_base:latest
```


### exec into into the standalone docker image
```
./exec-standalone.sh
```

## Interacting via curl

(See checkoff_rust instructions)

