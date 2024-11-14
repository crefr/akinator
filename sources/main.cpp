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
    // logCancelBuffer();

    FILE * base_file = fopen("base.txt", "r");
    akinator_t akinator = {};
    akinatorCtor(&akinator, base_file);
    fclose(base_file);

    treeDumpGraphWcs(akinator.root, akinatorPtrToStr);

    akinator_mode_t launch_mode = akinatorGetMode(&akinator);
    switch (launch_mode){
        case PLAY_MODE:{
            printf("HELLO\n");
            akinatorPlay(&akinator);
            break;
        }
        case DEFINITION_MODE:{
            wchar_t object_name[STR_BUFFER_LEN] = L"";
            wscanf(L"%ls", object_name);
            definition_t def = {};
            akinatorGiveDefinition(&akinator, &def, object_name);
            akinatorPrintDefinition(&def);
            break;
        }
        case DIFF_MODE:{
            wchar_t  first_name[STR_BUFFER_LEN] = L"";
            wchar_t second_name[STR_BUFFER_LEN] = L"";

            wscanf(L"%ls",  first_name);
            wscanf(L"%ls", second_name);

            definition_t  first_def = {};
            definition_t second_def = {};

            akinatorGiveDefinition(&akinator,  &first_def,  first_name);
            akinatorGiveDefinition(&akinator, &second_def, second_name);

            akinatorPrintDifference(&first_def, &second_def);
            break;
        }
        default:{
            wprintf(L"не поддерживаю такой режим / его не существует\n");
            break;
        }
    }

    treeDumpGraphWcs(akinator.root, akinatorPtrToStr);

    base_file = fopen("base.txt", "w");
    akinatorWriteBaseToFile(&akinator, base_file);
    fclose(base_file);

    akinatorDtor(&akinator);

    logExit();
    return 0;
}
