#pragma once
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <memory>

class AISolver {
public:
    struct Node {
        int x = 0;
        int y = 0;
        int g = 0;
        int h = 0;
        std::shared_ptr<Node> parent = nullptr;
        int f() const { return g + h; }
    };

    static int heuristic(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2); // Manhattan
    }

    // --- A* ALGORITHM ---
    static std::vector<std::pair<int, int>> findPath(
        int startX, int startY, int goalX, int goalY,
        const std::vector<std::vector<int>>& grid)
    {
        std::vector<std::pair<int, int>> path;
        if (grid.empty()) return path;

        int rows = (int)grid.size();
        int cols = (int)grid[0].size();

        // Safety: Check bounds and if goal is an obstacle
        if (goalX < 0 || goalX >= cols || goalY < 0 || goalY >= rows || grid[goalY][goalX] != 0) {
            return path;
        }

        auto cmp = [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->f() > b->f();
            };

        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, decltype(cmp)> open(cmp);
        std::vector<std::vector<int>> gScore(rows, std::vector<int>(cols, 1e9));

        auto startNode = std::make_shared<Node>();
        startNode->x = startX; startNode->y = startY; startNode->g = 0;
        startNode->h = heuristic(startX, startY, goalX, goalY);

        open.push(startNode);
        gScore[startY][startX] = 0;

        const int dx[4] = { 1, -1, 0, 0 };
        const int dy[4] = { 0, 0, 1, -1 };

        while (!open.empty()) {
            auto current = open.top();
            open.pop();

            if (current->x == goalX && current->y == goalY) {
                auto ptr = current;
                while (ptr) {
                    path.push_back({ ptr->x, ptr->y });
                    ptr = ptr->parent;
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            // Unutar AISolver klase u findPath metodi:
// ... (ostali dio koda je isti do petlje) ...

            for (int i = 0; i < 4; i++) {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];

                if (nx >= 0 && nx < cols && ny >= 0 && ny < rows && grid[ny][nx] == 0) {

                    // --- KLJUČ ZA ZIG-ZAG: Dodajemo kaznu za skretanje ---
                    int turnPenalty = 0;
                    if (current->parent) {
                        int prevDx = current->x - current->parent->x;
                        int prevDy = current->y - current->parent->y;
                        int currentDx = nx - current->x;
                        int currentDy = ny - current->y;

                        // Ako se smjer mijenja (skreće), dodaj malu kaznu
                        if (prevDx != currentDx || prevDy != currentDy) {
                            turnPenalty = 1; // Mala kazna je dovoljna da preferira ravnu liniju
                        }
                    }

                    int tentative_g = current->g + 1 + turnPenalty;

                    if (tentative_g < gScore[ny][nx]) {
                        gScore[ny][nx] = tentative_g;
                        auto neighbor = std::make_shared<Node>();
                        neighbor->x = nx; neighbor->y = ny;
                        neighbor->g = tentative_g;
                        // Da bi bila još preciznija, koristi malo pojačanu heuristiku (tie-breaker)
                        neighbor->h = heuristic(nx, ny, goalX, goalY);
                        neighbor->parent = current;
                        open.push(neighbor);
                    }
                }
            }
        }
        return path;
    }

    // --- SURVIVAL FALLBACK ---
    static std::pair<int, int> findSafeMove(int startX, int startY, const std::vector<std::vector<int>>& grid) {
        const int dx[4] = { 1, -1, 0, 0 };
        const int dy[4] = { 0, 0, 1, -1 };
        int rows = (int)grid.size();
        int cols = (int)grid[0].size();

        std::pair<int, int> bestMove = { startX, startY };
        int maxOpenNeighbors = -1;

        for (int i = 0; i < 4; i++) {
            int nx = startX + dx[i];
            int ny = startY + dy[i];

            if (nx >= 0 && ny >= 0 && nx < cols && ny < rows && grid[ny][nx] == 0) {
                int openNeighbors = 0;
                for (int j = 0; j < 4; j++) {
                    int nnx = nx + dx[j];
                    int nny = ny + dy[j];
                    if (nnx >= 0 && nnx < cols && nny >= 0 && nny < rows && grid[nny][nnx] == 0) {
                        openNeighbors++;
                    }
                }

                if (openNeighbors > maxOpenNeighbors) {
                    maxOpenNeighbors = openNeighbors;
                    bestMove = { nx, ny };
                }
            }
        }
        return bestMove;
    }
};