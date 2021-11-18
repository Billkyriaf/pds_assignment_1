import numpy as np
import plotly.graph_objects as go
from plotly.offline import iplot
import time


def plotLineChart(data_pthread: dict, data_openmp: dict, data_cilk: dict, t: str):
    n_threads = list(data_pthread.keys())
    y_pthreads = list(data_pthread.values())
    y_openmp = list(data_openmp.values())
    y_cilk = list(data_cilk.values())

    print("plotting " + t)

    # Create traces
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=n_threads, y=y_pthreads,
                             mode='lines+markers',
                             name='Pthread'))
    fig.add_trace(go.Scatter(x=n_threads, y=y_openmp,
                             mode='lines+markers',
                             name='OpenMP'))
    fig.add_trace(go.Scatter(x=n_threads, y=y_cilk,
                             mode='lines+markers',
                             name='OpenCilk'))

    fig.update_layout(
        title=t,
        yaxis=dict(
            title='Execution time (s)',
            titlefont_size=16,
            tickfont_size=14,
        ),
        xaxis=dict(
            title='Number of threads',
            titlefont_size=16,
            tickfont_size=14,
        ),
    )

    # fig.show()
    # fig.write_html(t + ‘plot.html’, auto_open=True)
    iplot(fig)


def plotBarGraph(data: dict):
    x_axis = list(data.keys())
    y_axis = list(data.values())
    trace = [go.Bar(x=x_axis, y=y_axis)]

    layout = go.Layout(barmode='group', bargroupgap=0)
    fig = go.Figure(data=trace, layout=layout)

    fig.update_layout(
        title='com-YouTube',
        yaxis=dict(
            title='Execution time (s)',
            titlefont_size=16,
            tickfont_size=14,
        ),
        xaxis=dict(
            title='Number of threads',
            titlefont_size=16,
            tickfont_size=14,
        ),
    )

    iplot(fig)


if __name__ == '__main__':
    graphs = {
        'Belgium OSM': "belgium_osm.txt",
        'com-YouTube': "com_youtube.txt",
        'Mycielskian 13': "mycielskian13.txt",
        'dblp 2010': "dblp_2010.txt",
        'NACA 0015': "naca_0015.txt"
    }

    for graph in graphs:
        file = open(graphs[graph], 'r')

        line = file.readline()

        serial_time_min = 0
        pthread_dict = {}
        openMP_dict = {}
        cilk_dict = {}

        if line.startswith("Serial"):
            time = file.readline()
            serial_times = []

            for s in time.split():
                try:
                    serial_times.append(float(s))
                except ValueError:
                    pass

            serial_time_min = np.amin(serial_times)

        line = file.readline()

        if line.startswith("Pthread_times"):
            time = file.readline()
            while time != "\n":
                print(time, graph)
                string_arr = time.split(":")
                key = string_arr[0].split()[1]
                avg_time = []

                for s in string_arr[1].split():
                    try:
                        avg_time.append(float(s))
                    except ValueError:
                        pass

                # avg_time.pop(0)
                # avg_time.pop(1)

                pthread_dict[key] = round(np.amin(avg_time), 4)
                time = file.readline()

            # print(pthread_dict)

        line = file.readline()

        if line.startswith("Openmp"):
            time = file.readline()
            while time != "\n":
                print(time, graph)
                string_arr = time.split(":")
                key = string_arr[0].split()[1]
                avg_time = []

                for s in string_arr[1].split():
                    try:
                        avg_time.append(float(s))
                    except ValueError:
                        pass

                openMP_dict[key] = round(np.amin(avg_time), 4)
                time = file.readline()

            # print(openMP_dict)

        line = file.readline()

        if line.startswith("OpenCilk"):
            time = file.readline()
            while time != "\n":
                print(time, graph)
                string_arr = time.split(":")
                key = string_arr[0].split()[1]
                avg_time = []

                for s in string_arr[1].split():
                    try:
                        avg_time.append(float(s))
                    except ValueError:
                        pass

                cilk_dict[key] = round(np.max(avg_time), 4)
                time = file.readline()

            # print(cilk_dict)

        title = graph + ': ' + 'Serial execution time ' + str(serial_time_min) + 's'
        plotLineChart(pthread_dict, openMP_dict, cilk_dict, title)
        file.close()
