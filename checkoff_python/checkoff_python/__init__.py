from flask import Flask

def create_app(config_file=None):
    from . import models, routes
    app = Flask(__name__)

    app.config.from_pyfile(config_file)

    models.init_app(app)
    routes.init_app(app)
    return app
