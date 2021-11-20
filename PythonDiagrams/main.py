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
            titlefont_size=24,
            tickfont_size=20,
        ),
        xaxis=dict(
            title='Number of threads',
            titlefont_size=24,
            tickfont_size=20,
        ),
        font=dict(
            size=24,
        )
    )

    fig.show()
    # fig.write_html(t + ‘plot.html’, auto_open=True)


def plotBarGraph(belg: dict, you: dict, naca: dict, mycielskian: dict, db: dict):
    graph_names = ['Belgium OSM', 'com-Youtube', 'Mycielskian 13', 'dblp-2010', 'NACA 0015']

    # belg['serial'], you['serial'], mycielskian[['serial']], db['serial'], naca['serial']
    y_serial = [belg['serial'], you['serial'], mycielskian['serial'], db['serial'], naca['serial']]
    y_pthread = [belg['pthread'], you['pthread'], mycielskian['pthread'], db['pthread'], naca['pthread']]
    y_openmp = [belg['openmp'], you['openmp'], mycielskian['openmp'], db['openmp'], naca['openmp']]
    y_cilk = belg['opencilk'], you['opencilk'], mycielskian['opencilk'], db['opencilk'], naca['opencilk']

    fig = go.Figure(data=[
        go.Bar(name='Serial', x=graph_names, y=y_serial),
        go.Bar(name='Pthread', x=graph_names, y=y_pthread),
        go.Bar(name='OpenMP', x=graph_names, y=y_openmp),
        go.Bar(name='OpenCilk', x=graph_names, y=y_cilk)
    ])
    fig.update_layout(
        barmode='group',
        title='Best Performance',
        yaxis=dict(
            title='Execution time (s)',
            titlefont_size=24,
            tickfont_size=20,
        ),
        xaxis=dict(
            tickfont_size=20,
        ),
        font=dict(
            size=24,
        )
    )
    fig.show()


if __name__ == '__main__':
    graphs = {
        'Belgium OSM': "belgium_osm.txt",
        'com-YouTube': "com_youtube.txt",
        'Mycielskian 13': "mycielskian13.txt",
        'dblp 2010': "dblp_2010.txt",
        'NACA 0015': "naca_0015.txt"
    }

    belgium_min = {}
    youtube_min = {}
    naca_min = {}
    mycielskian_min = {}
    dblp_min = {}

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

        if graph.startswith('B'):
            belgium_min['serial'] = serial_time_min
        elif graph.startswith('c'):
            youtube_min['serial'] = serial_time_min
        elif graph.startswith('M'):
            mycielskian_min['serial'] = serial_time_min
        elif graph.startswith('d'):
            dblp_min['serial'] = serial_time_min
        elif graph.startswith('N'):
            naca_min['serial'] = serial_time_min

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

        if graph.startswith('B'):
            belgium_min['pthread'] = pthread_dict[str(min(pthread_dict, key=pthread_dict.get))]
        elif graph.startswith('c'):
            youtube_min['pthread'] = pthread_dict[str(min(pthread_dict, key=pthread_dict.get))]
        elif graph.startswith('M'):
            mycielskian_min['pthread'] = pthread_dict[str(min(pthread_dict, key=pthread_dict.get))]
        elif graph.startswith('d'):
            dblp_min['pthread'] = pthread_dict[str(min(pthread_dict, key=pthread_dict.get))]
        elif graph.startswith('N'):
            naca_min['pthread'] = pthread_dict[str(min(pthread_dict, key=pthread_dict.get))]

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

        if graph.startswith('B'):
            belgium_min['openmp'] = openMP_dict[str(min(openMP_dict, key=openMP_dict.get))]
        elif graph.startswith('c'):
            youtube_min['openmp'] = openMP_dict[str(min(openMP_dict, key=openMP_dict.get))]
        elif graph.startswith('M'):
            mycielskian_min['openmp'] = openMP_dict[str(min(openMP_dict, key=openMP_dict.get))]
        elif graph.startswith('d'):
            dblp_min['openmp'] = openMP_dict[str(min(openMP_dict, key=openMP_dict.get))]
        elif graph.startswith('N'):
            naca_min['openmp'] = openMP_dict[str(min(openMP_dict, key=openMP_dict.get))]

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

        if graph.startswith('B'):
            belgium_min['opencilk'] = cilk_dict[str(min(cilk_dict, key=cilk_dict.get))]
        elif graph.startswith('c'):
            youtube_min['opencilk'] = cilk_dict[str(min(cilk_dict, key=cilk_dict.get))]
        elif graph.startswith('M'):
            mycielskian_min['opencilk'] = cilk_dict[str(min(cilk_dict, key=cilk_dict.get))]
        elif graph.startswith('d'):
            dblp_min['opencilk'] = cilk_dict[str(min(cilk_dict, key=cilk_dict.get))]
        elif graph.startswith('N'):
            naca_min['opencilk'] = cilk_dict[str(min(cilk_dict, key=cilk_dict.get))]

            # print(cilk_dict)

        # title = graph + ': ' + 'Serial execution time ' + str(serial_time_min) + 's'
        # plotLineChart(pthread_dict, openMP_dict, cilk_dict, title)
        file.close()

    plotBarGraph(belgium_min, youtube_min, naca_min, mycielskian_min, dblp_min)
