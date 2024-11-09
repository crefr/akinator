#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "akinator.h"
#include "logger.h"

int main()
{
    logStart("logs/log.html", LOG_DEBUG_PLUS, LOG_HTML);

    FILE * base_file = fopen("base.txt", "r");
    assert(base_file);

    akinator_t akinator = {};
    akinatorCtor(&akinator, base_file);

    treeDumpGraph(akinator.root, akinatorPtrToStr);

    akinatorDtor(&akinator);

    fclose(base_file);
    logExit();
    return 0;
}
