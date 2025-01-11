# This script runs single-threaded before the actual flask app starts.
# If we try this initialization inside the flask app, we get database
# race conditions

# e.g. multiple worker threads all try (1) check table existence, and then
# (2) create the table.. If two threads do (1) at the same time, both will try
# to create the table, and one will fail to do so.

# I found https://github.com/sqlalchemy/sqlalchemy/pull/5663 which might solve
# the issue, but I couldn't find the code in the main branch, despite the PR
# showing closed.

import json
from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from checkoff_python.models.base import db

def init_databases():
    config_file = 'trex_uptime_monitor/config.py'

    placeholder_app = Flask('placeholder_app')
    placeholder_app.config.from_pyfile(config_file)

    init_databases(placeholder_app)

    from trex_uptime_monitor import models
    models.init_app(placeholder_app)

    with placeholder_app.app_context():
        db.create_all()

if __name__ == '__main__':
    init_databases()
