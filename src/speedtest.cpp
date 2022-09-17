#include <iostream>
#include <chrono>
#include <functional>

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
            std::swap(b[i][j], b[j][i]);
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

void calcTransposedSingleCore()
{
    int i, j, k;
    double cc = 0;

    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            std::swap(b[i][j], b[j][i]);
        }

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

auto measure(std::function<void()> cb)
{
    fill();

    auto start = std::chrono::steady_clock::now();
    cb();
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsedSeconds = end - start;

    return elapsedSeconds.count();
}

int main()
{
    auto with = measure(calcTransposed);
    auto without = measure(calc);
    auto singleCore = measure(calcTransposedSingleCore);

    std::cout << "Using CPU cache: " << with << "s / " << 1000 / with << std::endl;
    std::cout << "Without CPU cache: " << without << "s / " << 1000 / without << std::endl;
    std::cout << "Relation without / with cache: " << without / with << std::endl;
    std::cout << "Single Core: " << singleCore << "s / " << 1000 / singleCore << std::endl;

    return 0;
}
