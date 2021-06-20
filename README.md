# ICCAD-2021-B
Problem B: Routing with Cell Movement Advanced

## 1. How to Build
**Step 1:** Download the source code. For example,
~~~
$ git clone --recursive https://github.com/jacky860226/ICCAD-2021-B.git
~~~

or

~~~
$ git clone https://github.com/jacky860226/ICCAD-2021-B.git
$ cd ICCAD-2021-B
$ git submodule init
$ git submodule update --recursive
$ cd ..
~~~

**Step 2:** Go to the project root and build by
~~~
$ cd ICCAD-2021-B
$ export BOOST_LIBRARY_PATH=your/boost/library/path
$ make
~~~

### 1.1. Dependencies

* [GCC](https://gcc.gnu.org/) (version >= 7.5.0) or other working c++ compliers
* [Boost C++ Libraries](https://beta.boost.org/) (version = 1.64.0)

## 2. How to run

~~~
$ ./cell_move_router <input.txt> [<output.txt>]
~~~

If there is no `<output.txt>`, it will use standard output.

## 3. How to test and evaluate
~~~
$ make test $(TEST_CASE_ID)
~~~
Example:
~~~
$ make test 1 && make test 2
~~~