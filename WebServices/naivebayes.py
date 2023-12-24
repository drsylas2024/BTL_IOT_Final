from sklearn.naive_bayes import GaussianNB
from sklearn.model_selection import train_test_split
import pickle 
import pyodbc

cursor, cnxn = None, None
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

def loadData(cursor, table = "Trainning"):
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


def trainModel(data : list) -> GaussianNB:
    X = []
    y = []
    for row in data:
        X.append(row[:-1])
        y.append(row[-1])
    X_train, x_, y_train, y_ = train_test_split(X, y, test_size=0.25, random_state=42)
    gnb = GaussianNB()
    gnb.fit(X_train, y_train)
    return gnb

def predict(gnb : GaussianNB, data : dict):
    properties = ["temperature", "humidity", "gas", "light", "sound"]
    return (intToLabel(gnb.predict([[data[properties[0]], data[properties[1]], data[properties[2]], data[properties[3]], data[properties[4]]]])[0]))


def saveModel(gnb : GaussianNB):
    with open('./naive_bayes_model.pkl', 'wb') as file:
        pickle.dump(gnb, file)


def loadModel() -> GaussianNB:
    gnb = pickle.load(open('./model.pkl', 'rb'))
    return gnb

if __name__ == "__main__":
    #  Temperature	Humidity	Gas     	Light	Sound     Label
    #	26.2	    53	        300.9	    1	    171.3	    0
    cusors, _ = connectToSQLServer()
    rows = loadData(cusors)
    gnb = trainModel(rows)
    saveModel(gnb)
    print("Train model done !")
    cusors.close()
    _.close()