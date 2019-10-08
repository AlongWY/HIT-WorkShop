from typing import Any

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
import logging

hostname = '127.0.0.1'
port = '3306'
username = 'root'
pwd = 'mariadb'
database = 'shop'

dburl = 'mysql+mysqldb://{}:{}@{}:{}/{}'.format(username, pwd, hostname, port, database)


def query(object_, *multiparams: Any, echo=False, **params: Any):
    engine = create_engine(dburl, echo=echo)
    with engine.connect() as con:
        return con.execute(object_, *multiparams, **params)


def execute(clause: Any, params: Any = None, mapper: Any = None, bind: Any = None, echo=False, **kw: Any):
    engine = create_engine(dburl, echo=echo)
    Session = sessionmaker(bind=engine)
    session = Session()
    result = session.execute(clause, params, mapper, bind, **kw)
    session.commit()
    session.close()
    return result
