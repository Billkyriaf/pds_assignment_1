#include <omp.h>
#include <stdio.h>

#define THREADS 8



int main() {
    static int num_steps = 1000000000;
    double pi, sum = 0.0;
    double step = 1.0 / (double) num_steps;

    omp_set_num_threads(THREADS);

    double start = omp_get_wtime();
    #pragma omp parallel default(none) shared(num_steps, sum, step)
    {
        #pragma omp for schedule(static, 100) reduction(+: sum)
        for (int i = 0; i < num_steps; ++i) {
            double x = (i + 0.5) * step;
            sum += +4.0 / (1.0 + x * x);
        }
    }

    pi = step * sum;
    double stop = omp_get_wtime();

    printf("pi= %.10f, time: %f", pi, stop - start);
    return 0;
}