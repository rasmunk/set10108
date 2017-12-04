// 3.2.2

#include <iostream>
#include <omp.h>
#include <thread>


using namespace std;

int main() {
    auto num_threads = thread::hardware_concurrency();

    const int n = static_cast<int>(pow(2, 30));
    double factor = 0.0;
    double pi = 0.0;

#pragma omp parallel for num_threads(num_threads) reduction(+:pi) private(factor)
    for (int k = 0; k < n; k++) {
        if (k % 2 == 0)
            factor = 1.0;
        else
            factor = -1.0;

        pi += factor / (2.0 * k + 1);
    }
    pi *= 4.0;

    cout.precision(numeric_limits<double>::digits10);
    cout << "pi = " << pi << endl;

    return 0;
}