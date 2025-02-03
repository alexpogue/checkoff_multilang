# gunicorn.conf.py
from checkoff_python.init_single_threaded import init_databases


def when_ready(server):
    """Called just after the server is started."""
    init_databases()
