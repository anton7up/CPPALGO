#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <utility>
#include <chrono>

struct Rectangle {
    std::pair<int, int> start, end;
};

struct Node {
    int modifier = 0;
    int from = 0, to = 0;
    Node* left = nullptr; Node* right = nullptr;
};

struct Event {
    int start, end, operation;
};

struct comparator {
    bool operator()(const std::pair<int, Event>& a, const std::pair<int, Event>& b) const {
        if (a.first == b.first) {
            if (a.second.start == b.second.start) {
                if (a.second.end == b.second.end) {
                    return a.second.operation < b.second.operation;
                }
                return a.second.end < b.second.end;
            }
            return a.second.start < b.second.start;
        }
        return a.first < b.first;
    }
};

Node* createNodes(int numberOfNodes, int offset = 1) {
    if (numberOfNodes == 1) {
        Node* leaf = new Node;
        leaf->from = leaf->to = offset;
        return leaf;
    }
    int partition = (numberOfNodes + 1) / 2;
    Node* left_branch = createNodes(partition, offset);
    Node* right_branch = createNodes(numberOfNodes - partition, offset + partition);
    Node* result = new Node;
    result->left = left_branch;
    result->right = right_branch;
    result->from = offset;
    result->to = offset + numberOfNodes - 1;
    return result;
}

Node* copyNode(Node* node) {
    Node* copy = new Node;
    copy->modifier = node->modifier;
    copy->from = node->from;
    copy->to = node->to;
    copy->left = node->left;
    copy->right = node->right;
    return copy;
}

void deleteTree(Node* root) {
    if (root->left) {
        deleteTree(root->left);
    }
    if (root->right) {
        deleteTree(root->right);
    }
    delete root;
}

void operation(std::vector<Node*>& trees, bool isChanged, Node* parent, Node* root, int start, int end, int number) {
    if (end <= root->from || root->to < start) {
        return;
    }
    Node* changed = root;
    if (isChanged || parent != nullptr) {
        Node* copy = copyNode(root);
        if (parent == nullptr) {
            trees.push_back(copy);
        }
        else {
            if (parent->left == root) {
                parent->left = copy;
            }
            else {
                parent->right = copy;
            }
        }
        changed = copy;
    }

    if ((start <= changed->from && changed->from < end) && (start <= changed->to && changed->to < end)) {
        changed->modifier += number;
    }
    else {
        operation(trees, isChanged, changed, changed->left, start, end, number);
        operation(trees, isChanged, changed, changed->right, start, end, number);
    }
    return;
}

int goToLeaf(std::vector<Node*>& trees, int x, int y, int numberOfLeaves) {
    if (0 < x && 0 < y && y <= numberOfLeaves) {
        Node* root = trees[x];
        int sumOfModifier = 0;
        while (root->left != nullptr) {
            sumOfModifier += root->modifier;
            if (root->left->from <= y && y <= root->left->to) {
                root = root->left;
            }
            else {
                root = root->right;
            }
        }
        sumOfModifier += root->modifier;
        return sumOfModifier;
    }
    return 0;
}

std::pair<size_t, size_t> third(std::vector<Rectangle>& rectangles, std::vector<std::pair<int, int>>& points) {
    std::pair<size_t, size_t> times;

    if (rectangles.empty()) {
        for (size_t i = 0; i < points.size(); ++i) {
            int result = 0;
            std::cout << result << ' ';
        }
        return times;
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    std::set<int> x, y;
    for (auto rectangle : rectangles) {
        x.insert(rectangle.start.first); x.insert(rectangle.end.first);
        y.insert(rectangle.start.second); y.insert(rectangle.end.second);
    }
    std::vector<int> sortedX(x.cbegin(), x.cend());
    std::vector<int> sortedY(y.cbegin(), y.cend());

    std::set<std::pair<int, Event>, comparator> events;
    for (auto rectangle : rectangles) {
        std::pair<int, Event> event;
        event.first = std::distance(sortedX.begin(), std::upper_bound(sortedX.begin(), sortedX.end(), rectangle.start.first));
        event.second.operation = 1;
        event.second.start = std::distance(sortedY.begin(), std::upper_bound(sortedY.begin(), sortedY.end(), rectangle.start.second));
        event.second.end = std::distance(sortedY.begin(), std::upper_bound(sortedY.begin(), sortedY.end(), rectangle.end.second));
        events.insert(event);

        event.first = std::distance(sortedX.begin(), std::upper_bound(sortedX.begin(), sortedX.end(), rectangle.end.first));
        event.second.operation = -1;
        events.insert(event);
    }

    int numberOfLeaves = y.size() - 1;
    Node* root = createNodes(numberOfLeaves);
    std::vector<Node*> trees;
    trees.reserve(x.size() + 1);
    trees.push_back(root);
    for (auto event : events) {
        if (event.first != (trees.size() - 1)) {
            operation(trees, true, nullptr, trees[trees.size() - 1], event.second.start, event.second.end, event.second.operation);
        }
        else {
            operation(trees, false, nullptr, trees[trees.size() - 1], event.second.start, event.second.end, event.second.operation);
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    times.first = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    t0 = std::chrono::high_resolution_clock::now();
    for (auto point : points) {
        int pos_x = std::distance(sortedX.begin(), std::upper_bound(sortedX.begin(), sortedX.end(), point.first));
        int pos_y = std::distance(sortedY.begin(), std::upper_bound(sortedY.begin(), sortedY.end(), point.second));
        int result = goToLeaf(trees, pos_x, pos_y, numberOfLeaves);
        std::cout << result << ' ';
    }
    t1 = std::chrono::high_resolution_clock::now();
    times.second = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    return times;
}

int main() {
    std::vector<Rectangle> rectangles;
    std::vector<std::pair<int, int>> points;

    int n = 0, m = 0;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        std::cin >> x1 >> y1 >> x2 >> y2;
        std::pair<int, int> start(x1, y1);
        std::pair<int, int> end(x2, y2);
        Rectangle rectangle;
        rectangle.start = start;
        rectangle.end = end;
        rectangles.push_back(rectangle);
    }
    std::cin >> m;
    for (int i = 0; i < m; ++i) {
        int x1 = 0, y1 = 0;
        std::cin >> x1 >> y1;
        std::pair<int, int> point(x1, y1);
        points.push_back(point);
    }

    third(rectangles, points);

    return 0;
}