#include <iostream>
#include <chrono>
#include <functional>

using namespace std;

const int N = 1000;
double a[N][N];
double b[N][N];
double c[N][N];

void fill()
{
    int i, j;

#pragma omp parallel for shared(a, b) private(i, j)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            a[i][j] = (rand() % 100) / 100.0 + 1.0;
            b[i][j] = (rand() % 100) / 100.0 + 1.0;
        }
}

void calcTransposed()
{
    int i, j, k;
    double cc = 0;

#pragma omp parallel for shared(a, b) private(i, j)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            swap(b[i][j], b[j][i]);
        }

#pragma omp parallel for shared(a, b, c) private(i, j, k) schedule(static)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            cc = 0;
            for (k = 0; k < N; ++k)
            {
                cc += a[i][k] * b[j][k];
            }
            c[i][j] = cc;
        }
}

void calc()
{
    int i, j, k;
    double cc = 0;

#pragma omp parallel for shared(a, b, c) private(i, j, k) schedule(static)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            cc = 0;
            for (k = 0; k < N; ++k)
            {
                cc += a[i][k] * b[k][j];
            }
            c[i][j] = cc;
        }
}

auto measure(function<void()> cb)
{
    fill();

    auto start = chrono::steady_clock::now();
    cb();
    auto end = chrono::steady_clock::now();

    chrono::duration<double> elapsedSeconds = end - start;

    return elapsedSeconds.count();
}

int main()
{
    auto with = measure(calcTransposed);
    auto without = measure(calc);

    cout << "Using CPU cache: " << with << endl;
    cout << "Without CPU cache: " << without << endl;

    cout << "Using CPU cache reciprocal: " << 1000 / with << endl;
    cout << "Without CPU cache reciprocal: " << 1000 / without << endl;

    cout << "Relation: " << without / with << endl;

    return 0;
}
