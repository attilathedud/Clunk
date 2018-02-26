#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

typedef struct {
    Buffer b;
} Editor;

void editor_cleanup( Editor * );
void editor_load_file( Editor *, const char *, const char * );
void editor_print( Editor * );

#endif
