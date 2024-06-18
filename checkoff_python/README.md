## Setup

### Setup MySql container
MySql database needed. Use this tutorial to get mysql database running on localhost using docker: https://www.datacamp.com/tutorial/set-up-and-configure-mysql-in-docker

`docker run -d --name checkoff-mysql -e MYSQL_ROOT_PASSWORD=strong_password -p 3307:3306 mysql`
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

From project root directory:

`python main.py`

###  Running locally on docker

```
docker build -t checkoff_python:latest .
docker run -it -p 3000:3000 checkoff_rust:latest
```

## Interacting via curl

(See checkoff_rust instructions)

