<h1 align="center">Graph</h1>

## Description

 Implementation of the graph from TAOCP 7.1.2.6.S.

## How to integrate
 
 use [storage](https://github.com/baitim/ConanPackages), project = "graph", version = "1.0", user = "baitim"

## How to run

1. Clone <br>
    <code>git clone https://github.com/baitim/Graph.git</code>

2. Go to folder <br>
    <code>cd Graph</code>

3. Prepare conan <br>
    <code>uv sync --group dev; source .venv/bin/activate</code><br>
    <code>conan profile detect --force</code>

4. Init dependencies <br>
    <code>conan install . --build=missing -s compiler.cppstd=gnu20</code><br>
    maybe you will need these flags for the conan <code>-s build_type=Debug</code>

5. Build <br>
    <code>cmake --preset release; cmake --build build/Release</code>

6. Run <br>
    <code>./build/Release/src/graph</code>

## How to test

* Testing
    - End to end & Unit<br>
        <code>ctest --test-dir build/Release --output-on-failure</code>

<p align="center"><img src="https://github.com/baitim/Graph/blob/main/images/cat.gif" width="40%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**