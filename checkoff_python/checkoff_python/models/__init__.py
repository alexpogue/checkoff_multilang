from .base import db

from .todo_item import TodoItem

def init_app():
    with db:
        db.create_tables([TodoItem])
