#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>

#include "akinator.h"
#include "logger.h"

const size_t STR_BUFFER_LEN = 64;

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    wprintf(L"говно этот ваш wchar_t\n");

    system("mkdir -p logs/");
    logStart("logs/log.html", LOG_DEBUG_PLUS, LOG_HTML);
    logCancelBuffer();

    FILE * base_file = fopen("base.txt", "r");
    akinator_t akinator = {};
    akinatorCtor(&akinator, base_file);
    fclose(base_file);

    treeDumpGraphWcs(akinator.root, akinatorPtrToStr);

    akinator_mode_t launch_mode = akinatorGetMode(&akinator);
    switch (launch_mode){
        case PLAY_MODE:
            akinatorPlay(&akinator);
            break;
        case DEFINITION_MODE:
            akinatorGiveDefinition(&akinator, L"овчос");
            break;
        default:
            wprintf(L"не поддерживаю такой режим / его не существует\n");
            break;
    }

    treeDumpGraphWcs(akinator.root, akinatorPtrToStr);

    base_file = fopen("base.txt", "w");
    akinatorDumpBaseToFile(&akinator, base_file);
    fclose(base_file);

    akinatorDtor(&akinator);

    logExit();
    return 0;
}
