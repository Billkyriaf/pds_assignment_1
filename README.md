<div id="top"></div>

<br />
<div align="center">
  <h1 align="center">Parallel and Distributed Systems Assignment 1</h1>
  <h3 align="center">Aristotle University of Thessaloniki</h3>
  <h4 align="center">School of Electrical & Computer Engineering</h4>
  <p align="center">
    Contributors: Kyriafinis Vasilis, Koro Erika
    <br />
    Winter Semester 2021 - 2022
    <br />
    <br />
    <br />
    <br />
  </p>
</div>

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-this-project">About this project</a></li>
    <li><a href="#getting-started">Getting started</a></li>
    <li><a href="#dependencies">Dependencies</a></li>
    <li><a href="#project-directory-layout">Project directory layout</a></li>
    <li><a href="#compile-and-run">Complile and run</a></li>
    <li><a href="#graphs">Graphs</a></li>
    <li><a href="#useful-links">Useful links</a></li>
  </ol>
</details>

## About this project

<p align="justify">
  The objective of this assignment is to calculate the number of triangles in a given graph. The graphs given are large graphs with unweighted edges. Their matixes are sparse and very large to the point they don't fit in memory. To overcome this problem we use the <a href='https://www.geeksforgeeks.org/sparse-matrix-representations-set-3-csr/?ref=lbp'>CSR</a> (Compressed Sparse Row) representation. The graphs are provided in matrix market format and their CSR vectors must be computed. After that we square the initial sparse matrix and from the computed matrix we keep only the elements in the places that are non zero in the initial matrix.
<br/>
<br/>
  The first part of the project is to calculate the number of triangles with serial programming and confirm that the proccess works. Afterwards the workload must be computed  using parallel programming for optimal performance using pthreads openMP and Cilk.
<br/>
<br/>
</p>

## Getting started

To setup this repository on your local machine run the following commands on the terminal:

```console
git clone https://github.com/Billkyriaf/pds_assignment_1.git
```

Or alternatively [*download*](https://github.com/Billkyriaf/pds_assignment_1/archive/refs/heads/main.zip) and extract the zip file of the repository
<br/>
<br/>

## Dependencies
#### 1. Make or Cmake

This project uses make utilities to build and run the excecutables. Alternatively you can use Cmake but openCilk is not supported in the build script.

#### 2. OpenMP

Make sure you have an OpenMP capable compiler

#### 3. OpenCilk-9.0.1

You can install OpenCilk for linux operating systems by running the following commands (some commands may require `sudo`):

1. Download the compressed file from GitHub
    ```console
    wget https://github.com/OpenCilk/opencilk-project/releases/download/opencilk%2Fbeta3/OpenCilk-9.0.1-Linux.tar.gz
    tar xvzf OpenCilk-9.0.1-Linux.tar.gz
    mv OpenCilk-9.0.1-Linux/ /usr/local/
    ```
    > t is recommended to use the directory specified above otherwise you will have to edit the [*Makefile*](TriangleCalculator/Makefile).
2. Update the permissions  
    ```console
    chmod og+xr /usr/local/OpenCilk-9.0.1-Linux/ -R
    ```
3. (Optional) Remove the compressed file
    ```console
    rm -f OpenCilk-9.0.1-Linux.tar.gz
    ```

#### 4. ANSI C library for Matrix Market I/O
The [*ANSI C library for Matrix Market I/O*](https://math.nist.gov/MatrixMarket/mmio-c.html) library is used for reading matrix market files that contain the graphs. The project already contains the files requiered for compiling and linking the files in the [*ext_libraries*](TriangleCalculator/src/ext_libraries) directory. The Matrix Market standard can be found [*here*](https://networkrepository.com/mtx-matrix-market-format.html). In general _**coordinate pattern symmetric**_ matrices are used. If the graphs represented in those matrices are _**Unweighted**_ the weight for each edge is concidered as 1.

<br/>

## Project directory layout

### Top-level
```
.
├─ Graphs                # Graph Data are stored here (.mtx .txt files)
├─ PythonDiagrams        # Pycharm Project to create plots
├─ TriangleCalculator    # Main Project
└─ README.md
```
### TriangleCalculator
```
.
├── ...
├── TrangleCalculator
|   ├── .idea                # Idea folder for Clion project
|   ├── Julia                # Julia Code for the serial implementation of the algorithm
|   ├── src
|   |   ├── OpenCilk         # OpenCilk related source files
|   |   ├── OpenMP           # OpenMP related source files
|   |   ├── Pthreads         # Pthread related source files
|   |   ├── Serial           # Serial algorithm source files
|   |   ├── ext_libraries    # Extrernal dependencies source files
|   |   ├── libraries        # Utility source files
|   |   ├── opencilk_main.c 
|   |   ├── openmp_main.c
|   |   ├── pthread_main.c
|   |   └── serial_main.c
|   ├── CMakeLists.txt       # Cmake build script for Clion
|   └── Makefile             # Makefile for linux base distros 
|
└─ ...
```
<br/>
<br/>

## Compile and run

### Linux
Simply run `make all` in the [*TriangleCalculator*](TriangleCalculator) directory. The executable files will be created in the `linux_build` directory along with the `object` files. To run an executable `cd` in the `linux_build` directory and run the command `./executable_name arg1 arg2`. Run `make clean` to clear all the object and executable files. For more information on the command line arguments read bellow

### Windows (OpenCilk not supported)
The code was developed on windows and specificaly in CLion. The easiest way to run the source code is to import the project to a Clion project. The [*CMakeLists.txt*](TriangleCalculator/CMakeLists.txt) provides targets for executing every module of the program. The only extra step required is to create the configurations and add the command line arguments.

<br/>

### Command line argumnets (Both Linux and Windows)
* Serial executable
    ```C
    argv[1] = 'Path to the .mtx file of the praph'
    ```
* Pthread executable
    ```C
    argv[1] = 'Path to the .mtx file of the praph'
    argv[2] = number of threads to spawn and divide the work (be reasonable max number of threads is not specified)
    ```
* Serial executable
    ```C
    argv[1] = 'Path to the .mtx file of the praph'
    argv[2] = number of threads to request from the OpenMP API
    ```
* Serial executable
    ```C
    argv[1] = 'Path to the .mtx file of the praph'
    argv[2] = number of threads to request from the OpenCilk API
    ```
    
Note: Relative file paths are not supported on Windows.



## Graphs

The input of the program is a `graph.mtx` file. This is a `matrix market` file that contains the relations between the nodes oth the graphs. IMPORTANT note: All the graphs must be _**unweighted**_ and _**undirected**_. This is crusial because only then the Sparse matrix of the graph is symmetrical. The code provided in the [*libraries*](TriangleCalculator/src/libraries) directory takes this assumption for granted and does not check if it is true. During the development phase the graphs bellow were used to test the code, along with some small graphs created for debugging purposes. All the files can be found in the [*Graphs*](Graphs) directory.

<br/>
The proceeding table showcases the graphs and creates a reference performance. (The times displayed were given with the assignment)
<br/>
<br/>

|                                    Name                                     | Excecution time |
| :-------------------------------------------------------------------------: | :-------------: |
|    [DIMACS10: Belgiumosm](https://sparse.tamu.edu/DIMACS10/belgium_osm)     |      2.8s       |
|        [SNAP: com-Youtube](https://sparse.tamu.edu/SNAP/com-Youtube)        |      6.5s       |
| [Mycielski: mycielskian13](https://sparse.tamu.edu/Mycielski/mycielskian13) |      0.36s      |
|                [LAW](https://sparse.tamu.edu/LAW/dblp-2010)                 |      0.38s      |
|       [DIMACS10: NACA0015](https://sparse.tamu.edu/DIMACS10/NACA0015)       |      2.46s      |

Note: The excecution time for the above graphs is measured on a two core laptop.

Along with the `graph.mtx` for most of the graphs `graph.csr` files are provided. Those files were created for debbuging purposes and store the Sparse matrices ins CSR format together with information regarding the size of the matrix and the number of non zero elements. Fianly the text files in the directory store performance data for executions with all the implementations (Serial and Parallel) for every graph.
 

## Useful links
- [Sparse Matrix operations](http://www.mathcs.emory.edu/~cheung/Courses/561/Syllabus/3-C/sparse.html)

