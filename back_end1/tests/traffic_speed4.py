from flask import Blueprint, jsonify, request
from tests import mysql

bp = Blueprint('traffic_speed4', __name__)


@bp.route('/', methods=['GET'])
def get_traffic_speed4():
    try:
        cur = mysql.connection.cursor()
        cur.execute("SELECT traffic_speed4 FROM customers WHERE traffic_speed4 IS NOT NULL ORDER BY id DESC LIMIT 1")
        data = cur.fetchone()  # Fetch one record from the query

        if data is None:
            return jsonify({"message": "No data found"}), 404  # Return 404 if no data is found

        # Extract the value from the tuple and convert it to a float
        traffic_speed = float(data[0])

        cur.close()

        # Return the float value in a JSON object
        return jsonify({"traffic_speed4": traffic_speed}), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500  # Return error message as JSON


@bp.route('/', methods=['POST'])
def add_traffic_speed4():
    data = request.json
    current_traffic_speed = data.get('traffic_speed4')
    try:
        cur = mysql.connection.cursor()
        cur.execute("INSERT INTO customers (traffic_speed4) VALUES (%s)", (current_traffic_speed,))
        mysql.connection.commit()
        cur.close()
        return jsonify({"message": "Recent added successfully!"}), 201
    except Exception as e:
        return jsonify({"error": str(e)}), 500
