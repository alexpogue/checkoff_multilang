from flask import Blueprint, jsonify, request, abort

from ..models.base import db
from ..models.todo_item import TodoItem, todo_item_schema, todo_items_schema

from .util import get_by_id, ensure_json_or_die

todo_item_blueprint = Blueprint('todo_items', __name__)


@todo_item_blueprint.route('/')
def get_todo_items():
    all_todo_items = TodoItem.query.all()
    return jsonify({'status': 'success', 'data': todo_items_schema.dump(all_todo_items)})


@todo_item_blueprint.route('/<int:todo_item_id>')
def get_todo_item(todo_item_id):
    todo_item = get_by_id(TodoItem, todo_item_id, todo_item_schema)
    return jsonify({'status': 'success', 'data': todo_item_schema.dump(todo_item)})

@todo_item_blueprint.route('/', methods=['POST'])
def new_todo_item():
    ensure_json_or_die()
    request_data = request.get_json()

    title = request_data['title']
    details = request_data['details']
    is_complete = request_data['isComplete']
    todo_item = TodoItem(title=title, details=details, isComplete=is_complete)

    db.session.add(todo_item)
    db.session.commit()
    return jsonify({'status': 'success'})

@todo_item_blueprint.route('/<int:todo_item_id>', methods=['PUT'])
def update_todo_item(todo_item_id):
    ensure_json_or_die()
    request_data = request.get_json()

    todo_item = TodoItem.query.get(todo_item_id)
    if todo_item is None:
        abort(404)

    new_title = request_data.get('title')
    if new_title is not None:
        todo_item.title = new_title

    new_details = request_data.get('details')
    if new_details is not None:
        todo_item.details = new_details

    new_is_complete = request_data.get('isComplete')
    if new_is_complete is not None:
        todo_item.isComplete = new_is_complete

    db.session.commit()
    return jsonify({'status': 'success'})

@todo_item_blueprint.route('/<int:todo_item_id>', methods=['DELETE'])
def delete_todo_item(todo_item_id):
    todo_item = TodoItem.query.get(todo_item_id)
    if todo_item is None:
        abort(404)

    db.session.delete(todo_item)
    db.session.commit()
    return jsonify({'status': 'success'})
