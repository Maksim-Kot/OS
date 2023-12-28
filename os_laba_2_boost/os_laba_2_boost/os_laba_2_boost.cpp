#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

struct Array 
{
    std::vector<double> a;
    double average;
    double min_;
    double max_;

    explicit Array(const std::vector<double>& a_) : a(a_), average(0), min_(0), max_(0) {}
};

void Replace(Array& param) 
{
    for (int i = 0; i < param.a.size(); i++) 
    {
        if (param.a[i] == param.max_ || param.a[i] == param.min_)
            param.a[i] = param.average;
    }
}

void MinMax(Array& param) 
{
    double min_ = param.a[0];
    double max_ = min_;
    for (int i = 1; i < param.a.size(); i++) 
    {
        if (param.a[i] > max_) max_ = param.a[i];
        boost::this_thread::sleep_for(boost::chrono::milliseconds(7));
        if (param.a[i] < min_) min_ = param.a[i];
        boost::this_thread::sleep_for(boost::chrono::milliseconds(7));
    }
    param.max_ = max_;
    param.min_ = min_;
}

void Average(Array& param) 
{
    double sum = 0;
    for (int i = 0; i < param.a.size(); i++) {
        sum += param.a[i];
        boost::this_thread::sleep_for(boost::chrono::milliseconds(12));
    }
    param.average = sum / param.a.size();
}

int main() 
{
    int n;
    std::cout << "Enter count of elements: ";
    std::cin >> n;
    std::vector<double> a(n);
    std::cout << "Enter elements:\n";
    for (int i = 0; i < n; ++i) 
    {
        std::cin >> a[i];
    }
    Array param(a);

    boost::thread tMinMax(MinMax, boost::ref(param));
    boost::thread tAverage(Average, boost::ref(param));

    tMinMax.join();
    tAverage.join();

    std::cout << "New array: ";
    Replace(param);
    for (int i = 0; i < param.a.size(); i++) 
    {
        std::cout << std::setprecision(3) << param.a[i] << " ";
    }
    std::cout << '\n';

    return 0;
}
