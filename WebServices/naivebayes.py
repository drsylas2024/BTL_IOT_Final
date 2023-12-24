from sklearn.naive_bayes import GaussianNB
from sklearn.model_selection import train_test_split
from main import connectToSQLServer
import pickle 
import dweepy
import time 
from threading import Thread, Event
import requests

def loadData(cursor, table = "Training"):
    cursor.execute(f"SELECT * FROM {table}")
    rows = cursor.fetchall()
    return rows

def intToLabel(x : int):
    a = {
        0 : "Bình thường", #ok
        1 : "Cháy Gas", #ok
        2 : "Cháy nổ", #ok
        3 : "Có rò rỉ khí gas", #ok
        4 : "Độ ẩm cao", #ok
        5 : "Có âm thanh lớn", #ok
        6 : "Nhiệt độ quá cao" #ok
    }
    return a[x] + " !"

def labelToAlert(x : str):
    a = {
        "Cháy Gas !" : "Gas_Fire_Warning",
        "Cháy nổ !" : "Loud_Fire_Warning",
        "Có rò rỉ khí gas !" : "Gas_Leak_Warning",
        "Độ ẩm cao !" : "Humidity_Warning",
        "Có âm thanh lớn !" : "Loud_Warning",
        "Nhiệt độ quá cao !" : "Temperature_Warning"
    }
    return a[x]

def trainModel(data : list) -> GaussianNB:
    X = []
    y = []
    for row in data:
        X.append(row[3:-1])
        y.append(row[-1])
    X_train, x_, y_train, y_ = train_test_split(X, y, test_size=0.25, random_state=42)
    gnb = GaussianNB()
    gnb.fit(X_train, y_train)
    return gnb

def predict(gnb : GaussianNB, data : dict):
    properties = ["temperature", "humidity", "gas", "light", "sound"]
    return (intToLabel(gnb.predict([[data[properties[0]], data[properties[1]], data[properties[2]], data[properties[3]], data[properties[4]]]])[0]))


def saveModel(data):
    gnb = trainModel(data)
    with open('./naive_bayes_model.pkl', 'wb') as file:
        pickle.dump(gnb, file)

def formatTime(time : str):
    date = time.split('T')[0]
    time_format = time.split('T')[1]
    time_format = time_format.split(':')
    time_format[0] = str(int(time_format[0]) + 7)
    time_format = ':'.join(time_format)
    return f"{date} - {time_format}"

def loadModel() -> GaussianNB:
    gnb = pickle.load(open('./model.pkl', 'rb'))
    return gnb

if __name__ == "__main__":
    #  Temperature	Humidity	Gas     	Light	Sound     Label
    #	26.2	    53	        184.9	    1	    171.3	    0
    cusors, _ = connectToSQLServer()
    cusors.close()
    _.close()