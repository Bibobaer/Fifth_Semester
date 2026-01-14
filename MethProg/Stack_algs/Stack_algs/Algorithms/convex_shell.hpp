#pragma once

#include <stack>
#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>

namespace Shell {
    using Point = std::pair<double, double>;

    inline double crossProduct(const Point& a, const Point& b) {
        return a.first * b.second - b.first * a.second;
    }

    inline double get_distance(const Point& a, const Point& b) {
        return std::sqrt((a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second));
    }

    inline double orientation(const Point& a, const Point& b, const Point& c) {
        return crossProduct(std::make_pair(b.first - a.first, b.second - a.second), std::make_pair(c.first - a.first, c.second - a.second));
    }

    bool compare(const Point& p0, const Point& a, const Point& b) {
        double orient = orientation(p0, a, b);

        if (orient == 0)
            return get_distance(p0, a) - get_distance(p0, b);

        return orient > 0;
    }

    std::vector<Point> make_convex_shell(std::vector<Point>& points) {
        size_t n = points.size();
        if (points.size() <= 3)
            return points;

        auto minEl = std::min_element(points.begin(), points.end(), [](const Point& a, const Point& b) {return a.second < b.second; });
        std::iter_swap(begin(points), minEl);

        Point p0 = points[0];

        std::sort(points.begin() + 1, points.end(), [&p0](const Point& a, const Point& b) { return compare(p0, a, b); });

        std::stack<Point> S;
        S.push(points[0]);
        S.push(points[1]);

        for (size_t c = 2; c < n; c++) {
            while (true) {
                Point b = S.top(); S.pop();
                Point a = S.top();

                Point vecAB = { b.first - a.first, b.second - a.second };
                Point vecBC = { points[c].first - b.first, points[c].second - b.second };
                
                if (crossProduct(vecAB, vecBC) > 0) {
                    S.push(b);
                    S.push(points[c]);
                    break;
                }
            }
        }

        std::vector<Point> shell;

        while (S.size()) {
            shell.push_back(std::move(S.top())); S.pop();
        }
        return shell;
    }
}

/*


// Функция для определения направления поворота
// Возвращает:
// > 0 - против часовой стрелки
// < 0 - по часовой стрелке
// = 0 - коллинеарны
double orientation(const Point& a, const Point& b, const Point& c) {
    return crossProduct(b - a, c - a);
}

// Функция сравнения для сортировки по полярному углу
bool compare(const Point& p0, const Point& a, const Point& b) {
    double orient = orientation(p0, a, b);

    if (orient == 0) {
        // Если точки коллинеарны, сортируем по расстоянию от p0
        return distance(p0, a) < distance(p0, b);
    }

    return orient > 0; // Сортируем против часовой стрелки
}

// Основная функция для построения выпуклой оболочки
vector<Point> convexHull(vector<Point>& points) {
    int n = points.size();
    if (n < 3) {
        return points; // Выпуклая оболочка требует минимум 3 точки
    }

    // Шаг i: Находим самую нижнюю точку (и самую левую при равенстве y)
    int minIndex = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].y < points[minIndex].y ||
           (points[i].y == points[minIndex].y && points[i].x < points[minIndex].x)) {
            minIndex = i;
        }
    }

    // Шаг ii: Переставляем самую нижнюю точку на 0-ой индекс
    swap(points[0], points[minIndex]);
    Point p0 = points[0];

    // Шаг iii-iv: Сортируем оставшиеся точки по полярному углу
    sort(points.begin() + 1, points.end(), [&p0](const Point& a, const Point& b) {
        return compare(p0, a, b);
    });

    // Шаг v: Обход Грэхема
    stack<Point> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    for (int i = 3; i < n; i++) {
        while (S.size() >= 2) {
            Point b = S.top();
            S.pop();
            Point a = S.top();

            // Если поворот от ab к points[i] против часовой стрелки
            if (orientation(a, b, points[i]) > 0) {
                S.push(b); // Возвращаем b обратно
                break;
            }
        }
        S.push(points[i]);
    }

    // Преобразуем стек в вектор
    vector<Point> hull;
    while (!S.empty()) {
        hull.push_back(S.top());
        S.pop();
    }

    // Разворачиваем вектор, так как в стеке точки в обратном порядке
    reverse(hull.begin(), hull.end());

    return hull;
}

// Вспомогательная функция для вывода точек
void printPoints(const vector<Point>& points) {
    for (const auto& p : points) {
        cout << "(" << p.x << ", " << p.y << ") ";
    }
    cout << endl;
}
*/