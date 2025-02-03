from peewee import MySQLDatabase, Model

db = MySQLDatabase(
    'checkoff_py',
    user='root',
    password='strong_password',
    host='checkoff-mysql',
    port=3306
)


class BaseModel(Model):
    class Meta:
        database = db
