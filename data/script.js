// script.js

function createChart(containerId, titleText, yAxisTitle, seriesColor) {
  return new Highcharts.Chart({
      chart: { renderTo: containerId },
      title: { text: titleText },
      series: [{
          showInLegend: false,
          data: []
      }],
      plotOptions: {
          line: {
              animation: false,
              dataLabels: { enabled: true }
          },
          series: { color: seriesColor }
      },
      xAxis: {
          type: 'datetime',
          dateTimeLabelFormats: { second: '%H:%M:%S' },
          gridLineWidth: 1
      },
      yAxis: {
          title: { text: yAxisTitle },
          gridLineWidth: 1
      },
      credits: { enabled: false }
  });
}

function updateChart(chart, url) {
  setInterval(function () {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
              var x = (new Date()).getTime(),
                  y = parseFloat(this.responseText);
              if (chart.series[0].data.length > 40) {
                  chart.series[0].addPoint([x, y], true, true, true);
              } else {
                  chart.series[0].addPoint([x, y], true, false, true);
              }
          }
      };
      xhttp.open("GET", url, true);
      xhttp.send();
  }, 5000);
}

var chartT = createChart('chart-temperature', 'Water Temperature', 'Temperature (Celsius)', '#059e8a');
updateChart(chartT, '/temperature');
console.log(updateChart(chartT, '/temperature'));

var chartH = createChart('chart-turbidity', 'Turbidity', 'NTU', '');
updateChart(chartH, '/turbidity');
console.log(updateChart(chartH, '/turbidity'));

var chartP = createChart('chart-tds', 'TDS Level', 'ppm', '#18009c');
updateChart(chartP, '/tds');
console.log(updateChart(chartP, '/tds'));
