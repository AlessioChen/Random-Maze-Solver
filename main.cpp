#include <vector>
#include <ctime>
#include "Maze.cpp"
#include <omp.h>

using namespace std;
using namespace chrono;


#define MAZE_M 10
#define MAZE_N 10
#define N_TESTS 10
#define N_PARTICLES 100


char **sequentialSolver(vector<Particle> particles, Maze maze, Point exit);

char **parallelSolver(vector<Particle> particles, Maze maze, Point exit, bool exitFound);

void printSolution(char **solution, int M, int N);

void printVector(vector<float> v, string name);

int main() {

    Point start = {1, 1};
    Point exit = {2 * MAZE_M, 2 * MAZE_N - 1};

    vector<float> times = {};

    Maze maze(MAZE_M, MAZE_N);
    maze.generate();
    maze.displayMaze();


    srand(static_cast<unsigned>(time(nullptr)));
    vector<Particle> particles;

    for (int i = 0; i < N_PARTICLES; i++) {
        Particle p = {start};
        p.path.push_back(start);
        particles.emplace_back(p);
    }

    char **solution;
    auto startTime = chrono::system_clock::now();
    for (int i = 0; i < N_TESTS; i++) {
        solution = sequentialSolver(particles, maze, exit);
    }
    auto endTime = system_clock::now();
    cout << solution << endl;
    auto seqElapsed = duration_cast<milliseconds>(endTime - startTime) / N_TESTS;
    cout << "Sequential: " << seqElapsed.count() << "ms" << endl;
    cout << "-----------------------------------------" << endl;
    times.push_back(seqElapsed.count());

//    printSolution(solution, maze.M, maze.N);

    vector<int> threadTests = {2, 4, 6, 8, 16, 32, 64};
    vector<float> speedups = {};

    for (int i = 0; i < threadTests.size(); i++) {
        omp_set_num_threads(threadTests[i]);
        startTime = chrono::system_clock::now();
        bool exitFound = false;
        for (int j = 0; j < N_TESTS; j++) {
            solution = parallelSolver(particles, maze, exit, exitFound);
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
//    printSolution(solution, maze.M, maze.N);

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

char **parallelSolver(vector<Particle> particles, Maze maze, Point exit, bool exitFound) {


    Particle firstToFindExit;
    char **solution = maze.maze;

#pragma omp parallel shared(exitFound) {

#pragma omp for
    for (int i = 0; i < particles.size(); i++) {
        Particle particle = particles[i];

#pragma  omp critical
        while (!exitFound) {

            maze.randomMove(particle);
            if (maze.isExitFound(particle.position, exit)) {
                firstToFindExit = particle;
                exitFound = true;
            }
        }
    }

    for (int i = 0; i < firstToFindExit.path.size(); i++) {
        Point &p = firstToFindExit.path[i];
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