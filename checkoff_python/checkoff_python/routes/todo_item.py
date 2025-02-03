from flask import Blueprint, jsonify, request, abort

from ..models.todo_item import TodoItem, TodoItemSchema

from .util import get_by_id, ensure_json_or_die

todo_item_blueprint = Blueprint('todo_items', __name__)


@todo_item_blueprint.route('/')
def get_todo_items():
    all_todo_items = TodoItem.select()
    all_todo_items_json = TodoItemSchema().dump(all_todo_items, many=True)
    return jsonify({'status': 'success', 'data': all_todo_items_json})


@todo_item_blueprint.route('/<int:todo_item_id>')
def get_todo_item(todo_item_id):
    todo_item = get_by_id(TodoItem, todo_item_id, TodoItemSchema())
    return jsonify({'status': 'success', 'data': todo_item})

@todo_item_blueprint.route('/', methods=['POST'])
def new_todo_item():
    ensure_json_or_die()
    request_data = request.get_json()

    title = request_data['title']
    details = request_data['details']
    is_complete = request_data['isComplete']
    todo_item = TodoItem(title=title, details=details, isComplete=is_complete)
    todo_item.save()

    return jsonify({'status': 'success'})

@todo_item_blueprint.route('/<int:todo_item_id>', methods=['PUT'])
def update_todo_item(todo_item_id):
    ensure_json_or_die()
    request_data = request.get_json()

    try:
        todo_item = TodoItem.get_by_id(todo_item_id)
    except TodoItem.DoesNotExist:
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

    todo_item.save()
    return jsonify({'status': 'success'})

@todo_item_blueprint.route('/<int:todo_item_id>', methods=['DELETE'])
def delete_todo_item(todo_item_id):
    try:
        todo_item = TodoItem.get_by_id(todo_item_id)
    except TodoItem.DoesNotExist:
        abort(404)

    todo_item.delete_instance()
    return jsonify({'status': 'success'})
