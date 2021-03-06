#include <iostream>
#include <chrono>
#define OPT 1

using namespace std;

const int N = 1000;
double a[N][N];
double b[N][N];
double c[N][N];

int main()
{
    int i, j, k;
    double cc = 0;

#pragma omp parallel for shared(a, b) private(i, j)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            a[i][j] = (rand() % 100) / 100.0 + 1.0;
            b[i][j] = (rand() % 100) / 100.0 + 1.0;
        }

    auto start = chrono::steady_clock::now();

#ifdef OPT
#pragma omp parallel for shared(a, b) private(i, j)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            swap(b[i][j], b[j][i]);
        }
#endif

#pragma omp parallel for shared(a, b, c) private(i, j, k) schedule(static)
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j)
        {
            cc = 0;
            for (k = 0; k < N; ++k)
#ifdef OPT
                cc += a[i][k] * b[j][k];
#else
                cc += a[i][k] * b[k][j];
#endif
            c[i][j] = cc;
        }

    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsedSeconds = end - start;

    cout << elapsedSeconds.count() << endl;

    return 0;
}
