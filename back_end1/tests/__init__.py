from flask import Flask
from flask_cors import CORS
from flask_mysqldb import MySQL

mysql = MySQL()

def create_app():
    app = Flask(__name__)

    CORS(app)

    # Load the configuration from config.py
    app.config.from_object('config.Config')

    # Initialize MySQL
    mysql.init_app(app)

    # # Set up CORS
    # CORS(app, resources={r"/data": {"origins": "http://localhost:5173"}})

    # Register routes
    from tests import recents, marked, traffic_speed, traffic_speed2, traffic_speed3, traffic_speed4

    app.register_blueprint(recents.bp, url_prefix='/recents')
    app.register_blueprint(marked.bp, url_prefix='/marked')
    app.register_blueprint(traffic_speed.bp, url_prefix='/traffic_speed')
    app.register_blueprint(traffic_speed2.bp, url_prefix='/traffic_speed2')
    app.register_blueprint(traffic_speed3.bp, url_prefix='/traffic_speed3')
    app.register_blueprint(traffic_speed4.bp, url_prefix='/traffic_speed4')

    return app