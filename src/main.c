#include "program/program.h"
#include <soloud_c.h>
int main(void)
{

    Program this = programCreate();
    programMainLoop(this);
    programDestroy(this);

    return 0;
}