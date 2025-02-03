from flask import Flask

import os

def create_app(config_file=None):
    print(f'PYTHONMALLOC = {os.getenv("PYTHONMALLOC")}')

    from . import models, routes
    app = Flask(__name__)

    app.config.from_pyfile(config_file)

    # Turn off strict_slashes to treat `/todo-item` as valid endpoint. Without this setting, it would 304 redirect to `/todo-item/`.
    # Default Rust Axum doesn't include the ending slash, and there is no hard-and-fast rule for REST APIs
    app.url_map.strict_slashes = False

    models.init_app()
    routes.init_app(app)
    return app
