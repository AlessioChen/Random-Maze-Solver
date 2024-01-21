#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h> // For sleep function
#include "Maze.h"

using namespace std;

struct Point {
    int x, y;
};

struct Particle {
    Point position{};
    vector<Point> path;
};


bool isExitFound(const Point &particle, const Point &exit) {
    return particle.x == exit.x && particle.y == exit.y;
}

bool isValidMove(char **maze, int x, int y, int M, int N) {
    return x >= 0 && x < M && y >= 0 && y < N && maze[x][y] != '#' && maze[x][y] != 'S';
}

void randomMove(Particle &particle, char **maze, int M, int N) {

    const vector<pair<int, int>> directions = {{0,  1},
                                               {1,  0},
                                               {0,  -1},
                                               {-1, 0}};

    int i = rand() % directions.size();
    int newX = particle.position.x + directions[i].first;
    int newY = particle.position.y + directions[i].second;

    if (isValidMove(maze, newX, newY, M, N)) {
        maze[particle.position.x][particle.position.y] = ' ';
        particle.position.x = newX;
        particle.position.y = newY;
        maze[particle.position.x][particle.position.y] = '.';

        particle.path.push_back(particle.position);
    }

}


int main() {
    const int numParticles = 100;

    m = 50;
    n = 50;
    int M = 2 * m + 1;
    int N = 2 * n + 1;
    Point start = {1, 1};
    Point exit = {2* m , 2* n -1};

    char **maze = createRandomMaze(M, N);
    displayMaze(M, N, maze);
    sleep(1);

    char **solution = maze;


    srand(static_cast<unsigned>(time(nullptr)));
    vector<Particle> particles;

    for (int i = 0; i < numParticles; i++) {
        Particle p = {start};
        p.path.push_back(start);
        particles.emplace_back(p);
    }

    Particle firstToFindExit;

    while (all_of(particles.begin(), particles.end(), [&exit](const Particle &p) {
        return !isExitFound(p.position, exit);
    })) {
        for (Particle &particle: particles) {
            randomMove(particle, maze, M, N);

            if (isExitFound(particle.position, exit)) {
                firstToFindExit = particle;
                break;
            }
        }
//        displayMaze(M, N, maze);
    }


    for (Point &p: firstToFindExit.path) {
        solution[p.x][p.y] = '.';
    }

    displayMaze(M, N, solution);


}



