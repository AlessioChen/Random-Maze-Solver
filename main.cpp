#include <vector>
#include "Maze.cpp"
#include <omp.h>

using namespace std;
using namespace chrono;

#define MAZE_M 25
#define MAZE_N 25
#define N_TESTS 20
#define FIXED_SEED 42

char **sequentialSolver(vector<Particle> particles, Maze maze, Point exit);

char **parallelSolver(vector<Particle> particles, Maze maze, Point exit, int threadNums);

void printSolution(char **solution);

void printVector(vector<float> v, string name);

int main() {

    srand(FIXED_SEED);

    Point start = {1, 1};
    Point exit = {2 * MAZE_M, 2 * MAZE_N - 1};


    Maze maze(MAZE_M, MAZE_N);
    maze.generate();
    maze.displayMaze();


    vector<Particle> particles;
    vector<int> N_PARTICLES = {500, 1000, 2500, 5000, 10000};
    vector<int> N_THREADS = {2, 4, 6, 8, 10, 16, 32, 64};

    char **solution;

    for (auto n: N_PARTICLES) {

        cout << n << " particles" << endl << endl;

        // Initializations
        vector<float> times = {};
        vector<float> speedups = {};
        particles.clear();

        for (int i = 0; i < n; i++) {
            Particle p = {start};
            p.path.push_back(start);
            particles.push_back(p);
        }

        // Sequential tests
        auto startTime = system_clock::now();
        for (int i = 0; i < N_TESTS; i++) {
            vector<Particle> particles_copy = particles;
            Maze maze_copy = maze;
            solution = sequentialSolver(particles_copy, maze_copy, exit);
        }
        auto endTime = system_clock::now();
        cout << solution << endl;
        auto seqElapsed = duration_cast<milliseconds>(endTime - startTime) / N_TESTS;
        cout << "Sequential: " << seqElapsed.count() << "ms" << endl;
        cout << "-----------------------------------------" << endl;
        times.push_back(seqElapsed.count());


        // Parallel tests
        for (auto n_thread: N_THREADS) {
            startTime = system_clock::now();
            for (int j = 0; j < N_TESTS; j++) {
                vector<Particle> particles_copy = particles;
                Maze maze_copy = maze;
                solution = parallelSolver(particles_copy, maze_copy, exit, n_thread);
            }
            endTime = system_clock::now();

            cout << solution << endl;
            auto elapsed = duration_cast<milliseconds>(endTime - startTime) / N_TESTS;
            cout << "Parallel [t= " << n_thread << "]: " << elapsed.count() << "ms" << endl;
            cout << "Speedup: " << (float) seqElapsed.count() / elapsed.count() << "x" << endl;
            cout << "-----------------------------------------" << endl;
            times.push_back(elapsed.count());
            speedups.push_back((float) seqElapsed.count() / elapsed.count());
        }

        // Print results
        printVector(times, "Times");
        printVector(speedups, "Speedups");

    }

    printSolution(solution);

}


char **sequentialSolver(vector<Particle> particles, Maze maze, Point exit) {
    Particle firstToFindExit;
    bool solutionFound = false;
    char **solution = maze.maze;

    while (!solutionFound) {
        for (Particle &particle: particles) {
            if (!solutionFound) {
                maze.randomMove(particle);

                if (maze.isExitFound(particle.position, exit)) {
                    firstToFindExit = particle;
                    solutionFound = true;
                }
            }
        }
    }

    for (Point &p: firstToFindExit.path) {
        solution[p.x][p.y] = '.';
    }

    return solution;
}


char **parallelSolver(std::vector<Particle> particles, Maze maze, Point exit, int threadNums) {
    char **solution = maze.maze;
    bool exitFound = false;
    Particle firstToFindExit;
    int partitionSize = particles.size() / threadNums;

#pragma omp parallel num_threads(threadNums) shared(particles, maze, exit, partitionSize)
    {

        int threadId = omp_get_thread_num();
        int start = partitionSize * threadId;
        int end = (threadId == threadNums - 1) ? particles.size() : start + partitionSize;

        while (!exitFound) {
            for (int i = start; i < end; i++) {
                if (!exitFound) {
                    maze.randomMove(particles[i]);

                    if (maze.isExitFound(particles[i].position, exit)) {
#pragma omp critical
                        {
                            if (!exitFound) {
                                firstToFindExit = particles[i];
                                exitFound = true;
                            }
                        }
                    }
                }
            }
        }

    }

    for (const Point &p: firstToFindExit.path) {
        solution[p.x][p.y] = '.';
    }

    return solution;
}


void printSolution(char **solution) {
    solution[2 * MAZE_M][2 * MAZE_N - 1] = 'E';
    for (int i = 0; i < 2 * MAZE_M + 1; i++) {
        for (int j = 0; j < 2 * MAZE_N + 1; j++) {
            cout << solution[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


void printVector(vector<float> v, string name) {
    for (int i = 0; i < v.size(); i++) {
        if (i == 0)
            cout << name << ": [";
        cout << v[i];
        if (i != v.size() - 1)
            cout << ", ";
        else
            cout << "]" << endl;
    }
}
