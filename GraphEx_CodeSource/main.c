#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

int main(int argc, char **args) {
    if (argc != 2) {
        if (argc < 2)
            printf("Error: No target file specified for the compiler\n");
        printf("Use: gx <filepath>\n");
        return EXIT_FAILURE;
    }

    PROGRAM_File = fopen(args[1], "r");

    if (PROGRAM_File == NULL) {
        printf("Error: failed to find target source file at path \"%s\"\n", args[1]);
        return EXIT_FAILURE;
    }
    
    CURRENT_CHAR = 'a';
    CURRENT_ROW = 1;
    CURRENT_COLUMN = 1;
    
    parse_program(); // Lexical and syntaxic analysis of the given file

    fclose(PROGRAM_File);

    return EXIT_SUCCESS;
}
