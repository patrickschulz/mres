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
        "y", // yocto
        "z", // zepta
        "a", // atto
        "f", // femto
        "p", // pico
        "n", // nano
        "u", // micro
        "m", // milli
        "",  // - none -
        "k", // kilo
        "M", // Mega
        "G", // Giga
        "T", // Tera
        "P", // Peta
        "E", // Exa
        "Z", // Zetta
        "Y", // Yotta
    };
    int power = floor(log10(fabs(number)) / 3);
    *new_num = number / (exp(3 * power * log(10)));
    *prefix = powertable[power + 8]; // 8: center offset of powertable
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
    //local engineering = require "engineering"
    if((argc > 1) && ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)))
    {
        puts("mres material [width/xrep] [length/yrep]");
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
        for(size_t i = 0; i < sizeof(metals) / sizeof(metals[0]); ++i)
        {
            printf(" %s", metals[i].name);
        }
        putchar('\n');
        puts("possible vias are:");
        putchar(' ');
        for(size_t i = 0; i < sizeof(vias) / sizeof(vias[0]); ++i)
        {
            printf(" %s", vias[i].name);
        }
        putchar('\n');
        return 0;
    }

    // is material known?
    int known_material = 0;
    for(size_t i = 0; i < sizeof(metals) / sizeof(metals[0]); ++i)
    {
        if(strcmp(argv[1], metals[i].name) == 0)
        {
            known_material = 1;
        }
    }
    for(size_t i = 0; i < sizeof(vias) / sizeof(vias[0]); ++i)
    {
        if(strcmp(argv[1], vias[i].name) == 0)
        {
            known_material = 1;
        }
    }

    if(!known_material)
    {
        printf("material '%s' is unknown\n", argv[1]);
        return 1;
    }

    // metal/via index
    int metal_index = -1;
    int via_index = -1;
    for(size_t i = 0; i < sizeof(metals) / sizeof(metals[0]); ++i)
    {
        if(strcmp(argv[1], metals[i].name) == 0)
        {
            metal_index = i;
        }
    }
    for(size_t i = 0; i < sizeof(vias) / sizeof(vias[0]); ++i)
    {
        if(strcmp(argv[1], vias[i].name) == 0)
        {
            via_index = 1;
        }
    }

    if(metal_index >= 0)
    {
        double width;
        if(argc > 2)
        {
            width = atof(argv[2]);
        }
        else
        {
            width = metals[metal_index].width;
        }

        double length;
        if(argc > 3)
        {
            length = atof(argv[3]);
        }
        else
        {
            length = 1.0;
        }

        const char* str = 
        "              %.1f %sm\n"
        "        ⮜─────────────────⮞\n"
        "        ┌──────────────────┐\n"
        "        │       ⮝          │\n"
        "    %s  │       │ %5.1f %sm │ = %.1f %sΩ\n"
        "        │       ⮟          │\n"
        "        └──────────────────┘\n"
        ;
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
        printf(str, l, lp, metals[metal_index].name, w, wp, r, rp);
    }
    else // via
    {
        unsigned int xrep;
        if(argc > 2)
        {
            xrep = atoi(argv[2]);
        }
        else
        {
            xrep = 1;
        }

        unsigned int yrep;
        if(argc > 3)
        {
            yrep = atoi(argv[3]);
        }
        else
        {
            yrep = 1;
        }

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
        double resistance = vias[via_index].resistance / xrep / yrep;
        double r;
        const char* rp;
        engineering_notation(resistance, &r, &rp);
        printf(str, xrep, yrep, r, rp);
    }
}
