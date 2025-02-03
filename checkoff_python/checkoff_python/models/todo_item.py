from peewee import CharField, BooleanField
from .base import BaseModel
from marshmallow_peewee import ModelSchema


class TodoItem(BaseModel):
    title = CharField()
    details = CharField()
    isComplete = BooleanField()


class TodoItemSchema(ModelSchema):
    class Meta():
        model = TodoItem
