import numpy as np
import plotly.graph_objects as go
import plotly.io as pio
from plotly.offline import iplot


def plotList(data: list):
    fig = go.FigureWidget()

    x_axis = np.arange(1, len(data) + 1)
    fig.add_trace(go.Scatter(x=x_axis, y=data, mode='lines', name='evaluation', text=data,
                             line=dict(color='#0000ff', width=2)))

    # fig.show()

    pio.kaleido.scope.default_format = "svg"
    pio.kaleido.scope.default_width = 1920
    pio.kaleido.scope.default_height = 1080
    pio.kaleido.scope.default_scale = 1

    # fig.write_image("fig.svg")
    fig.show()


def plotBarGraph(data: dict):
    x_axis = list(data.keys())
    y_axis = list(data.values())
    trace = [go.Bar(x=x_axis, y=y_axis)]

    layout = go.Layout(barmode='group', bargroupgap=0)
    fig = go.Figure(data=trace, layout=layout)

    iplot(fig)


if __name__ == '__main__':
    belgium = "D:\\University\\AUTH\\Electrical_engineears\\7nth_semester\\Parallel_and_Distributed_Systems\\" \
              "Assignment_1\\pds_assignment_1\\Graphs\\belgium_osm\\belgium_osm.txt"

    youtube = "D:\\University\\AUTH\\Electrical_engineears\\7nth_semester\\Parallel_and_Distributed_Systems\\" \
              "Assignment_1\\pds_assignment_1\\Graphs\\com_youtube\\com_youtube.txt"

    file = open(belgium, "r")

    line = file.readline()

    times = []
    parallel_dict = {}
    openMP_dict = {}

    # if line.startswith("Serial"):
    #     time = file.readline()
    #
    #     for s in time.split():
    #         try:
    #             times.append(float(s))
    #         except ValueError:
    #             pass
    #
    #     print(np.mean(times))
    #
    # line = file.readline()

    if line.startswith("Parallel_times"):
        time = file.readline()
        while time != "\n":
            string_arr = time.split(":")
            key = string_arr[0].split()[1]
            avg_time = []

            for s in string_arr[1].split():
                try:
                    avg_time.append(float(s))
                except ValueError:
                    pass

            avg_time.pop(0)
            avg_time.pop(1)

            parallel_dict[key] = round(np.mean(avg_time), 5)
            time = file.readline()

        print(parallel_dict)

    line = file.readline()

    if line.startswith("Openmp"):
        time = file.readline()
        while time != "\n":
            string_arr = time.split(":")
            key = string_arr[0].split()[1]
            avg_time = []

            for s in string_arr[1].split():
                try:
                    avg_time.append(float(s))
                except ValueError:
                    pass

            openMP_dict[key] = round(np.mean(avg_time), 5)
            time = file.readline()

        print(openMP_dict)

    plotBarGraph(parallel_dict)
    #plotBarGraph(openMP_dict)
