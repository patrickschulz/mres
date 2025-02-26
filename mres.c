#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void engineering_notation(double number, double* new_num, const char** prefix)
{
    if(number == 0)
    {
        *new_num = 0;
        *prefix = "";
        return;
    }
    const char* powertable[] = {
        "y", /* yocto       */
        "z", /* zepta       */
        "a", /* atto        */
        "f", /* femto       */
        "p", /* pico        */
        "n", /* nano        */
        "u", /* micro       */
        "m", /* milli       */
        "",  /* - none -    */
        "k", /* kilo        */
        "M", /* Mega        */
        "G", /* Giga        */
        "T", /* Tera        */
        "P", /* Peta        */
        "E", /* Exa         */
        "Z", /* Zetta       */
        "Y", /* Yotta       */
    };
    int power = floor(log10(fabs(number)) / 3);
    *new_num = number / (exp(3 * power * log(10)));
    *prefix = powertable[power + 8]; /* 8: center offset of powertable */
}

struct metal {
    const char* name;
    double width;
    double resistance;
};

struct via {
    const char* name;
    double resistance;
};

#include "metal_via_data.c"

int main(int argc, char** argv)
{
    size_t i;
    if((argc > 1) && ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)))
    {
        puts("mres [-h | --help] [-p | --print] [-u | --unicode] material [width/xrep] [length/yrep]");
        puts("     (the options, if present, need to be given before the material and geometry parameters)");
        putchar('\n');
        puts("metals:");
        puts("      width is the width in nanometer (default minimum width)");
        puts("      length is the width in micrometer (default 1)");
        putchar('\n');
        puts("vias:");
        puts("      xrep is the number of vias in x direction (default 1)");
        puts("      yrep is the number of vias in y direction (default 1)");
        putchar('\n');
        puts("execute mres without any arguments to get list of materials");
        return 0;
    }

    if(argc == 1)
    {
        puts("you need to specify a material");
        puts("possible metals are:");
        putchar(' ');
        for(i = 0; i < sizeof(metals) / sizeof(metals[0]); ++i)
        {
            printf(" %s", metals[i].name);
        }
        putchar('\n');
        puts("possible vias are:");
        putchar(' ');
        for(i = 0; i < sizeof(vias) / sizeof(vias[0]); ++i)
        {
            printf(" %s", vias[i].name);
        }
        putchar('\n');
        return 0;
    }

    /* print and unicode mode */
    int print = 0;
    int use_unicode = 0;
    int a;
    for(a = 1; a < argc; ++a)
    {
        if((strcmp(argv[a], "-p") == 0) || (strcmp(argv[a], "--print") == 0))
        {
            print = 1;
        }
        if((strcmp(argv[a], "-u") == 0) || (strcmp(argv[a], "--unicode") == 0))
        {
            use_unicode = 1;
        }
    }

    /* find material argument index, skip options */
    size_t arg_material = 1;
    for(a = 1; a < argc; ++a)
    {
        if(argv[a][0] != '-')
        {
            arg_material = a;
            break;
        }
    }

    /* metal/via index */
    int metal_index = -1;
    int via_index = -1;
    for(i = 0; i < sizeof(metals) / sizeof(metals[0]); ++i)
    {
        if(strcmp(argv[arg_material], metals[i].name) == 0)
        {
            metal_index = i;
        }
    }
    for(i = 0; i < sizeof(vias) / sizeof(vias[0]); ++i)
    {
        if(strcmp(argv[arg_material], vias[i].name) == 0)
        {
            via_index = i;
        }
    }

    /* check if material is known */
    if((metal_index == -1) && (via_index == -1))
    {
        printf("material '%s' is unknown\n", argv[1]);
        return 1;
    }

    /* metal mode */
    if(metal_index >= 0)
    {
        double width;
        if(argc > arg_material + 1)
        {
            width = atof(argv[arg_material + 1]);
        }
        else
        {
            width = metals[metal_index].width;
        }

        double length;
        if(argc > arg_material + 2)
        {
            length = atof(argv[arg_material + 2]);
        }
        else
        {
            length = 1.0;
        }

        double resistance = metals[metal_index].resistance * length * metals[metal_index].width / width;
        double r;
        const char* rp;
        engineering_notation(resistance, &r, &rp);
        double w;
        const char* wp;
        engineering_notation(width * 1e-9, &w, &wp);
        double l;
        const char* lp;
        engineering_notation(length * 1e-6, &l, &lp);
        if(print)
        {
            if(use_unicode)
            {
                const char* str = 
                "              %.1f %sm\n"
                "        ⮜─────────────────⮞\n"
                "        ┌──────────────────┐\n"
                "        │       ⮝          │\n"
                "    %s  │       │ %5.1f %sm │ = %.1f %sΩ\n"
                "        │       ⮟          │\n"
                "        └──────────────────┘\n"
                ;
                printf(str, l, lp, metals[metal_index].name, w, wp, r, rp);
            }
            else
            {
                const char* str = 
                "              %.1f %sm\n"
                "        <----------------->\n"
                "        +------------------+\n"
                "        |       ^          |\n"
                "    %s  |       | %5.1f %sm | = %.1f %sOhm\n"
                "        |       v          |\n"
                "        +------------------+\n"
                ;
                printf(str, l, lp, metals[metal_index].name, w, wp, r, rp);
            }
        }
        else
        {
            printf("%s (%.1f %sm / %.1f%sm) = %.1f %1sOhm\n", metals[metal_index].name, w, wp, l, lp, r, rp);
        }
    }
    else /* via mode */
    {
        unsigned int xrep;
        if(argc > arg_material + 1)
        {
            xrep = atoi(argv[arg_material + 1]);
        }
        else
        {
            xrep = 1;
        }

        unsigned int yrep;
        if(argc > arg_material + 2)
        {
            yrep = atoi(argv[arg_material + 2]);
        }
        else
        {
            yrep = 1;
        }

        double resistance = vias[via_index].resistance / xrep / yrep;
        double r;
        const char* rp;
        engineering_notation(resistance, &r, &rp);
        if(print)
        {
            if(use_unicode)
            {
                const char* str =
                    "             x %d\n"
                    "         ┌──┐  ┌──┐\n"
                    "         │  │  │  │\n"
                    "         └──┘  └──┘ \n"
                    "    x %d               = %.1f %sΩ\n"
                    "         ┌──┐  ┌──┐\n"
                    "         │  │  │  │\n"
                    "         └──┘  └──┘\n"
                    ;
                printf(str, xrep, yrep, r, rp);
            }
            else
            {
                const char* str =
                    "             x %d\n"
                    "         +--+  +--+\n"
                    "         |  |  |  |\n"
                    "         +--+  +--+ \n"
                    "    x %d               = %.1f %sOhm\n"
                    "         +--+  +--+\n"
                    "         |  |  |  |\n"
                    "         +--+  +--+\n"
                    ;
                printf(str, xrep, yrep, r, rp);
            }
        }
        else
        {
            if(use_unicode)
            {
                printf("%s (%d x %d) = %.1f %sΩ\n", vias[via_index].name, xrep, yrep, r, rp);
            }
            else
            {
                printf("%s (%d x %d) = %.1f %sOhm\n", vias[via_index].name, xrep, yrep, r, rp);
            }
        }
    }
    return 0;
}
