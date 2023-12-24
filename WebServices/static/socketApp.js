let makeToast = (title, delay = 3000) => {
    Toastify({
        text: title,
        duration: delay, // Thời gian hiển thị thông báo (3 giây)
        gravity : "bottom",
        offset: {
          x: 50, // horizontal axis - can be a number or a string indicating unity. eg: '2em'
          y: 10 // vertical axis - can be a number or a string indicating unity. eg: '2em'
        },
        style: {
          background: "linear-gradient(to right, #00b09b, #96c93d)",
          height : "60px",
          width : "300px",
          "text-align": "center",
          "font-size": "20px",
          "background": "red",
          border: "3px solid #000",
          "border-radius": "10px"
        }
    }).showToast();
}

// Khởi tạo socket
const socket = io()
const client = {
    charts : [],
    connect : () => {
        socket.connect({transports: ['websocket', 'polling', 'flashsocket']});
    },
    pushToChart : (data) =>{
        //Temp - Sound - Humidity - Gas
        let config = [
            {
                index : 0,
                value : data.value.temperature
            },
            {
                index : 1,
                value : data.value.sound
            },
            {
                index : 2,
                value : data.value.humidity
            },
            {
                index : 3,
                value : data.value.gas
            }
        ]
        config.forEach((item) =>{
            client.charts[item.index].data.labels.push(data.time);
            client.charts[item.index].data.datasets[0].data.push(item.value);
            client.charts[item.index].update();
        })
    },
    handleEvent : () =>{
        socket.on('connect', () => {
            console.log('Connected to server');
        });
        socket.on('disconnect', () => {
            console.log('Disconnected from server');
        });
        socket.on('data_update', (data) => {    
            client.pushToChart(data);
        });
        socket.on('status_update', (data) => {
            makeToast(data.value, 3000)
        })
    },
    run : () =>{
        client.connect()
        client.handleEvent()
    }
}

export default client;