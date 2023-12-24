from main import connectToSQLServer
import random

cursor, cnxn = connectToSQLServer()

def getRows(cursor, tableName = "SecurityLog"):
    cursor.execute(f"SELECT * FROM {tableName}")
    rows = cursor.fetchall()
    return rows

def sortParam(arr : list):
    #(Sound, Humidity, Temperature, Light, Gas) -> expected
    #(Humidity, Light, Gas, Temp, Sound) -> real
    _ = []
    for data in arr:
        _.append([data[-1], data[0], data[3], data[1], data[2]])
    return _

def markLabel(arr : list, label : int):
    for _ in arr:
        _.append(label)
    return arr

def sendToSQLServer(data : list, cursor, deviceName = "esp32", tableName = "trainSet") -> bool:
    sqlCommand = f"INSERT INTO {tableName} (Device_Name, Noise, Humidity, Temperature, Light, Gas, Label) VALUES ('{deviceName}',{data[0]}, {data[1]}, {data[2]}, {1 if data[3] == True else 0}, {data[4]}, {data[5]})"
    print(sqlCommand)
    cursor.execute(sqlCommand)
    cnxn.commit()
    print(f"[INSERT] Data {data} - Device {deviceName} SUCCESS")
    return True

def getData(cursor):
    tables = {
        'SecurityLog' : 0, 
        'SecurityLog_ChayGas' : 1, 
        'SecurityLog_ChayNo' : 2, 
        'SecurityLog_Gas' : 3, 
        'SecurityLog_Humidity' : 4,
        'SecurityLog_Sound' : 5, 
        'SecurityLog_Temp' : 6
        }
    data = []
    for table in tables:
        data += markLabel(sortParam([list(x) for x in getRows(cursor,table)]), tables[table])
    random.shuffle(data)
    return data

if __name__ == "__main__":
    data = getData(cursor)
    for row in data:
        sendToSQLServer(row, cursor, tableName="Training")
    cursor.close()