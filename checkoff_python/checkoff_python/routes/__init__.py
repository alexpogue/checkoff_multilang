from .todo_item import todo_item_blueprint


def init_app(app):
    app.register_blueprint(todo_item_blueprint, url_prefix='/todo-item')
