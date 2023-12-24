import pyodbc
import requests
import time

conn = pyodbc.connect('Driver={ODBC Driver 17 for SQL Server};'
        'Server=.;'
        'Database=IOT;'
        'UID=admin;'
        'PWD=123456;'
        'Trusted_Connection=no;'
        )
def lstToObj(lst : list) -> dict:
    return {
        "sound" : lst[7],
        "humidity" : lst[4],
        "temperature" : lst[3],
        "light" : lst[6],
        "gas" : lst[5]
    }
cursor = conn.cursor()
cursor.execute(f"SELECT * FROM Training")
rows = cursor.fetchall()
for _ in range(0, 100):
    for row in rows:
        data = lstToObj(list(row))
        print(data)
        requests.post('https://dweet.io:443/dweet/quietly/for/nhom1cntt1504', json=data)
        print("Sent !")
        time.sleep(1)