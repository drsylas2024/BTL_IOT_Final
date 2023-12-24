import pyodbc
from flask import Flask, jsonify, request
import time
import requests
import random

cursor = None
cnxn = None
def connectToSQLServer() -> None:
    global cursor, cnxn
    server = '.'
    database = 'IOT'
    username = 'admin'
    password = '123456'
    driver = '{ODBC Driver 17 for SQL Server}' 

    cnxn = pyodbc.connect(f'DRIVER={driver};SERVER={server};DATABASE={database};UID={username};PWD={password};Trusted_Connection=yes;')
    cursor = cnxn.cursor()
    return cursor, cnxn
    

def sendToSQLServer(data : list, cursor, deviceName = "esp32", tableName = "SecurityLog") -> bool:
    #try:
        sqlCommand = f"INSERT INTO {tableName} (Sound, Humidity, Temperature, Light, Gas) VALUES ({data[0]}, {data[1]}, {data[2]}, {data[3]}, {data[4]})"
        cursor.execute(sqlCommand)
        cnxn.commit()
        return True
    #except:
        #return False


app = Flask(__name__)


@app.route('/get_data', methods=['GET'])
def get_data():
    data_param = request.args.get('data')
    device_name = request.args.get('device_name')
    # Data receive : Sound|Humidity|Temperature|Light|Gas
    data_receive = None
    print(f"[GET] /get_data - data: {data_param} - device_name: {device_name}")
    if data_param and device_name:
        try:
            data_receive = [float(x) for x in data_param.split('|')]
        except:
            return jsonify("invalid_data")
    else:
        return jsonify("no_data")
    # stt = sendToSQLServer(data_receive, cursor, deviceName=device_name)
    # if stt:
    #     print(f"[INSERT] Data {data_receive} - Device {device_name} SUCCESS")
    # else:
    #     print(f"[INSERT] Data {data_receive} - Device {device_name} FAILED")
    url = f"https://dweet.io/dweet/for/nhom1cntt1504?sound={data_receive[0]}&humidity={data_receive[1]}&temperature={data_receive[2]}&light={data_receive[3]}&gas={data_receive[4]}"
    req = requests.get(url)
    if req.status_code == 200:
        print(f"[DWEET] Data {data_receive} - Device {device_name} SUCCESS")
        return jsonify("ok")
    else:
        print(f"[DWEET] Data {data_receive} - Device {device_name} FAILED")
        return jsonify("error")

def getData(config : str, numberData : int = 30):
    data = config.split('|')
    rangeSound = [int(x) for x in data[0].split('-')]
    rangeHumidity = [int(x) for x in data[1].split('-')]
    rangeTemperature = [int(x) for x in data[2].split('-')]
    rangeLight = [int(x) for x in data[3].split('-')]
    rangeGas = [int(x) for x in data[4].split('-')]
    res = []
    for _ in range(numberData):
        sound = round(random.random() * (rangeSound[1] - rangeSound[0]) + rangeSound[0],1)
        humidity = round(random.random() * (rangeHumidity[1] - rangeHumidity[0]) + rangeHumidity[0],1)
        temperature = round(random.random() * (rangeTemperature[1] - rangeTemperature[0]) + rangeTemperature[0],1)
        light = int(random.random() * (rangeLight[1] - rangeLight[0]) + rangeLight[0])
        gas = round(random.random() * (rangeGas[1] - rangeGas[0]) + rangeGas[0],1)
        res.append([sound, humidity, temperature, light, gas])
    return res
if __name__ == '__main__':
    connectToSQLServer()
    #app.run(host='0.0.0.0',debug=True, port=5000)
    # Data receive : Sound|Humidity|Temperature|Light|Gas
    chayno_test = ["575-891|4-15|50-65|1-1|150-200","SecurityLog_ChayNo", 57 ]
    humidity_test = ["150-173|80-98|25-30|1-1|150-200", "SecurityLog_Humidity", 47 ]
    temp_test = ["150-173|4-15|50-65|1-1|150-200", "SecurityLog_Temp", 50]
    gas_test = ["160-190|40-67|25-30|1-1|650-890", "SecurityLog_Gas", 17]
    sound_test = ["304-672|40-67|25-30|1-1|165-200", "SecurityLog_Sound", 22 ]
    chayga_test = ["150-173|7-24|50-60|1-1|650-890", "SecurityLog_ChayGas", 29 ]
    binhthuong_test = ["150-173|40-67|25-30|1-1|175-200","SecurityLog", 23 ]
    print("Connected to SQL Server")
    cases = [chayno_test, humidity_test, temp_test, gas_test, sound_test, chayga_test, binhthuong_test]
    for case in cases:
        data = getData(case[0], case[2])
        for _ in data:
            res = sendToSQLServer(_, cursor,tableName=case[1])
            print(f"Data {_} - {res}")