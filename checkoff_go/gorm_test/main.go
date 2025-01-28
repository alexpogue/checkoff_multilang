package main

import (
  "gorm.io/driver/sqlite"
  "gorm.io/gorm"
  "log"
)

func connectToSQLite() (*gorm.DB, error) {
  db, err := gorm.Open(sqlite.Open("checkoff.db"), &gorm.Config{})
  if err != nil {
    return nil, err
  }

  return db, nil
}

type TodoItem struct {
  ID uint `gorm:"primaryKey"`
  Title string
  Details string
  IsComplete bool
}

func createTodoItem(db *gorm.DB, todoItem *TodoItem) error {
  result := db.Create(todoItem)
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

func updateTodoItem(db *gorm.DB, todoItem *TodoItem) error {
  result := db.Save(todoItem)
  if result.Error != nil {
    return result.Error
  }
  return nil
}

func deleteTodoItem(db *gorm.DB, todoItem *TodoItem) error {
  result := db.Delete(todoItem)
  if result.Error != nil {
    return result.Error
  }
  return nil
}

func main() {
  db, err := connectToSQLite()
  if err != nil {
    log.Fatal(err)
  }
  //defer db.Close()

  // perform database migration
  err = db.AutoMigrate(&TodoItem{})
  if err != nil {
    log.Fatal(err)
  }

  newTodoItem := &TodoItem{Title: "john_doe", Details: "john.doe@example.com", IsComplete: false}
  err = createTodoItem(db, newTodoItem)
  if err != nil {
    log.Fatal(err)
  }
  log.Println("Created TodoItem:", newTodoItem)

  todoItemID := newTodoItem.ID
  todoItem, err := getTodoItemByID(db, todoItemID)
  if err != nil {
    log.Fatal(err)
  }
  log.Println("TodoItem by ID:", todoItem)

  todoItem.Details = "updated_email@example.com"
  err = updateTodoItem(db, todoItem)
  if err != nil {
    log.Fatal(err)
  }
  log.Println("Updated TodoItem:", todoItem)

  err = deleteTodoItem(db, todoItem)
  if err != nil {
    log.Fatal(err)
  }
  log.Println("Deleted TodoItem:", todoItem)
}
