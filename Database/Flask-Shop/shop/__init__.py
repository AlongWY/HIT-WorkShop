from flask import Flask
from config import Config
from flask_login import LoginManager

app = Flask(__name__)
app.config.from_object(Config)
login = LoginManager(app)
login.login_view = 'login'

from shop import routes

from shop.models import Cart


@app.context_processor
def cart_info():
    return {'cart': Cart()}
