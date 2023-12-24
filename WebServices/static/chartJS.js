// Lưu ý: Cần import ChartJS trước khi import file này
let makeChart = (ctx, {label, color = 'rgb(75, 192, 192)', borderWidth = 1.8}) => new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: label,
            data: [], // Dữ liệu của biểu đồ
            borderColor: color, // Màu viền
            borderWidth: borderWidth, // Độ rộng viền
            fill: false // Không tô màu dưới đường
        }]
    },
    options: {
        responsive: true, // Biểu đồ có phản ứng với kích thước
        maintainAspectRatio: false, // Giữ tỷ lệ khung hình
    }
});

export default makeChart;