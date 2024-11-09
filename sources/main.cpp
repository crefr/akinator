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
    akinator_t akinator = {};
    akinatorCtor(&akinator, base_file);
    fclose(base_file);

    treeDumpGraph(akinator.root, akinatorPtrToStr);

    akinatorPlay(&akinator);

    treeDumpGraph(akinator.root, akinatorPtrToStr);

    base_file = fopen("base.txt", "w");
    akinatorDumpBaseToFile(&akinator, base_file);
    fclose(base_file);

    akinatorDtor(&akinator);

    logExit();
    return 0;
}
