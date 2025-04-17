#include "algheader.h"
#include <set>

std::pair<size_t, size_t> second(std::vector<Rectangle>& rectangles, std::vector<std::pair<int, int>>& points) {
    std::pair<size_t, size_t> times;

    auto t0 = std::chrono::high_resolution_clock::now();
    std::set<int> x, y;
    for (auto rectangle : rectangles) {
        x.insert(rectangle.start.first); x.insert(rectangle.end.first);
        y.insert(rectangle.start.second); y.insert(rectangle.end.second);
    }
    std::vector<int> sortedX(x.cbegin(), x.cend());
    std::vector<int> sortedY(y.cbegin(), y.cend());

    int** matrix = new int* [x.size()];
    for (size_t i = 0; i < x.size(); ++i) {
        matrix[i] = new int[y.size()];
        for (size_t j = 0; j < y.size(); ++j) {
            matrix[i][j] = 0;
        }
    }

    for (auto rectangle : rectangles) {
        for (size_t x_pos = std::distance(sortedX.begin(), std::upper_bound(sortedX.begin(), sortedX.end(), rectangle.start.first)) - 1; x_pos < (std::distance(sortedX.begin(), std::upper_bound(sortedX.begin(), sortedX.end(), rectangle.end.first)) - 1); ++x_pos) {
            for (size_t y_pos = std::distance(sortedY.begin(), std::upper_bound(sortedY.begin(), sortedY.end(), rectangle.start.second)) - 1; y_pos < (std::distance(sortedY.begin(), std::upper_bound(sortedY.begin(), sortedY.end(), rectangle.end.second)) - 1); ++y_pos) {
                ++matrix[x_pos][y_pos];
            }
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    times.first = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    t0 = std::chrono::high_resolution_clock::now();
    for (auto point : points) {
        int pos_x = std::distance(sortedX.begin(), std::upper_bound(sortedX.begin(), sortedX.end(), point.first)) - 1;
        int pos_y = std::distance(sortedY.begin(), std::upper_bound(sortedY.begin(), sortedY.end(), point.second)) - 1;
        int result = 0;
        if (!(pos_x < 0 || pos_y < 0)) {
            result = matrix[pos_x][pos_y];
        }
        //std::cout << result << ' ';
    }
    t1 = std::chrono::high_resolution_clock::now();
    times.second = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    for (size_t i = 0; i < x.size(); ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;

    return times;
}