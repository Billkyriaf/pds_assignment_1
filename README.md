## Parallel and Distributed Systems Assignment 1

- [Parallel and Distributed Systems Assignment 1](#parallel-and-distributed-systems-assignment-1)
  - [Dependencies](#dependencies)
      - [1. ANSI C library for Matrix Market I/O](#1-ansi-c-library-for-matrix-market-io)
  - [Graphs source](#graphs-source)

### Dependencies
##### 1. ANSI C library for Matrix Market I/O

The [ANSI C library for Matrix Market I/O](https://math.nist.gov/MatrixMarket/mmio-c.html) library is used for reading matrix market files that contain the graphs. The project already contains the files requiered for compiling and linking the library [here](TriangleCalculator/libraries). Also a simple example [file](TriangleCalculator/read_matrix.c) can be found in the project used to demonstrate the read functionality of the library.


### Graphs source

|                                    Name                                     | Excecution time |
| :-------------------------------------------------------------------------: | :-------------: |
|    [DIMACS10: Belgiumosm](https://sparse.tamu.edu/DIMACS10/belgium_osm)     |      2.8s       |
|        [SNAP: com-Youtube](https://sparse.tamu.edu/SNAP/com-Youtube)        |      6.5s       |
| [Mycielski: mycielskian13](https://sparse.tamu.edu/Mycielski/mycielskian13) |      0.36s      |
|                [LAW](https://sparse.tamu.edu/LAW/dblp-2010)                 |      0.38s      |
|       [DIMACS10: NACA0015](https://sparse.tamu.edu/DIMACS10/NACA0015)       |      2.46s      |


Note: The excecution time for the above graphs is measured on a two core laptop.
