from flask import Flask, render_template
from flask_socketio import SocketIO, emit
from flask_cors import CORS
import dweepy
import time
import requests
from naivebayes import loadModel, predict

app = Flask(__name__)
CORS(app)
socketio = SocketIO(app, cors_allowed_origins=['http://localhost:5000','http://127.0.0.1:5000','*'])
app.config['CORS_HEADERS'] = 'Content-Type'
last_time = ""
model = loadModel()
ipsub = "192.168.137.38"

def getDataFromDweet(dweetObj : dict) -> dict:
    sound = dweetObj['content']["sound"]
    temperature = dweetObj['content']["temperature"]
    humidity = dweetObj['content']["humidity"]
    light = dweetObj['content']["light"]
    gas = dweetObj['content']["gas"]
    longdate = dweetObj['created']
    return {
        "sound" : sound,
        "temperature" : temperature,
        "humidity" : humidity,
        "light" : light,
        "gas" : gas,
        "longdate" : longdate
    }

def send2Client():
    global last_time, socketio, stt
    while True:
        dweet = dweepy.get_latest_dweet_for('nhom1cntt1504')
        data = getDataFromDweet(dweet[0])
        current_time = time.strftime("%H:%M:%S", time.localtime())
        # Gửi dữ liệu tới client qua WebSocket với sự kiện 'data_update'
        if last_time != data["longdate"]:
            last_time = data["longdate"]
            socketio.emit('data_update', {"time" : current_time, "value" : data})
            pre = predict(model, data)
            print(f"Predict : {current_time}",pre)
            if pre != "Bình thường !":
                print("Predict : ",pre)
                # requests.post("https://pushmore.io/webhook/iqPPXvy5mEiZ8eLW4kGKZhyA", json = (pre + " - " + current_time))
                socketio.emit('status_update', {"time" : current_time, "value" : pre})
                # requests.get(f"http://{ipsub}/warn")
        # Đợi một khoảng thời gian trước khi lấy dữ liệu mới (ví dụ: 0.5 giây)
        time.sleep(0.5)

@app.route('/')
def index():
    return render_template('index.html')

@socketio.on('connect')
def handle_connect():
    print('Client connected')

@socketio.on('disconnect')
def handle_disconnect():
    print('Client disconnected')

if __name__ == '__main__':
    # Khởi chạy luồng lấy dữ liệu và gửi qua WebSocket
    socketio.start_background_task(send2Client)
    socketio.run(app, host="0.0.0.0")
