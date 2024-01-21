#include <iostream>
#include <stack>
#include <vector>
#include <random>

using namespace std;

struct Point {
    int x, y;
};

struct Particle {
    Point position;
    vector<Point> path;
};


class Maze {
public:
    char **maze;
    int m;
    int n;
    int M;
    int N;

    Maze(int _m = 4, int _n = 4) {
        m = _m;
        n = _n;
        M = 2 * m + 1;
        N = 2 * n +1;

        maze = new char *[M];

        for (int i = 0; i < M; i++) {
            maze[i] = new char[N];
        }

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (!(i & 1) || !(j & 1))
                    maze[i][j] = '#';
                else
                    maze[i][j] = ' ';

            }
        }

        for (int i = 1; i < M; i += 2) {
            for (int j = 1; j < N; j += 2) {
                maze[i][j] = ' ';
            }

        }


    }

    void displayMaze() {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                cout << maze[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    // A utility function to get the index of cell with indices x, y;
    int getIdx(int x, int y, vector<pair<int, pair<int, int> > > cell_list) {
        for (int i = 0; i < cell_list.size(); i++) {
            if (cell_list[i].second.first == x && cell_list[i].second.second == y)
                return cell_list[i].first;
        }
        cout << "getIdx() couldn't find the index!" << endl;
        return -1;
    }

    void generate() {

        vector<pair<int, pair<int, int> > > cell_list;
        vector<bool> visited(m * n, false);
        stack<pair<int, pair<int, int> > > m_stack;
        random_device rdev;
        mt19937 rng(rdev());
        uniform_int_distribution<mt19937::result_type> dist100(1, 100);

        int nVisited = 0;
        int k = 0;

        for (int i = 1; i < M; i += 2) {
            for (int j = 1; j < N; j += 2) {
                cell_list.push_back(make_pair(k, make_pair(i, j)));
                k++;
            }
        }

        int randIdx = dist100(rng) % m * n;
        m_stack.push(cell_list[randIdx]);
        visited[randIdx] = true;
        nVisited++;

        // Algo
        while (nVisited < m * n) {

            vector<int> neighbours;
            // North
            if (m_stack.top().second.first > 1) {
                if (maze[m_stack.top().second.first - 2][m_stack.top().second.second + 0] &&
                    !visited[getIdx(m_stack.top().second.first - 2, m_stack.top().second.second + 0, cell_list)]) {
                    neighbours.push_back(0);
                }
            }
            // East
            if (m_stack.top().second.second < N - 2) {
                if (maze[m_stack.top().second.first + 0][m_stack.top().second.second + 2] &&
                    !visited[getIdx(m_stack.top().second.first + 0, m_stack.top().second.second + 2, cell_list)]) {
                    neighbours.push_back(1);
                }
            }
            // South
            if (m_stack.top().second.first < M - 2) {
                if (maze[m_stack.top().second.first + 2][m_stack.top().second.second + 0] &&
                    !visited[getIdx(m_stack.top().second.first + 2, m_stack.top().second.second + 0, cell_list)]) {
                    neighbours.push_back(2);
                }
            }
            // West
            if (m_stack.top().second.second > 1) {
                if (maze[m_stack.top().second.first + 0][m_stack.top().second.second - 2] &&
                    !visited[getIdx(m_stack.top().second.first + 0, m_stack.top().second.second - 2, cell_list)]) {
                    neighbours.push_back(3);
                }
            }
            // Neighbours available?
            if (!neighbours.empty()) {
                // Choose a random direction
                int next_cell_dir = neighbours[dist100(rng) % neighbours.size()];
                // Create a path between this cell and neighbour
                switch (next_cell_dir) {
                    case 0: // North
                        maze[m_stack.top().second.first - 1][m_stack.top().second.second + 0] = ' ';
                        m_stack.push(cell_list[getIdx(m_stack.top().second.first - 2, m_stack.top().second.second + 0,
                                                      cell_list)]);
                        break;
                    case 1: // East
                        maze[m_stack.top().second.first + 0][m_stack.top().second.second + 1] = ' ';
                        m_stack.push(cell_list[getIdx(m_stack.top().second.first + 0, m_stack.top().second.second + 2,
                                                      cell_list)]);
                        break;
                    case 2: // South
                        maze[m_stack.top().second.first + 1][m_stack.top().second.second + 0] = ' ';
                        m_stack.push(cell_list[getIdx(m_stack.top().second.first + 2, m_stack.top().second.second + 0,
                                                      cell_list)]);
                        break;
                    case 3: // West
                        maze[m_stack.top().second.first + 0][m_stack.top().second.second - 1] = ' ';
                        m_stack.push(cell_list[getIdx(m_stack.top().second.first + 0, m_stack.top().second.second - 2,
                                                      cell_list)]);
                        break;
                }

                visited[m_stack.top().first] = true;
                nVisited++;
            } else {
                m_stack.pop();
            }
        }

        maze[0][1] = 'S';
        maze[2 * m][2 * n - 1] = 'E';
    }

    bool isExitFound(const Point &particle, const Point &exit) {
        return particle.x == exit.x && particle.y == exit.y;
    }

    bool isValidMove(int x, int y) {
        return x >= 0 && x < M && y >= 0 && y < N && maze[x][y] != '#' && maze[x][y] != 'S';
    }

    void randomMove(Particle &particle) {

        const vector<pair<int, int>> directions = {{0,  1},
                                                   {1,  0},
                                                   {0,  -1},
                                                   {-1, 0}};

        int i = rand() % directions.size();
        int newX = particle.position.x + directions[i].first;
        int newY = particle.position.y + directions[i].second;

        if (isValidMove(newX, newY)) {
            maze[particle.position.x][particle.position.y] = ' ';
            particle.position.x = newX;
            particle.position.y = newY;
            maze[particle.position.x][particle.position.y] = '.';

            particle.path.push_back(particle.position);
        }

    }



};