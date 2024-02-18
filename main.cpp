#include <vector>
#include "Maze.cpp"
#include <omp.h>

using namespace std;
using namespace chrono;

#define MAZE_M 20
#define MAZE_N 20
#define N_TESTS 10
#define N_PARTICLES 1000
#define FIXED_SEED 42


char **sequentialSolver(vector<Particle> particles, Maze maze, Point exit);

char **parallelSolver(vector<Particle> particles, Maze maze, Point exit, int threadNums);

void printSolution(char **solution, int M, int N);

void printVector(vector<float> v, string name);

int main() {

    Point start = {1, 1};
    Point exit = {2 * MAZE_M, 2 * MAZE_N - 1};

    vector<float> times = {};
    srand(FIXED_SEED);

    Maze maze(MAZE_M, MAZE_N);
    maze.generate();
    maze.displayMaze();

    char **solution;

    vector<Particle> particles;

    for (int i = 0; i < N_PARTICLES; i++) {
        Particle p = {start};
        p.path.push_back(start);
        particles.push_back(p);
    }

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


    vector<int> threadTests = {2, 4, 6, 8, 16, 32, 64};
    vector<float> speedups = {};

    for (int i = 0; i < threadTests.size(); i++) {

        startTime = system_clock::now();
        for (int j = 0; j < N_TESTS; j++) {
            vector<Particle> particles_copy = particles;
            Maze maze_copy = maze;
            solution = parallelSolver(particles_copy, maze_copy, exit, threadTests[i]);
        }
        endTime = system_clock::now();

        cout << solution << endl;
        auto elapsed = duration_cast<milliseconds>(endTime - startTime) / N_TESTS;
        cout << "Parallel [t= " << threadTests[i] << "]: " << elapsed.count() << "ms" << endl;
        cout << "Speedup: " << (float) seqElapsed.count() / elapsed.count() << "x" << endl;
        cout << "-----------------------------------------" << endl;
        times.push_back(elapsed.count());
        speedups.push_back((float) seqElapsed.count() / elapsed.count());
    }

    printVector(times, "Times");
    printVector(speedups, "Speedups");
}


char **sequentialSolver(vector<Particle> particles, Maze maze, Point exit) {

    Particle firstToFindExit;
    char **solution = maze.maze;

    while (all_of(particles.begin(), particles.end(),
                  [&exit, &maze](const Particle &p) {
                      return !maze.isExitFound(p.position, exit);
                  })) {
        for (Particle &particle: particles) {
            maze.randomMove(particle);

            if (maze.isExitFound(particle.position, exit)) {
                firstToFindExit = particle;
                break;
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


void printSolution(char **solution, int M, int N) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
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
