from flask import Blueprint, jsonify, request
from tests import mysql

bp = Blueprint('marked', __name__)

@bp.route('/', methods=['GET'])
def get_marked():

    try:
        cur = mysql.connection.cursor()
        cur.execute("SELECT marked FROM customers WHERE marked IS NOT NULL ORDER BY id DESC LIMIT 4")
        data = cur.fetchall()
        # recent_locations = [row[0] for row in data]
        # cur.close()
        #
        # column_names = [desc[0] for desc in cur .description]
        # result = [dict(zip(column_names, row)) for row in data ]

        return jsonify(data), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

    # return jsonify(data)

@bp.route('/', methods=['POST'])
def add_marked():
    data = request.json
    recent_location = data.get('location')
    try:
        cur = mysql.connection.cursor()
        cur.execute("INSERT INTO customers (marked) VALUES (%s)", (recent_location,))
        mysql.connection.commit()
        cur.close()
        return jsonify({"message": "Marked added successfully!"}), 201
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@bp.route('/<location>', methods=['DELETE'])
def delete_marked(location):
    try:
        cur = mysql.connection.cursor()
        cur.execute("DELETE FROM customers WHERE marked = %s", (location,))
        mysql.connection.commit()
        cur.close()
        return jsonify({"message": "Marked location deleted successfully!"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500