## Parallel and Distributed Systems Assignment 1

### Short Descritpion

The objective of this assignment is to calculate the number of triangles in a given graph. The graphs given are large graphs with unweighted edges. Their matixes are sparse and very large to the point they don't fit in memory. To overcome this problem we use the [CSR](https://www.geeksforgeeks.org/sparse-matrix-representations-set-3-csr/?ref=lbp) (Compressed Sparse Row) representation. The graphs are provided in matrix market format and their CSR vectors must be computed. After that we square the initial sparse matrix and from the computed matrix we keep only the elements in the places that are non zero in the initial matrix.
<br/>
The first part of the project is to calculate the number of triangles with serial programming and confirm that the proccess works. Afterward the workload must be computed using parallel programming for optimal performance using pthreads openMP and Cilk.

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

### Useful Links
- [Sparse Matrix manipulation](http://www.mathcs.emory.edu/~cheung/Courses/561/Syllabus/3-C/sparse.html)
