tutorial: https://www.twilio.com/en-us/blog/build-high-performance-rest-apis-rust-axum

## Setup

MySql database needed. Use this tutorial to get mysql database running on localhost using docker: https://www.datacamp.com/tutorial/set-up-and-configure-mysql-in-docker

`docker run -d --name test-mysql -e MYSQL_ROOT_PASSWORD=strong_password -p 3307:3306 mysql`
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

## Running

### Running on local OS

cargo build
cargo run

### Running locally on docker

```
docker build -t checkoff_rust:latest .
docker run -it -p 3000:3000 checkoff_rust:latest
```

## Interacting via curl

Create todo item:
`curl -H'Content-Type: application/json' -XPOST -d'{"title": "Dishes", "details": "Do them!", "isComplete": false}' localhost:3000/todo-item`

Look up all todo items:
`curl localhost:3000/todo-item`

Look up todo item 1:
`curl localhost:3000/todo-item/1`

Edit todo item 1:
`curl -H'Content-Type: application/json' -XPUT -d'{"title": "Make bed", "details": "Start morning right!", "isComplete": false}' localhost:3000/todo-item/1`
 
Delete todo item 1:
`curl -XDELETE localhost:3000/todo-item/1`

