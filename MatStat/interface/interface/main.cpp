#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <cmath>
#include <iomanip>

#include <raylib.h>


// Парсинг группированной выборки (как в 713List.txt)
std::vector<double> parseGroupedSample(const std::string& filename) {
    std::vector<double> result;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;

    while (std::getline(file, line)) {
        // Пропускаем пустые строки
        if (line.empty()) continue;

        // Парсим строку вида "4.31       |       2"
        std::regex pattern(R"((-?\d*\.?\d+)\s*\|\s*(\d+))");
        std::smatch match;

        if (std::regex_search(line, match, pattern)) {
            if (match.size() == 3) {
                double value = std::stod(match[1].str());
                int frequency = std::stoi(match[2].str());

                // Добавляем значение столько раз, сколько указано в частоте
                for (int i = 0; i < frequency; ++i) {
                    result.push_back(value);
                }
            }
        }
    }

    file.close();
    return result;
}

// Парсинг интервальной выборки (как в 721Interval.txt)
std::map<double, double> parseIntervalSample(const std::string& filename) {
    std::map<double, double> result;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;

    while (std::getline(file, line)) {
        // Пропускаем пустые строки
        if (line.empty()) continue;

        // Парсим строку вида "[     2.1,      2.2] |         14"
        std::regex pattern(R"(\[\s*([\d.]+)\s*,\s*([\d.]+)\]\s*\|\s*(\d+))");
        std::smatch match;

        if (std::regex_search(line, match, pattern)) {
            if (match.size() == 4) {
                double left = std::stod(match[1].str());
                double right = std::stod(match[2].str());

                result[left] = right;
            }
        }
    }

    file.close();
    return result;
}

// Вспомогательная функция для парсинга частот из интервальной выборки
std::map<double, int> parseIntervalFrequencies(const std::string& filename) {
    std::map<double, int> frequencies;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;

    while (std::getline(file, line)) {
        // Пропускаем пустые строки
        if (line.empty()) continue;

        // Парсим строку вида "[     2.1,      2.2] |         14"
        std::regex pattern(R"(\[\s*([\d.]+)\s*,\s*([\d.]+)\]\s*\|\s*(\d+))");
        std::smatch match;

        if (std::regex_search(line, match, pattern)) {
            if (match.size() == 4) {
                double left = std::stod(match[1].str());
                int frequency = std::stoi(match[3].str());

                frequencies[left] = frequency;
            }
        }
    }

    file.close();
    return frequencies;
}

// Автоматическое определение типа выборки по имени файла
void parseSample(const std::string& filename,
    std::vector<double>& groupedSample,
    std::map<double, double>& intervalSample) {

    // Определяем тип по имени файла
    if (filename.find("List") != std::string::npos) {
        groupedSample = parseGroupedSample(filename);
    }
    else if (filename.find("Interval") != std::string::npos) {
        intervalSample = parseIntervalSample(filename);
    }
    else {
        // Попробуем автоматически определить по содержимому
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string firstLine;
        std::getline(file, firstLine);
        file.close();

        // Проверяем, содержит ли строка квадратные скобки (интервальная выборка)
        if (firstLine.find('[') != std::string::npos &&
            firstLine.find(']') != std::string::npos) {
            intervalSample = parseIntervalSample(filename);
        }
        else {
            groupedSample = parseGroupedSample(filename);
        }
    }
}

// Конвертация интервальной выборки в группированную (используем середины интервалов)
std::vector<double> convertIntervalToGrouped(const std::map<double, double>& intervalSample) {
    std::vector<double> result;

    for (const auto& interval : intervalSample) {
        double left = interval.first;
        double right = interval.second;
        double midpoint = (left + right) / 2.0;
        result.push_back(midpoint);
    }

    return result;
}

// Конвертация интервальной выборки в группированную с учетом частот из файла
std::vector<double> convertIntervalToGroupedWithFrequency(
    const std::map<double, double>& intervalSample,
    const std::string& filename) {

    std::vector<double> result;
    std::map<double, int> frequencies = parseIntervalFrequencies(filename);

    for (const auto& interval : intervalSample) {
        double left = interval.first;
        double right = interval.second;
        double midpoint = (left + right) / 2.0;

        // Находим частоту для данного интервала
        auto freqIt = frequencies.find(left);
        int frequency = (freqIt != frequencies.end()) ? freqIt->second : 1;

        // Добавляем midpoint столько раз, сколько указано в частоте
        for (int i = 0; i < frequency; ++i) {
            result.push_back(midpoint);
        }
    }

    return result;
}

// Получение частот для интервальной выборки
std::map<double, int> getIntervalFrequencies(const std::string& filename) {
    return parseIntervalFrequencies(filename);
}

// Вспомогательная функция для печати группированной выборки
void printGroupedSample(const std::vector<double>& sample) {
    std::cout << "Grouped sample (" << sample.size() << " elements): ";
    for (size_t i = 0; i < std::min(sample.size(), size_t(10)); ++i) {
        std::cout << sample[i];
        if (i < std::min(sample.size(), size_t(10)) - 1) std::cout << ", ";
    }
    if (sample.size() > 10) {
        std::cout << ", ... (and " << sample.size() - 10 << " more)";
    }
    std::cout << std::endl;
}

// Вспомогательная функция для печати интервальной выборки
void printIntervalSample(const std::map<double, double>& sample) {
    std::cout << "Interval sample (" << sample.size() << " intervals):" << std::endl;
    for (const auto& interval : sample) {
        std::cout << "  [" << interval.first << " - " << interval.second << "]" << std::endl;
    }
}

std::map<double, int> reinterpretait_table(std::vector<double>& table) {
    std::map<double, int> out;
    for (auto& el : table) {
        out[el]++;
    }
    return out;
}

double get_interval(std::vector<double>& table) {
    auto max_messanger = *std::max_element(table.begin(), table.end());
    auto min_messanger = *std::min_element(table.begin(), table.end());

    return ((max_messanger - min_messanger) / (1 + 3.322 * std::log10(table.size())));
}

std::map<std::pair<double, double>, int> get_interval_table(std::vector<double>& table) {
    double h = get_interval(table);
    std::cout << "Interval width: " << h << '\n';
    auto new_table = reinterpretait_table(table);

    std::map<std::pair<double, double>, int> out;

    bool is_in_interval = true;
    double start;
    double last;
    for (auto& [k, v] : new_table) {
        if (is_in_interval) {
            start = k;
            last = k + h;
            is_in_interval = false;
            out[{start, last}] = v;
        }
        else {
            if (start <= k && k <= last) {
                out[{start, last}] += v;
            }
            else {
                start = k;
                last = k + h;
                out[{start, last}] = v;
            }
        }
    }
    return out;
}

enum type_dis {
    ispravlenaya,
    not_ispravlenaya
};

std::string doubleToString(double value, int precision = 4) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

double Mat_Waiting(std::vector<double>& table) {
    auto size = table.size();
    double sum = 0;
    for (auto& el : table) {
        sum += el;
    }
    return sum / size;
}

double Dispersia(std::vector<double>& table, type_dis a = ispravlenaya) {
    double mw = Mat_Waiting(table);
    auto size = a == ispravlenaya ? (table.size() - 1) : table.size();
    double sum = 0;

    for (auto& el : table) {
        sum += (el - mw) * (el - mw) / size;
    }
    return sum;
}


std::vector<std::pair<double, double>> get_empirical_distribution(std::vector<double>& table) {
    auto group_tab = reinterpretait_table(table);

    std::vector<std::pair<double, double>> empirical;
    int n = table.size();

    for (auto& [el, _] : group_tab) {
        int sum = 0;
        for (auto& [k, v] : group_tab) {
            if (k < el)
                sum += v;
            else
                break;
        }
        empirical.push_back({ el, ((double)sum / n) });
    }

    return empirical;
}

std::vector<std::pair<double, double>> get_teoresi_distribution(std::vector<double>& table) {
    auto group_tab = reinterpretait_table(table);

    std::vector<std::pair<double, double>> res_func;
    res_func.push_back({-1, 0});

    auto func = [](double x) -> double {
        if (x < 0) return 0;
        return 1 - std::exp(-0.01987 * x);
    };

    for (auto& [el, _] : group_tab) {
        res_func.push_back({el, func(el)});
    }

    return res_func;
}

int main() {
    const int screenWidth = 1800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Statistical Graphs");

    SetTargetFPS(60);
    std::string table_name = "./tables/Sem2List.txt";
    std::vector<double> groupTable;
    std::map<double, double> intervaledTable;
    parseSample(table_name, groupTable, intervaledTable);
    if (!intervaledTable.empty()) {
        groupTable = convertIntervalToGrouped(intervaledTable);
    }

    double matWaiting = Mat_Waiting(groupTable);
    double dispersia = Dispersia(groupTable);
    double dispersiaNotIsp = Dispersia(groupTable, not_ispravlenaya);

    std::cout << table_name << " mat waiting: " << matWaiting << '\n';
    std::cout << table_name << " Dispersia: " << dispersiaNotIsp << '\n';
    std::cout << table_name << " isp Dispersia: " << dispersia << '\n';
    std::cout << table_name << " size: " << groupTable.size() << '\n';

    auto intervalTable = get_interval_table(groupTable);
    auto empiricalDist = get_empirical_distribution(groupTable);

    auto temp = reinterpretait_table(groupTable);
    std::cout << "Group Table" << '\n';
    for (auto& [k, v] : temp) {
        std::cout << k << " : " << v << '\n';
    }
    /*std::cout << "Interval Table" << '\n';
    for (auto& [k, v] : intervalTable) {
        std::cout << "[ " << k.first << " - " << k.second << " ]" << " : " << v << '\n';
    }*/

    std::cout << "Empirical func\n";
    for (auto& el : empiricalDist) {
        std::cout << el.first << " : " << el.second << "\n";
    }
    std::cout << "Teoretical func\n";
    auto a = get_teoresi_distribution(groupTable);
    for (auto& el : a) {
        std::cout << el.first << " : " << el.second << "\n";
    }
    std::cout << "Test\n";
    for (int i = 0; i < empiricalDist.size(); i++) {
        auto& temp_sub = empiricalDist[i].second;
        std::cout << empiricalDist[i].first << " = " << std::abs(temp_sub - a[i].second) << " : " << std::abs(temp_sub - a[i+1].second) << "\n";
    }
    //std::cout << a.back().second << "\n";

    double minValue = INFINITY;
    double maxValue = -INFINITY;
    int maxFrequency = 0;
    double maxEmpirical = 1.0;

    for (auto& [interval, freq] : intervalTable) {
        minValue = std::min(minValue, interval.first);
        maxValue = std::max(maxValue, interval.second);
        maxFrequency = std::max(maxFrequency, freq);
    }

    const int margin = 80;
    const int graphWidth = (screenWidth - 4 * margin) / 3;
    const int graphHeight = screenHeight - 2 * margin;
    const int histogramX = margin;
    const int polygonX = margin * 2 + graphWidth;
    const int empiricalX = margin * 3 + graphWidth * 2;
    const float scaleX = graphWidth / (maxValue - minValue);
    const float scaleY = graphHeight / (maxFrequency + 2);
    const float empiricalScaleY = graphHeight / maxEmpirical;

    Font font = GetFontDefault();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawText(table_name.c_str(), screenWidth / 2 - MeasureText(table_name.c_str(), 24) / 2, 20, 24, BLACK);

        DrawRectangleLines(histogramX, margin, graphWidth, graphHeight, BLACK);
        DrawText("Histogram", histogramX + graphWidth / 2 - MeasureText("Histogram", 20) / 2, margin - 30, 20, BLACK);

        DrawRectangleLines(polygonX, margin, graphWidth, graphHeight, BLACK);
        DrawText("Frequency Polygon", polygonX + graphWidth / 2 - MeasureText("Frequency Polygon", 20) / 2, margin - 30, 20, BLACK);

        DrawRectangleLines(empiricalX, margin, graphWidth, graphHeight, BLACK);
        DrawText("Empirical Distribution", empiricalX + graphWidth / 2 - MeasureText("Empirical Distribution", 20) / 2, margin - 30, 20, BLACK);

        DrawText("Value", screenWidth / 2 - MeasureText("Value", 20) / 2, screenHeight - 30, 20, BLACK);
        DrawTextPro(font, "Frequency", { 20, screenHeight / 2 }, { 0, 0 }, -90, 20, 1, BLACK);

        DrawText(("Mean: " + doubleToString(matWaiting)).c_str(), 20, 20, 20, BLUE);
        DrawText(("Variance: " + doubleToString(dispersia)).c_str(), 20, 50, 20, BLUE);
        DrawText(("Uncorrected Variance: " + doubleToString(dispersiaNotIsp)).c_str(), 20, 80, 20, BLUE);
        DrawText(("Data points: " + std::to_string(groupTable.size())).c_str(), 20, 110, 20, BLUE);

        std::vector<Vector2> polygonPoints;
        std::vector<Vector2> empiricalPoints;


        for (auto& [interval, freq] : intervalTable) {
            double midPoint = (interval.first + interval.second) / 2.0;
            float barX = histogramX + (interval.first - minValue) * scaleX;
            float barWidth = (interval.second - interval.first) * scaleX;
            float barHeight = freq * scaleY;

            DrawRectangle(barX, margin + graphHeight - barHeight, barWidth, barHeight, Fade(RED, 0.6f));
            DrawRectangleLines(barX, margin + graphHeight - barHeight, barWidth, barHeight, RED);

            std::string intervalLabel = doubleToString(interval.first) + "-" + doubleToString(interval.second);
            DrawText(intervalLabel.c_str(), barX + barWidth / 2 - MeasureText(intervalLabel.c_str(), 10) / 2,
                margin + graphHeight + 10, 10, BLACK);

            std::string freqLabel = std::to_string(freq);
            DrawText(freqLabel.c_str(), barX + barWidth / 2 - MeasureText(freqLabel.c_str(), 12) / 2,
                margin + graphHeight - barHeight - 20, 12, BLACK);

            Vector2 point = {
                polygonX + (midPoint - minValue) * scaleX,
                margin + graphHeight - freq * scaleY
            };
            polygonPoints.push_back(point);
        }

        for (size_t i = 0; i < empiricalDist.size(); i++) {
            float x = empiricalX + (empiricalDist[i].first - minValue) * scaleX;
            float y = margin + graphHeight - empiricalDist[i].second * empiricalScaleY;

            empiricalPoints.push_back({ x, y });

            DrawCircleV({ x, y }, 3.0f, GREEN);

            if (i > 0) {
                DrawLineEx({ empiricalPoints[i - 1].x, empiricalPoints[i - 1].y },
                    { x, empiricalPoints[i - 1].y }, 2.0f, GREEN);
            }
            if (i < empiricalDist.size() - 1) {
                DrawLineEx({ x, y }, { x, y }, 2.0f, GREEN);
            }
        }

        if (polygonPoints.size() > 1) {
            for (size_t i = 0; i < polygonPoints.size() - 1; i++) {
                DrawLineEx(polygonPoints[i], polygonPoints[i + 1], 2.0f, BLUE);
            }

            for (auto& point : polygonPoints) {
                DrawCircleV(point, 4.0f, BLUE);
                DrawCircleV(point, 2.0f, WHITE);
            }
        }

        for (int i = 0; i <= 5; i++) {
            float yPos = margin + graphHeight - (i * graphHeight / 5.0f);

            DrawLine(histogramX, yPos, histogramX + graphWidth, yPos, Fade(LIGHTGRAY, 0.3f));
            DrawLine(polygonX, yPos, polygonX + graphWidth, yPos, Fade(LIGHTGRAY, 0.3f));
            DrawLine(empiricalX, yPos, empiricalX + graphWidth, yPos, Fade(LIGHTGRAY, 0.3f));

            int freqValue = i * maxFrequency / 5;
            float empiricalValue = i * maxEmpirical / 5;

            std::string freqText = std::to_string(freqValue);
            std::string empiricalText = doubleToString(empiricalValue, 2);

            DrawText(freqText.c_str(), histogramX - 30, yPos - 10, 15, BLACK);
            DrawText(freqText.c_str(), polygonX - 30, yPos - 10, 15, BLACK);
            DrawText(empiricalText.c_str(), empiricalX - 30, yPos - 10, 15, BLACK);
        }

        for (int i = 0; i <= 5; i++) {
            float xPosHist = histogramX + (i * graphWidth / 5.0f);
            float xPosPoly = polygonX + (i * graphWidth / 5.0f);
            float xPosEmp = empiricalX + (i * graphWidth / 5.0f);
            double value = minValue + (i * (maxValue - minValue) / 5.0f);

            DrawLine(xPosHist, margin + graphHeight, xPosHist, margin + graphHeight + 5, BLACK);
            DrawLine(xPosPoly, margin + graphHeight, xPosPoly, margin + graphHeight + 5, BLACK);
            DrawLine(xPosEmp, margin + graphHeight, xPosEmp, margin + graphHeight + 5, BLACK);


            std::string valueText = doubleToString(value, 2);
            DrawText(valueText.c_str(), xPosHist - MeasureText(valueText.c_str(), 10) / 2, margin + graphHeight + 15, 10, BLACK);
            DrawText(valueText.c_str(), xPosPoly - MeasureText(valueText.c_str(), 10) / 2, margin + graphHeight + 15, 10, BLACK);
            DrawText(valueText.c_str(), xPosEmp - MeasureText(valueText.c_str(), 10) / 2, margin + graphHeight + 15, 10, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}