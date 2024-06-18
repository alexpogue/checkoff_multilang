from .base import db
from .base import ma

from .todo_item import TodoItem

def init_app(app):
    ma.init_app(app)
    db.init_app(app)

    with app.app_context():
        db.create_all()
