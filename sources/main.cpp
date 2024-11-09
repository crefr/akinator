#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "akinator.h"
#include "logger.h"

int main()
{
    system("mkdir -p logs/");
    logStart("logs/log.html", LOG_DEBUG_PLUS, LOG_HTML);
    logCancelBuffer();

    FILE * base_file = fopen("base.txt", "r");
    FILE * base_file_new = fopen("base_new.txt", "w");
    assert(base_file);

    akinator_t akinator = {};
    akinatorCtor(&akinator, base_file);

    treeDumpGraph(akinator.root, akinatorPtrToStr);

    akinatorPlay(&akinator);

    treeDumpGraph(akinator.root, akinatorPtrToStr);

    akinatorDumpBaseToFile(&akinator, base_file_new);

    akinatorDtor(&akinator);

    fclose(base_file);
    logExit();
    return 0;
}
