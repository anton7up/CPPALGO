#include "algheader.h"

size_t first(std::vector<Rectangle>& rectangles, std::vector<std::pair<int, int>>& points) {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto point : points) {
        int counter = 0;
        for (auto rectangle : rectangles) {
            if ((rectangle.start.first <= point.first) && (point.first < rectangle.end.first)) {
                if ((rectangle.start.second <= point.second) && (point.second < rectangle.end.second)) {
                    ++counter;
                }
            }
        }
        
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
}