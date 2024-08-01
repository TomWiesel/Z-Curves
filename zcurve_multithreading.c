#include "zcurve_multithreading.h"

void *z_curve_thread(void *arg)
{
    // cast the argument to the thread data
    thread_data_t *data = (thread_data_t *)arg;

    for (size_t i = data->start; i < data->end; ++i)
    {
        data->x[i] = 0;
        data->y[i] = 0;

        for (unsigned j = 0; j < data->degree; ++j)
        {
            data->x[i] |= ((i >> (j * 2)) & 1ull) << j;
            data->y[i] |= ((i >> (j * 2 + 1)) & 1ull) << j;
        }
    }

    return NULL;
}

int z_curve_multithreaded(unsigned degree, coord_t *x, coord_t *y, unsigned num_threads)
{
    size_t max = 1ull << (degree * 2);

    if (num_threads > max)
    {
        // who needs more threads than points?
        num_threads = max;
    }

    size_t left_over = max % num_threads;
    size_t points_per_thread = max / num_threads;

    pthread_t thread[num_threads];
    thread_data_t *thread_data = malloc(sizeof(thread_data_t) * num_threads);

    if (thread_data == NULL)
    {
        return -1;
    }

    for (unsigned i = 0; i < num_threads; ++i)
    {
        thread_data_t *data = &thread_data[i];

        data->thread_id = i;
        data->degree = degree;
        data->x = x;
        data->y = y;

        data->start = i * points_per_thread;
        data->end = data->start + points_per_thread;

        // if its the last thread, give it the left over points
        if (i == num_threads - 1)
        {
            data->end += left_over;
        }

        int result = pthread_create(&thread[i], NULL, z_curve_thread, (void *)data);

        // if the thread cant be created, cancel all threads and return
        if (result != 0)
        {
            for (unsigned j = 0; j < i; ++j)
            {
                pthread_cancel(thread[j]);
            }

            for (unsigned j = 0; j < i; ++j)
            {
                pthread_join(thread[j], NULL);
            }

            free(thread_data);
            return -1;
        }
    }

    // wait for all threads to finish
    for (unsigned i = 0; i < num_threads; ++i)
    {
        int result = pthread_join(thread[i], NULL);
        if (result != 0)
        {
            for (unsigned j = 0; j < num_threads; ++j)
            {
                pthread_cancel(thread[j]);
            }

            for (unsigned j = 0; j < num_threads; ++j)
            {
                pthread_join(thread[j], NULL);
            }

            free(thread_data);
            return -1;
        }
    }

    // all threads finished, free the thread data
    free(thread_data);

    return 0;
}
