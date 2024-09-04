# app.py
from flask import Flask
from user_routes import user_bp
from sensor_routes import sensor_bp
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

# Register blueprints
app.register_blueprint(user_bp, url_prefix='/users')
app.register_blueprint(sensor_bp, url_prefix='/sensors')

if __name__ == '__main__':
    app.run(debug=True)
