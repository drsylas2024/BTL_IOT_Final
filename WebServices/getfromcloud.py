import dweepy
from threading import Event
import time
import pyodbc
from threading import Thread, Event

idx = 0

def getLatestDweet_Thread(thingName, event : Event):
    global idx
    conn = pyodbc.connect('Driver={ODBC Driver 17 for SQL Server};'
        'Server=.;'
        'Database=IOT;'
        'UID=admin;'
        'PWD=123456;'
        'Trusted_Connection=no;'
        )
    cursor = conn.cursor()
    last_time = ""
    while not event.isSet():
        try:
            urls = dweepy.get_latest_dweet_for(thingName)
            for url in urls:
                dict = url
                sound = dict['content']["sound"]
                temperature = dict['content']["temperature"]
                humidity = dict['content']["humidity"]
                light = dict['content']["light"]
                gas = dict['content']["gas"]
                longdate = dict['created']
                if last_time != longdate:
                    cursor.execute(
                    f'''
                    INSERT INTO SecurityLog
                        ([Humidity]
                        ,[Light]
                        ,[Gas]
                        ,[Temperature]
                        ,[Sound])
                    VALUES
                        ({humidity}
                        ,{light}
                        ,{gas}
                        ,{temperature}
                        ,{sound})
                    '''

                    )
                    conn.commit()
                    last_time = longdate
                    print("Insert success")
        except:
            print("Missing a row!")
    time.sleep(2)
def stopGetDataDweet_Thread(event : Event):
    event.set()

def main():
    event = Event()
    getdata_thread = Thread(target=getLatestDweet_Thread,
    args=("nhom1cntt1504", event))
    getdata_thread.start()
    stop = input("Kết thúc (y/n)? ")
    if stop=="y":
        print("Main thread finished.")
        event.set()

if __name__ == '__main__' :
    main()