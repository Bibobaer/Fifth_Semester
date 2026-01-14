#pragma once

#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <string>
#include <functional>
#include <format>

#include "Pruffer.h"
#include "xlsxwriter.h"

std::vector<int> GenerateCode(int n) {
    std::vector<int> code(n - 2);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution dist(0, n - 1);

    std::generate(code.begin(), code.end(), [&]() {return dist(gen); });
    return code;
}

Pruffer::adjacency_list GenerateTree(int n) {
    return Pruffer::Decoder::DecodePrufferPQ(GenerateCode(n));
}


double measure_time(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void AddChart(lxw_chart* chart, std::string sheet_name, int& end_col) {
    chart_axis_set_name(chart->x_axis, "Tree size (n)");
    chart_axis_set_name(chart->y_axis, "Time (ms)");

    char sym = 'A' + end_col;
    auto ser1 = chart_add_series(chart,
        std::format("={}!$B$1:${}$1", sheet_name, sym).c_str(), // X-ось
        std::format("={}!$B$2:${}$2", sheet_name, sym).c_str()); // Y-ось: оптимизированный

    chart_series_set_name(ser1, "Optimal");

    auto ser2 = chart_add_series(chart,
        std::format("={}!$B$1:${}$1", sheet_name, sym).c_str(), // X-ось
        std::format("={}!$B$3:${}$3", sheet_name, sym).c_str()); // Y-ось: оптимизированный

    chart_series_set_name(ser2, "Optimized");
}

void TestPruffer() {
    auto vertexes = {1000, 10000, 100000, 1000000, 10000000 };

    lxw_workbook* workbook = workbook_new("test_result.xlsx");
    lxw_worksheet* encode_sheet = workbook_add_worksheet(workbook, "Encode Times");
    lxw_worksheet* decode_sheet = workbook_add_worksheet(workbook, "Decode Times");

    worksheet_write_string(encode_sheet, 0, 0, "n", NULL);
    worksheet_write_string(encode_sheet, 1, 0, "Optimal algorithm", NULL);
    worksheet_write_string(encode_sheet, 2, 0, "Optimized algorithm", NULL);

    worksheet_write_string(decode_sheet, 0, 0, "n", NULL);
    worksheet_write_string(decode_sheet, 1, 0, "Optimal algorithm", NULL);
    worksheet_write_string(decode_sheet, 2, 0, "Optimized algorithm", NULL);

    int col = 1;
    for (auto n : vertexes) {
        std::cout << "Testing n = " << n << "\n";

        auto tree = GenerateTree(n);

        std::cout << "Encoding opt\n";
        double time_optimal = measure_time([&](){
            auto p = std::bind(Pruffer::Encoder::EncodePruffer(tree));
            (void)p;
        });

        std::cout << "Encoding PQ\n";
        double time_PQ = measure_time([&](){
            auto p = std::bind(Pruffer::Encoder::EncodePrufferPQ(tree));
            (void)p;
        });

        worksheet_write_number(encode_sheet, 0, col, n, NULL);
        worksheet_write_number(encode_sheet, 1, col, time_optimal, NULL);
        worksheet_write_number(encode_sheet, 2, col, time_PQ, NULL);
        // ---------------------------------------------------------------
        auto code = GenerateCode(n);

        std::cout << "Decoding opt\n";
        time_optimal = measure_time([&]() {
            auto p = std::bind(Pruffer::Decoder::DecodePruffer(code));
            (void)p;
            });

        std::cout << "Decoding PQ\n";
        time_PQ = measure_time([&]() {
            auto p = std::bind(Pruffer::Decoder::DecodePrufferPQ(code));
            (void)p;
            });

        worksheet_write_number(decode_sheet, 0, col, n, NULL);
        worksheet_write_number(decode_sheet, 1, col, time_optimal, NULL);
        worksheet_write_number(decode_sheet, 2, col, time_PQ, NULL);

        col++;
    }

    lxw_chart* encode_chart = workbook_add_chart(workbook, LXW_CHART_LINE);
    chart_title_set_name(encode_chart, "Grafic of speed Encode Pruffer");

    AddChart(encode_chart, "Encode Times", col);
    worksheet_insert_chart(encode_sheet, 5, 1, encode_chart);

    lxw_chart* decode_chart = workbook_add_chart(workbook, LXW_CHART_LINE);
    chart_title_set_name(decode_chart, "Grafic of speed Decode Pruffer");

    AddChart(decode_chart, "Decode Times", col);
    worksheet_insert_chart(decode_sheet, 5, 1, decode_chart);

    workbook_close(workbook);
}