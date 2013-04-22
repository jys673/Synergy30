#include "synergy.h"
#include "iolib.h"

void cnf_print_map()
{
    sng_map *link_pt;

    link_pt = sng_map_hd.link_hd;
    fprintf(stderr,
        "\n--------------- LOCAL/GLOBAL NAME MAP --------------------\n");
    fprintf(stderr, "CSL Name: (%s)\n", sng_map_hd.csl_name);
    fprintf(stderr, "Component Name: (%s)\n",sng_map_hd.name);
    fprintf(stderr, "Appid: (%s)\n", sng_map_hd.appid);
    fprintf(stderr, "Protocol: (%s)\n",sng_map_hd.protocol);
    fprintf(stderr, "F (%d) P(%d) T(%d) D(%d) \n",
	sng_map_hd.f, sng_map_hd.p, sng_map_hd.t, sng_map_hd.d);
    fprintf(stderr, "Number of entries: %d\n", sng_map_hd.link_cnt);
    while (link_pt != NULL)
    {
	fprintf(stderr, "%c %c %s %s %s %s\n", 
		link_pt->type,
		link_pt->dirn,
		link_pt->ref_name,
		link_pt->obj_name,
		link_pt->cpu,
		link_pt->phys_name);
	link_pt = link_pt->next;
    }
    fprintf(stderr,
        "\n----------------------------------------------------------\n");

}

