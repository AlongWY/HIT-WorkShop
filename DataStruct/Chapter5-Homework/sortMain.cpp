//
// Created by along on 17-12-18.
//

/** Qt */
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGridLayout>
#include <QScreen>

/** 标准库 */
#include <random>
#include <iostream>
#include <sys/time.h>

/** Sort */
#include "Sort.h"

using namespace std;
QT_CHARTS_USE_NAMESPACE

template<typename Comparable>
void printVector(vector<Comparable> &data) {
    for (const auto &x:data)
        cout << x << " ";
    cout << endl;
}

#define TIMER_CREATE struct timeval _tpstart{}, _tpend{};
#define TIMER_RUN(timeuse, func) \
    gettimeofday(&_tpstart, nullptr);\
    func;\
    gettimeofday(&_tpend, nullptr);\
    timeuse = 1000000 * (_tpend.tv_sec - _tpstart.tv_sec) + _tpend.tv_usec - _tpstart.tv_usec;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto *bubbleSeries = new QSplineSeries();
    auto *bubbleSeriesU = new QSplineSeries();
    auto *bubbleSeriesD = new QSplineSeries();
    auto *quickSeries = new QSplineSeries();
    auto *quickSeriesU = new QSplineSeries();
    auto *quickSeriesD = new QSplineSeries();
    auto *search = new QSplineSeries();
    auto *searchR = new QSplineSeries();

    double timeuse;
    vector<int>
        VecSize{10, 100, 500, 1000, 2500, 5000, 7500, 10000, 15000, 20000, 25000, 30000, 35000, 40000, 45000, 50000};
    vector<unsigned long> vec;

    TIMER_CREATE;
    std::random_device rd;
    std::mt19937 mt(rd());
    cout << "冒泡排序" << endl;
    for (auto &size:VecSize) {
        for (unsigned long i = 0; i != size; ++i)
            vec.push_back(mt());
        TIMER_RUN(timeuse, BubbleSort(vec));
        bubbleSeries->append(vec.size(), timeuse);
        cout << "Size:" << size << "Time:" << timeuse << "us" << endl;
        vec.clear();
    }

    for (auto &size:VecSize) {
        for (unsigned long i = 0; i != size; ++i)
            vec.push_back(i);
        TIMER_RUN(timeuse, BubbleSort(vec));
        bubbleSeriesU->append(vec.size(), timeuse);
        cout << "Size:" << size << "Time:" << timeuse << "us" << endl;
        vec.clear();
    }

    for (auto &size:VecSize) {
        for (unsigned long i = 0; i != size; ++i)
            vec.push_back(size - i);
        TIMER_RUN(timeuse, BubbleSort(vec));
        bubbleSeriesD->append(vec.size(), timeuse);
        cout << "Size:" << size << "Time:" << timeuse << "us" << endl;
        vec.clear();
    }

    cout << "快速排序" << endl;
    for (auto &size:VecSize) {
        for (unsigned long i = 0; i != size; ++i)
            vec.push_back(mt());
        TIMER_RUN(timeuse, QuickSort(vec));
        quickSeries->append(vec.size(), timeuse);
        cout << "Size:" << size << "Time:" << timeuse << "us" << endl;
        vec.clear();
    }

    int res;
    for (auto &size:VecSize) {
        for (unsigned long i = 0; i != size; ++i)
            vec.push_back(i);
        TIMER_RUN(timeuse, QuickSort(vec));
        quickSeriesU->append(vec.size(), timeuse);
        cout << "Size:" << size << "Time:" << timeuse << "us" << endl;
        TIMER_RUN(timeuse, BinSearch(vec, 0, size - 1, mt()));
        search->append(size, timeuse);
        TIMER_RUN(timeuse, BinSearchR(vec, 0, size - 1, mt()));
        searchR->append(size, timeuse);
        vec.clear();
    }

    for (auto &size:VecSize) {
        for (unsigned long i = 0; i != size; ++i)
            vec.push_back(size - i);
        TIMER_RUN(timeuse, QuickSort(vec));
        quickSeriesD->append(vec.size(), timeuse);
        cout << "Size:" << size << "Time:" << timeuse << "us" << endl;
        vec.clear();
    }

    QChart *bubbleChart = new QChart();
    QChart *quickChart = new QChart();
    QChart *searchChart = new QChart();

    bubbleChart->legend()->hide();
    bubbleChart->addSeries(bubbleSeries);
    bubbleChart->addSeries(bubbleSeriesU);
    bubbleChart->addSeries(bubbleSeriesD);
    bubbleChart->createDefaultAxes();
    bubbleChart->setTitle("冒泡排序时间");

    quickChart->legend()->hide();
    quickChart->addSeries(quickSeries);
    quickChart->addSeries(quickSeriesU);
    quickChart->addSeries(quickSeriesD);
    quickChart->createDefaultAxes();
    quickChart->setTitle("快速排序时间");

    searchChart->legend()->hide();
    searchChart->addSeries(search);
    searchChart->addSeries(searchR);
    searchChart->createDefaultAxes();
    searchChart->setTitle("二分查找时间");

    auto *bubbleChartView = new QChartView(bubbleChart);
    bubbleChartView->setRenderHint(QPainter::Antialiasing);
    auto *quickChartView = new QChartView(quickChart);
    quickChartView->setRenderHint(QPainter::Antialiasing);
    auto *searchChartView = new QChartView(searchChart);
    searchChartView->setRenderHint(QPainter::Antialiasing);

    auto *layout = new QHBoxLayout;
    layout->addWidget(bubbleChartView);
    layout->addWidget(quickChartView);
    layout->addWidget(searchChartView);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    QMainWindow window;
    window.setCentralWidget(widget);
    window.resize(800, 600);
    window.show();

    return a.exec();
}
