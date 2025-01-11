# gunicorn.conf.py
from checkoff_python.init_single_threaded import init_databases

print(f"[gunicorn.conf.py] os.getenv('SQLALCHEMY_DATABASE_URI') = {os.getenv('SQLALCHEMY_DATABASE_URI')}")

def when_ready(server):
    """Called just after the server is started."""
    init_databases()
