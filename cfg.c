#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "cfg.h"
#include "util.h"

void config_init(config_t *cfg)
{
    // set default values
    cfg->mode = MODE_DEFAULT;
    cfg->implementation = IMPLEMENTATION_DEFAULT;
    cfg->degree = DEGREE_DEFAULT;
    cfg->should_benchmark = BENCHMARK_DEFAULT;
    cfg->benchmark_iterations = BENCHMARK_ITERATIONS_DEFAULT;
    cfg->save_svg = SVG_DEFAULT;
    cfg->svg_filename = SVG_FILENAME_DEFAULT;
    cfg->num_threads = THREADS_DEFAULT;
    cfg->path = NULL;
    cfg->index = INDEX_DEFAULT;
    cfg->x = X_DEFAULT;
    cfg->y = Y_DEFAULT;
}

int config_parse(int argc, char **argv, config_t *cfg)
{
    static struct option long_options[] = {
        {"V", optional_argument, 0, 'V'},
        {"B", optional_argument, 0, 'B'},
        {"d", required_argument, 0, 'd'},
        {"p", no_argument, 0, 'p'},
        {"i", required_argument, 0, 'i'},
        {"s", optional_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}};

    cfg->path = argv[0];

    const char *program_name = get_filename(argv[0]);

    if (argc == 1)
    {
        fprintf(stderr, "%s: missing arguments.\n", program_name);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        return EXIT_FAILURE;
    }

    int c;
    while ((c = getopt_long(argc, argv, "V::B::d:pi:t:s::h", long_options, 0)) != -1)
    {
        switch (c)
        {
        case 'V':

            if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-')
            {
                optarg = argv[optind++];
            }

            if (optarg != NULL)
            {
                if (!is_number(optarg))
                {
                    fprintf(stderr, "%s: argument for option -- '%c' is invalid: implementation must be a number\n", program_name, c);
                    return EXIT_FAILURE;
                }

                cfg->implementation = strtoul(optarg, 0, 10);
            }
            else
            {
                cfg->mode = VERSION_HELP;
                return EXIT_SUCCESS;
            }
            break;
        case 'B':
            cfg->should_benchmark = true;

            if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-')
            {
                optarg = argv[optind++];
            }

            if (optarg != NULL)
            {
                if (!is_number(optarg))
                {
                    fprintf(stderr, "%s: argument for option -- '%c' is invalid: benchmark repetitions must be a number\n", program_name, c);
                    return EXIT_FAILURE;
                }

                cfg->benchmark_iterations = strtoul(optarg, 0, 10);
                if (cfg->benchmark_iterations < 1 || cfg->benchmark_iterations > BENCHMARK_ITERATIONS_MAX)
                {
                    fprintf(stderr, "%s: argument for option -- '%c' is invalid: benchmark repetitions must be a number between 1 and %u\n", program_name, c, BENCHMARK_ITERATIONS_MAX);
                    return EXIT_FAILURE;
                }
            }
            break;
        case 'd':
            if (!is_number(optarg))
            {
                fprintf(stderr, "%s: argument for option -- '%c' is invalid: degree must be a number\n", program_name, c);
                return EXIT_FAILURE;
            }

            cfg->degree = strtoul(optarg, 0, 10);
            if (!cfg->degree)
            {
                fprintf(stderr, "%s: argument for option -- '%c' is invalid: degree must be a number larger than 1\n", program_name, c);
                return EXIT_FAILURE;
            }
            break;
        case 'p':
            if (cfg->mode != STANDARD)
            {
                fprintf(stderr, "%s: option -- '%c' is invalid: cannot use -i and -p at the same time\n", program_name, c);
                return EXIT_FAILURE;
            }
            cfg->mode = POSITION;
            break;
        case 'i':
            if (cfg->mode != STANDARD)
            {
                fprintf(stderr, "%s: option -- '%c' is invalid: cannot use -i and -p at the same time\n", program_name, c);
                return EXIT_FAILURE;
            }

            if (!is_number(optarg))
            {
                fprintf(stderr, "%s: argument for option -- '%c' is invalid: index must be a number\n", program_name, c);
                return EXIT_FAILURE;
            }

            unsigned long idx = strtoul(optarg, 0, 10);

            if (idx > INDEX_MAX)
            {
                fprintf(stderr, "%s: argument for option -- '%c' is invalid: index must be a number smaller than or equal to %llu\n", program_name, c, INDEX_MAX);
                return EXIT_FAILURE;
            }

            cfg->mode = INDEX;
            cfg->index = idx;
            break;
        case 't':
            if (!is_number(optarg))
            {
                fprintf(stderr, "%s: argument for option -- '%c' is invalid: threads must be a number\n", program_name, c);
                return EXIT_FAILURE;
            }

            cfg->num_threads = strtoul(optarg, 0, 10);
            if (cfg->num_threads < 1 || cfg->num_threads > THREADS_MAX)
            {
                fprintf(stderr, "%s: argument for option -- '%c' is invalid: threads must be between %u and %u\n", program_name, c, THREADS_MIN, THREADS_MAX);
                return EXIT_FAILURE;
            }
            break;
        case 's':
            cfg->save_svg = 1;

            if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-')
            {
                optarg = argv[optind++];
            }

            if (optarg != NULL)
            {
                if (strlen(optarg) > SVG_FILENAME_MAX_LENGTH)
                {
                    fprintf(stderr, "%s: argument for option -- '%c' is invalid: filename is too large\n", program_name, c);
                    return EXIT_FAILURE;
                }

                for (size_t i = 0; i < strlen(optarg); i++)
                {
                    if (optarg[i] == '/')
                    {
                        fprintf(stderr, "%s: argument for option -- '%c' is invalid: '%s' (char %zu)\n", program_name, c, optarg, i + 1);
                        return EXIT_FAILURE;
                    }
                }

                cfg->svg_filename = optarg;
            }
            break;
        case 'h':
            cfg->mode = HELP;
            return EXIT_SUCCESS;
        default:
            printf("Try '%s --help' for more information.\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (cfg->degree == 0)
    {
        fprintf(stderr, "%s: required option -- 'd' is missing\n", program_name);
        return EXIT_FAILURE;
    }

    if (cfg->mode == STANDARD)
    {
        if (cfg->implementation >= MAX_IMPL)
        {
            fprintf(stderr, "%s: argument for option -- '%c' is invalid: implementation must be a number between 0 and %u\n", program_name, c, MAX_IMPL - 1);
            return EXIT_FAILURE;
        }

        if (cfg->degree > DEGREE_MAX)
        {
            fprintf(stderr, "%s: argument for option -- 'd' is invalid: degree must be a number between 1 and %u\n", program_name, DEGREE_MAX);
            return EXIT_FAILURE;
        }
    }

    if (cfg->mode == INDEX)
    {
        if (cfg->implementation >= INDEX_MAX_IMPL)
        {
            fprintf(stderr, "%s: argument for option -- 'i' is invalid: implementation must be a number between 0 and %u\n", program_name, INDEX_MAX_IMPL - 1);
            return EXIT_FAILURE;
        }
    }

    if (cfg->mode == POSITION)
    {
        if (cfg->implementation >= POSITION_MAX_IMPL)
        {
            fprintf(stderr, "%s: argument for option -- 'p' is invalid: implementation must be a number between 0 and %u\n", program_name, POSITION_MAX_IMPL - 1);
            return EXIT_FAILURE;
        }

        if (optind + 2 > argc)
        {
            fprintf(stderr, "%s: required positional arguments x and y for option -- 'p' are missing\n", program_name);
            return EXIT_FAILURE;
        }

        if (!is_number(argv[optind]) || !is_number(argv[optind + 1]))
        {
            fprintf(stderr, "%s: positional arguments x and y for option -- 'p' are invalid: must be numbers\n", program_name);
            return EXIT_FAILURE;
        }

        unsigned long x = strtoul(argv[optind], 0, 10);
        unsigned long y = strtoul(argv[optind + 1], 0, 10);

        if (x > COORD_MAX)
        {
            fprintf(stderr, "%s: positional argument x for option -- 'p' is invalid: must be a number smaller than or equal to %llu\n", program_name, COORD_MAX);
            return EXIT_FAILURE;
        }

        if (y > COORD_MAX)
        {
            fprintf(stderr, "%s: positional argument y for option -- 'p' is invalid: must be a number smaller than or equal to %llu\n", program_name, COORD_MAX);
            return EXIT_FAILURE;
        }

        cfg->x = x;
        cfg->y = y;
    }

    return EXIT_SUCCESS;
}
