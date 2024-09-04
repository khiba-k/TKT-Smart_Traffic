# import mysql.connector
# import time
# from datetime import datetime

# conn = mysql.connector.connect(
#     host="localhost",
#     user="newuser",
#     password="newpassword",
#     database="recent_db"
# )

# cursor = conn.cursor()

# current_time = datetime.now()

# sql = "INSERT INTO customers (traffic_speed) VALUES (%s)"
# val = ("12 m/s",)
# cursor.execute(sql, val)

# # required to make changes
# conn.commit()



# conn.close()

import mysql.connector

# Establishing the connection to the database
mydb = mysql.connector.connect(
  host="localhost",
  user="newuser",
  password="newpassword",
  database="recent_db"
)

# Creating a cursor object
mycursor = mydb.cursor()

# Selecting all records from the 'customers' table
mycursor.execute("SELECT * FROM customers")

# Fetching all the records
myresult = mycursor.fetchall()

# Closing the cursor and the database connection
mycursor.close()
mydb.close()

# Printing the records
for x in myresult:
  print(x)


# import mysql.connector
#
# mydb = mysql.connector.connect(
#   host="localhost",
#   user="newuser",
#   password="newpassword",
#   database="recent_db"
# )
#
# mycursor = mydb.cursor()
#
# mycursor.execute("ALTER TABLE customers DROP COLUMN `traffic_speed`")


# import mysql.connector
#
# # Database connection details
# db_name = "recent_db"
# username = "newuser"
# password = "newpassword"
# host = "localhost"  # Change to your host if needed
#
# # Table and column details
# table_name = "customers"
# column_name = "traffic_speed"
# data_type = "FLOAT"  # Adjust data type based on your needs (e.g., INT)
#
# try:
#     # Connect to the database
#     connection = mysql.connector.connect(
#         database=db_name, user=username, password=password, host=host
#     )
#
#     # Create a cursor object
#     cursor = connection.cursor()
#
#     # Construct the CREATE TABLE query (if table doesn't exist)
#     create_table_query = f"""
#         CREATE TABLE IF NOT EXISTS {table_name} (
#             id INT AUTO_INCREMENT PRIMARY KEY,
#             name VARCHAR(255) NOT NULL
#             /* Add other existing columns here */
#         )
#     """
#
#     # Execute the CREATE TABLE query (if needed)
#     cursor.execute(create_table_query)
#
#     # Check if the column already exists
#     cursor.execute(f"SHOW COLUMNS FROM {table_name} LIKE '{column_name}'")
#     result = cursor.fetchone()
#
#     # If the column does not exist, add it
#     if not result:
#         alter_table_query = f"""
#             ALTER TABLE {table_name}
#             ADD COLUMN {column_name} {data_type}
#         """
#         cursor.execute(alter_table_query)
#         print(f"Column '{column_name}' created in table '{table_name}' successfully!")
#     else:
#         print(f"Column '{column_name}' already exists in table '{table_name}'.")
#
#     # Commit the changes
#     connection.commit()
#
# except mysql.connector.Error as err:
#     print(f"Error creating column: {err}")
#
# finally:
#     if connection.is_connected():
#         cursor.close()
#         connection.close()

##################################################################################
# ADDING THREE COLUMNS
##################################################################################

# import mysql.connector
#
# # Database connection details
# db_name = "recent_db"
# username = "newuser"
# password = "newpassword"
# host = "localhost"  # Change to your host if needed
#
# # Table and column details
# table_name = "customers"
# columns_to_add = [
#     {"name": "traffic_speed2", "data_type": "FLOAT"},
#     {"name": "traffic_speed3", "data_type": "FLOAT"},
#     {"name": "traffic_speed4", "data_type": "FLOAT"},
# ]
#
# try:
#     # Connect to the database
#     connection = mysql.connector.connect(
#         database=db_name, user=username, password=password, host=host
#     )
#
#     # Create a cursor object
#     cursor = connection.cursor()
#
#     # Construct the CREATE TABLE query (if table doesn't exist)
#     create_table_query = f"""
#         CREATE TABLE IF NOT EXISTS {table_name} (
#             id INT AUTO_INCREMENT PRIMARY KEY,
#             name VARCHAR(255) NOT NULL
#             /* Add other existing columns here */
#         )
#     """
#
#     # Execute the CREATE TABLE query (if needed)
#     cursor.execute(create_table_query)
#
#     # Function to check and add column if it doesn't exist
#     def add_column_if_not_exists(column_name, data_type):
#         cursor.execute(f"SHOW COLUMNS FROM {table_name} LIKE '{column_name}'")
#         result = cursor.fetchone()
#         if not result:
#             alter_table_query = f"""
#                 ALTER TABLE {table_name}
#                 ADD COLUMN {column_name} {data_type}
#             """
#             cursor.execute(alter_table_query)
#             print(f"Column '{column_name}' created in table '{table_name}' successfully!")
#         else:
#             print(f"Column '{column_name}' already exists in table '{table_name}'.")
#
#     # Iterate over the columns and add them if they don't exist
#     for column in columns_to_add:
#         add_column_if_not_exists(column["name"], column["data_type"])
#
#     # Commit the changes
#     connection.commit()
#
# except mysql.connector.Error as err:
#     print(f"Error creating column: {err}")
#
# finally:
#     if connection.is_connected():
#         cursor.close()
#         connection.close()
