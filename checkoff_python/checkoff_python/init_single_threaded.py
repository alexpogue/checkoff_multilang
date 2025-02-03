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
from checkoff_python.models.base import db

def init_databases():

    # NOTE: The following lines are commented because currently we don't need
    # the app in order to call models.init_app(). We used to need it because
    # database connection string was in the app config. Eventually we should
    # move the connection string back to the app config, and these lines will be
    # necessary again.

    #config_file = 'trex_uptime_monitor/config.py'

    #placeholder_app = Flask('placeholder_app')
    #placeholder_app.config.from_pyfile(config_file)

    #init_databases(placeholder_app)

    from checkoff_python import models
    models.init_app()

    with placeholder_app.app_context():
        db.create_all()

if __name__ == '__main__':
    init_databases()
