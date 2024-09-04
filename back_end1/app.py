from flask import Flask, jsonify, request
from flask_mysqldb import MySQL
from flask_cors import CORS

app = Flask(__name__)
CORS(app, resources={r"/data": {"origins": "http://localhost:5173"}})

# Configure MySQL
app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'newuser'
app.config['MYSQL_PASSWORD'] = 'newpassword'
app.config['MYSQL_DB'] = 'recent_db'

# Initialize MySQL
mysql = MySQL(app)


@app.route('/')
def index():
    return 'Welcome to my Flask app!'

@app.route('/data', methods=['GET'])
def get_data():
    cur = mysql.connection.cursor()
    cur.execute('''SELECT location FROM customers''')
    results = cur.fetchall()
    print(results)
    return jsonify(results)


@app.route('/data', methods=['POST'])
def add_data():
    data = request.json
    name = data['location']  # Assuming 'name' is a field in yourtable
    cur = mysql.connection.cursor()
    cur.execute('''INSERT INTO yourtable (name) VALUES (%s)''', (name,))
    mysql.connection.commit()
    cur.close()
    return jsonify({'message': 'Record added successfully'}), 201


@app.route('/data/<int:id>', methods=['PUT'])
def update_data(id):
    data = request.json
    name = data['location']  # Assuming 'name' is a field in yourtable
    cur = mysql.connection.cursor()
    cur.execute('''UPDATE yourtable SET name = %s WHERE id = %s''', (name, id))
    mysql.connection.commit()
    cur.close()
    return jsonify({'message': 'Record updated successfully'})


@app.route('/data/<int:id>', methods=['DELETE'])
def delete_data(id):
    cur = mysql.connection.cursor()
    cur.execute('''DELETE FROM yourtable WHERE id = %s''', (id,))
    mysql.connection.commit()
    cur.close()
    return jsonify({'message': 'Record deleted successfully'})


if __name__ == '__main__':
    app.run(debug=True)
