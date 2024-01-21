#include <vector>
#include <cstdlib>
#include <ctime>
#include "Maze.cpp"

using namespace std;
using namespace chrono;


#define MAZE_M 10
#define MAZE_N 5
#define N_TESTS 10
#define N_PARTICLES 100



char** sequentialSolver(vector<Particle> particles, Maze maze, Point exit);
void printSolution(char **solution, int M , int N);

int main() {

    Point start = {1, 1};
    Point exit = {2* MAZE_M , 2* MAZE_N -1};

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

    auto startTime = chrono::system_clock::now();
    for (int i = 0; i < N_TESTS; i++)
    {
        char**solution = sequentialSolver(particles, maze, exit);
    }
    auto endTime = system_clock::now();
    auto seqElapsed = duration_cast<milliseconds>(endTime - startTime) / N_TESTS;
    cout << "Sequential: " << seqElapsed.count() << "ms" << endl;
    cout << "-----------------------------------------" << endl;





}


char** sequentialSolver(vector<Particle> particles, Maze maze, Point exit){

    Particle firstToFindExit;
    char **solution = maze.maze;

    while (all_of(particles.begin(), particles.end(), [&exit, &maze](const Particle &p) {
        return !maze.isExitFound(p.position, exit);
    })) {
        for (Particle &particle: particles) {
            maze.randomMove(particle);

            if (maze.isExitFound(particle.position, exit)) {
                firstToFindExit = particle;
                break;
            }
        }
//        displayMaze(M, N, maze);
    }


    for (Point &p: firstToFindExit.path) {
        solution[p.x][p.y] = '.';
    }

   return solution;
}


void printSolution(char **solution, int M , int N){
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << solution[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


