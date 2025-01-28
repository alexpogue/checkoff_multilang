package main

import (
  "gorm.io/driver/mysql"
  "gorm.io/gorm"
  "gorm.io/gorm/schema"
  "encoding/json"
  "net/http"
  "log"
  "strconv"
)

type TodoItem struct {
  ID int `json:"id" gorm:"primaryKey;autoIncrement"`
  Title string `json:"title" gorm:"not null"`
  Details string `json:"details" gorm:"not null"`
  IsComplete bool `json:"isComplete" gorm:"column:isComplete;not null"`
}

type StatusResponse struct {
  Status string `json:"status"`
}

type DataStatusResponse struct {
  Data StatusResponse `json:"data"`
}

func connectToDb() (*gorm.DB, error) {
//  dsn := "root:strong_password@tcp(localhost:3307)/checkoff"
  dsn := "root:strong_password@tcp(checkoff-mysql:3306)/checkoff_py"
  db, err := gorm.Open(mysql.Open(dsn), &gorm.Config{
    NamingStrategy: schema.NamingStrategy {
      SingularTable: true, // name table "todo_item" instead of "todo_items"
    }})
  if err != nil {
    return nil, err
  }

  return db, nil
}

func getAllTodoItems(db *gorm.DB) ([]TodoItem, error) {
  db, err := connectToDb()
  if err != nil {
    log.Fatal(err)
  }

  var todoItems []TodoItem
  result := db.Find(&todoItems)
  if result.Error != nil {
    return nil, result.Error
  }
  return todoItems, nil
}

func createTodoItem(db *gorm.DB, todoItem *TodoItem) error {
  result := db.Create(todoItem)
  if result.Error != nil {
    return result.Error
  }
  return nil
}

func updateTodoItem(db *gorm.DB, todoItem *TodoItem) error {
  result := db.Save(todoItem)
  if result.Error != nil {
    return result.Error
  }
  return nil
}

func getTodoItemByID(db *gorm.DB, todoItemID uint) (*TodoItem, error) {
  var todoItem TodoItem;
  result := db.First(&todoItem, todoItemID)
  if result.Error != nil {
    return nil, result.Error
  }
  return &todoItem, nil
}

func deleteTodoItemByID(db *gorm.DB, id uint) error {
  todoItem, err := getTodoItemByID(db, id)
  if err != nil {
    return err
  }

  result := db.Delete(&todoItem)
  if result.Error != nil {
    return result.Error
  }
  return nil
}

func main() {
  db, err := connectToDb()
  if err != nil {
    log.Fatal(err)
  }
  err = db.AutoMigrate(&TodoItem{})

  http.HandleFunc("/todo_item", todoItemHandler)
  http.HandleFunc("/todo_item/{id}", todoItemParameterHandler)

  log.Println("Starting server on :8080")
  log.Fatal(http.ListenAndServe(":8080", nil))
}

func todoItemHandler(w http.ResponseWriter, r *http.Request) {
  switch r.Method {
    case http.MethodGet:
      getAllTodoItemsHandler(w, r)
    case http.MethodPost:
      addTodoItemHandler(w, r)
    default:
      http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
  }
}

func todoItemParameterHandler(w http.ResponseWriter, r *http.Request) {
  switch r.Method {
    case http.MethodGet:
      getTodoItemByIDHandler(w, r)
    case http.MethodDelete:
      deleteTodoItemHandler(w, r)
    default:
      http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
  }
}

func getAllTodoItemsHandler(w http.ResponseWriter, r *http.Request) {
  db, err := connectToDb()
  if err != nil {
    log.Fatal(err)
  }

  todoItems, err := getAllTodoItems(db)
  if err != nil {
    log.Fatal(err)
  }

  w.Header().Set("Content-Type", "application/json")
  json.NewEncoder(w).Encode(todoItems)
}

func getTodoItemByIDHelper(id uint) (*TodoItem, error) {
  db, err := connectToDb()
  if err != nil {
    return nil, err
  }

  todoItem, err := getTodoItemByID(db, id)
  if err != nil {
    return nil, err
  }

  return todoItem, nil
}

func getTodoItemByIDStringHelper(idString string) (*TodoItem, error) {
  idInt64, err := strconv.ParseUint(idString, 10, 32)
  id := uint(idInt64)

  todoItem, err := getTodoItemByIDHelper(id)
  if err != nil {
    return nil, err
  }

  return todoItem, nil
}

func getTodoItemByIDHandler(w http.ResponseWriter, r *http.Request) {
  idToGetStr := r.PathValue("id")
  todoItem, err := getTodoItemByIDStringHelper(idToGetStr)
  if err != nil {
    log.Fatal(err)
  }
  w.Header().Set("Content-Type", "application/json")
  json.NewEncoder(w).Encode(todoItem)
}

func addTodoItemHandler(w http.ResponseWriter, r *http.Request) {
  if r.Method != http.MethodPost {
    http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
    return
  }

  var todoItem TodoItem
  if err := json.NewDecoder(r.Body).Decode(&todoItem); err != nil {
    http.Error(w, err.Error(), http.StatusBadRequest)
    return
  }
  log.Println("todoItem = ", todoItem)

  db, err := connectToDb()
  if err != nil {
    log.Fatal(err)
  }

  err = createTodoItem(db, &todoItem)
  if err != nil {
    log.Fatal(err)
  }

  w.Header().Set("Content-Type", "application/json")
  json.NewEncoder(w).Encode(todoItem)
}

func deleteTodoItemByIdStringHelper(idStr string) error {
  idInt64, err := strconv.ParseUint(idStr, 10, 32)
  id := uint(idInt64)

  db, err := connectToDb()
  if err != nil {
    return err
  }

  err = deleteTodoItemByID(db, id)
  if err != nil {
    return err
  }

  return nil
}

func deleteTodoItemHandler(w http.ResponseWriter, r *http.Request) {
  idToDeleteStr := r.PathValue("id")

  err := deleteTodoItemByIdStringHelper(idToDeleteStr)
  if err != nil {
    log.Fatal(err)
  }

  if r.Method != http.MethodDelete {
    http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
    return
  }

  successResponse := DataStatusResponse{Data: StatusResponse{Status: "success"}}
  w.Header().Set("Content-Type", "application/json")
  json.NewEncoder(w).Encode(successResponse)
}
