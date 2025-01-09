tutorial: https://www.twilio.com/en-us/blog/build-high-performance-rest-apis-rust-axum

## Setup

### Setup docker network for MySql and main container to connect

`docker network create --driver bridge checkoff_network`

### Setup MySql container
MySql database needed. Use this tutorial to get mysql database running on localhost using docker: https://www.datacamp.com/tutorial/set-up-and-configure-mysql-in-docker

`docker run -d --network checkoff_network --name checkoff-mysql -e MYSQL_ROOT_PASSWORD=strong_password -p 3307:3306 mysql`
`docker exec -it checkoff-mysql /bin/bash`
`mysql -u root -p`

in mysql command line exec, run:

`create database checkoff;` # Create database
`use checkoff;` # use the database
```
create table todo_item (
  id int primary key auto_increment,
  title varchar(200) not null,
  details varchar(200) not null,
  isComplete bool not null
);
``` # create table with fields
`describe table todo_item;` # describe the table schema

```
insert into todo_item (id, title, details, isComplete)
values (1, "Do the dishes", "Scrub them squeaky clean", false),
  (2, "Take out the trash", "Feels good to be clean", true);
```
`quit` # quit mysql
`ctrl-D` # leave docker shell

## Running

### Running on local OS

Make sure database url is set to connect to "127.0.0.1"

cargo build
cargo run

### Running locally on docker

Make sure database url is set to connect to "checkoff-mysql"

```
docker build -t checkoff_rust:latest .
docker run -it --network=checkoff_network -p 3000:3000 checkoff_rust:latest
```

## Interacting via curl

Create todo item:
`curl -XPOST -H'Content-Type: application/json' -d'{"title": "Dishes", "details": "Do them!", "isComplete": false}' localhost:3000/todo-item`

Look up all todo items:
`curl localhost:3000/todo-item`

Look up todo item 1:
`curl localhost:3000/todo-item/1`

Edit todo item 1:
`curl -XPUT -H'Content-Type: application/json' -d'{"title": "Make bed", "details": "Start morning right!", "isComplete": false}' localhost:3000/todo-item/1`
 
Delete todo item 1:
`curl -XDELETE localhost:3000/todo-item/1`

