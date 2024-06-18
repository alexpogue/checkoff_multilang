from .base import db
from .base import ma

from marshmallow import fields

class TodoItem(db.Model):
    __tablename__ = 'todo_item'

    id = db.Column(db.Integer, index=True, nullable=False, primary_key=True)
    title = db.Column(db.String(200), nullable=False)
    details = db.Column(db.String(200), nullable=False)
    isComplete = db.Column(db.Boolean, nullable=False)

class TodoItemSchema(ma.Schema):
    id = fields.Integer()
    title = fields.String()
    details = fields.String()
    isComplete = fields.Boolean()

todo_item_schema = TodoItemSchema()
todo_items_schema = TodoItemSchema(many=True)
