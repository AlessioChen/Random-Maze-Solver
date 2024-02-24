# Random Maze Solver with OpenMP

## Introduction
This repository contains a C++ implementation of a Random Maze Solver utilizing the OpenMP multi-threading API for parallelization. The project compares the performance of sequential and parallel programs in solving randomly generated mazes.

## Features
- Generates random mazes using a Depth-First Search (DFS) algorithm.
- Implements a Random Maze Solver algorithm using particles that move through the maze.
- Provides both sequential and parallel implementations of the solver.
- Evaluates and compares the performance of sequential and parallel programs under different scenarios.
- Visualizes the generated mazes and their solutions.

## Setup
To run the code, ensure you have a C++ compiler that supports OpenMP installed on your system.

## Usag

1. Clone the repository to your local machine.
2. Compile the code using your preferred C++ compiler with OpenMP support.
3. Run the executable to generate and solve random mazes.

## Tests
The repository includes tests to evaluate the performance of the parallel solver under different conditions:
- **Thread Scalability Evaluation**: Assesses the impact of increasing the number of threads while maintaining a fixed number of particles.
- **Particle Scalability Evaluation**: Evaluates performance variations by altering the number of particles navigating the maze.

## Results
The results of the tests are presented in the accompanying report file. They demonstrate significant performance improvements achieved through parallelization, with speedup values generally increasing with the number of threads.
