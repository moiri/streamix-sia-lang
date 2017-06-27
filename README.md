# streamix-sia-lang
A small language to define sias and compile them into a graph file

## 1. Installation and Requirements
The tool requires [`flex`](https://github.com/westes/flex), [`bison`](https://www.gnu.org/software/bison/), and [`igraph`](http://igraph.org/c/)

To install the tool type

    git clone --recursive https://github.com/moiri/streamix-sia-lang.git
    make
    sudo make install

Execuatables will be installed into `/usr/local/bin/`.
Make sure that `/usr/local/bin/` is in your `PATH` environment variable.

In order to install the tool, the following packages and libraries have to be installed:

### 1.1. [`flex`](https://github.com/westes/flex)
This is used for lexing the Streamix code. To install on an apt-based linux sytem type

    sudo apt update
    sudo apt install flex

### 1.2. [`bison`](https://www.gnu.org/software/bison/)
This is used for parsing the Streamix code. To install on an apt-based linux system type

    sudo apt update
    sudo apt install bison

### 1.3. [`igraph`](http://igraph.org/c/)
This is used to build depedency graphs. Make sure GraphML is enabled (see http://igraph.org/c/ for details)

    cd igraph
    ./configure
    make
    make check
    sudo make install

## 2. Usage

    ./smxc [OPTION...] FILE

    Options:
      -h            This message
      -v            Version
      -o 'path'     Path to store the generated files
      -f 'format'   Format of the graph either 'gml' or 'graphml'
