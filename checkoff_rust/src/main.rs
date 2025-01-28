use axum::{
    extract::{Path, Extension},
    response::IntoResponse,
    routing::{get, post, put, delete},
    Json,
    Router
};
use tower_http::cors::CorsLayer;
use serde::Serialize;
use serde::Deserialize;
use sqlx::{MySqlPool, Row};
use mimalloc::MiMalloc;

#[global_allocator]
static GLOBAL: MiMalloc = MiMalloc;

#[derive(Serialize)]
#[derive(Debug)]
#[allow(non_snake_case)]
struct TodoItem {
    id: i32,
    title: String,
    details: String,
    isComplete: bool,
}

#[derive(Serialize)]
struct SingleTodoItemResponse {
    status: String,
    data: TodoItem,
}

#[derive(Serialize)]
struct MultipleTodoItemsResponse {
    status: String,
    data: Vec<TodoItem>,
}

#[derive(Serialize)]
struct StatusResponse {
    status: String,
}

#[derive(Deserialize)]
#[derive(Debug)]
#[allow(non_snake_case)]
struct TodoItemInsert {
    title: String,
    details: String,
    isComplete: bool,
}

// Define the get_users function as before
async fn get_todo_items(Extension(pool): Extension<MySqlPool>) -> impl IntoResponse {
    let rows = match sqlx::query("SELECT id, title, details, isComplete FROM todo_item")
        .fetch_all(&pool)
        .await
    {
        Ok(rows) => rows,
        Err(e) => {
            eprintln!("Application error while getting all todo items: {e}");
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        }
    };

    let todo_items: Vec<TodoItem> = rows
        .into_iter()
        .map(|row| {
            TodoItem {
                id: row.try_get::<i32, _>("id").unwrap_or_default(),
                title: row.try_get::<String, _>("title").unwrap_or_default(),
                details: row.try_get::<String, _>("details").unwrap_or_default(),
                isComplete: row.try_get::<bool, _>("isComplete").unwrap_or_default(),
            }
        })
        .collect();

    let todo_items_response = MultipleTodoItemsResponse {
        status: "success".to_string(),
        data: todo_items,
    };

    (axum::http::StatusCode::OK, Json(todo_items_response)).into_response()
}

async fn create_todo_item(Extension(pool): Extension<MySqlPool>, Json(todo_item): Json<TodoItemInsert>) -> impl IntoResponse {
    println!("todo_item = {:?}", todo_item);

    match sqlx::query("INSERT INTO todo_item (title, details, isComplete) VALUES (?, ?, ?)")
        .bind(todo_item.title)
        .bind(todo_item.details)
        .bind(todo_item.isComplete)
        .fetch_all(&pool)
        .await
    {
        Err(e) => {
            eprintln!("Application error: {e}");
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        },
        _ => (),
    }
    (axum::http::StatusCode::OK, Json(StatusResponse {status: "success".to_string()})).into_response()
}

async fn get_todo_item_helper(id: i32, pool: &MySqlPool) -> Result<Option<TodoItem>, String> {
    let row = match sqlx::query("SELECT id, title, details, isComplete FROM todo_item WHERE id=?")
        .bind(id)
        .fetch_all(pool)
        .await
    {
        Ok(row) => row,
        Err(e) => {
            return Err(format!("Database query error. Details: {e}"))
        },
    };
    let todo_item_option: Option<TodoItem> = row
        .into_iter()
        .map(|row| {
            TodoItem {
                id: row.try_get::<i32, _>("id").unwrap_or_default(),
                title: row.try_get::<String, _>("title").unwrap_or_default(),
                details: row.try_get::<String, _>("details").unwrap_or_default(),
                isComplete: row.try_get::<bool, _>("isComplete").unwrap_or_default(),
            }
        })
        .collect::<Vec<TodoItem>>()
        .pop();

    return Ok(todo_item_option)
}

async fn get_todo_item(Extension(pool): Extension<MySqlPool>, Path(id): Path<i32>) -> impl IntoResponse {
    let todo_item = match get_todo_item_helper(id, &pool).await {
        Ok(Some(todo_item)) => todo_item,
        Ok(None) => {
            eprintln!("Application error: could not find todo item with id {id}");
            return (
                axum::http::StatusCode::NOT_FOUND,
                "Not found",
            )
                .into_response()

        },
        Err(e) => {
            eprintln!("Application error occurred getting item with id {id}. Error: {e}");
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        }
    };
    println!("todo_item = {:?}", todo_item);
    let todo_item_response = SingleTodoItemResponse {
        status: "success".to_string(),
        data: todo_item,
    };
    (axum::http::StatusCode::OK, Json(todo_item_response)).into_response()
}


async fn update_todo_item(Extension(pool): Extension<MySqlPool>, Path(id): Path<i32>, Json(todo_item): Json<TodoItemInsert>) -> impl IntoResponse {
    let existing_todo_item = match get_todo_item_helper(id, &pool).await {
        Ok(Some(todo)) => todo,
        Ok(None) =>
            return (
                axum::http::StatusCode::NOT_FOUND,
                "Not found",
            )
                .into_response(),
        Err(e) => {
            eprintln!("Application error occurred getting item with id {id}. Error: {e}");
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        }

    };
    println!("existing todo_item = {:?}", existing_todo_item);

    match sqlx::query("UPDATE todo_item SET title = ?, details = ?, isComplete = ? WHERE id = ?")
        .bind(todo_item.title)
        .bind(todo_item.details)
        .bind(todo_item.isComplete)
        .bind(id)
        .fetch_all(&pool)
        .await
    {
        Err(e) => {
            eprintln!("Application error: {e}");
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        },
        _ => (),
    }

    (axum::http::StatusCode::OK, Json(StatusResponse {status: "success".to_string()})).into_response()
}

async fn delete_todo_item(Extension(pool): Extension<MySqlPool>, Path(id): Path<i32>) -> impl IntoResponse {
    let existing_todo_item = match get_todo_item_helper(id, &pool).await {
        Ok(Some(todo)) => todo,
        Ok(None) =>
            return (
                axum::http::StatusCode::NOT_FOUND,
                "Not found",
            )
                .into_response(),
        Err(e) => {
            eprintln!("Application error occurred getting item with id {id}. Error: {e}");
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        }

    };
    println!("deleting todo_item = {:?}", existing_todo_item);

    match sqlx::query("DELETE FROM todo_item WHERE id = ?")
        .bind(id)
        .fetch_all(&pool)
        .await
    {
        Ok(rows) => rows,
        Err(_) => {
            return (
                axum::http::StatusCode::INTERNAL_SERVER_ERROR,
                "Internal server error",
            )
                .into_response()
        }
    };


    (axum::http::StatusCode::OK, Json(StatusResponse {status: "success".to_string()})).into_response()
}


#[tokio::main]
async fn main() {
    // Set up the database connection pool
    let database_url = "mysql://root:strong_password@checkoff-mysql:3306/checkoff";
    //let database_url = "mysql://root:strong_password@127.0.0.1:3307/checkoff";
    let pool = MySqlPool::connect(&database_url)
        .await
        .expect("Could not connect to the database");

    // Create the Axum router
    let app = Router::new()
        .route("/todo-item", post(create_todo_item))
        .route("/todo-item", get(get_todo_items))
        .route("/todo-item/{id}", get(get_todo_item))
        .route("/todo-item/{id}", put(update_todo_item))
        .route("/todo-item/{id}", delete(delete_todo_item))
        .layer(Extension(pool))
        .layer(CorsLayer::permissive());

    let port=3000;

    // Run the Axum server
    let listener = tokio::net::TcpListener::bind(format!("0.0.0.0:{:?}", port)).await.unwrap();

    println!("Listening on port {:?}", port);

    axum::serve(listener, app).await.unwrap();
    /*
    Server::bind(&"0.0.0.0:3000".parse().unwrap())
        .serve(app.into_make_service())
        .await
        .unwrap();
        */
}
