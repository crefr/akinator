#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "akinator.h"
#include "logger.h"

const size_t STR_BUFFER_LEN = 64;

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    wprintf(L"говно этот ваш wchar_t\n");

    mkdir("logs", S_IFDIR);
    logStart("logs/log.html", LOG_DEBUG_PLUS, LOG_HTML);
    //logCancelBuffer();

    FILE * base_file = fopen("base.txt", "r");
    akinator_t akinator = {};
    akinatorCtor(&akinator, base_file);
    fclose(base_file);

    treeSetDumpMode(DUMP_MEDIUM);
    akinatorDump(&akinator);

    akinator_mode_t launch_mode = akinatorGetMode(&akinator);
    akinatorLaunch(&akinator, launch_mode);

    akinatorDump(&akinator);

    base_file = fopen("base.txt", "w");
    akinatorWriteBaseToFile(&akinator, base_file);
    fclose(base_file);

    akinatorDtor(&akinator);

    logExit();
    return 0;
}
