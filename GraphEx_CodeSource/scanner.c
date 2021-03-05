/**
 * @file
 * @brief Scanner source file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "scanner.h"

/**
 * Constant char* array for mapping the token type to its string.
*/
const char* const token_map[] = {
    "ID_TOKEN", "NUM_TOKEN", "OP_TOKEN", "CP_TOKEN", "EQ_TOKEN", "NEQ_TOKEN", "GT_TOKEN", "LT_TOKEN", "LEQ_TOKEN", "BEQ_TOKEN",
    "OB_TOKEN", "CB_TOKEN", "MAIN_TOKEN", "PTYPE_TOKEN", "PDECLARE_TOKEN", "PSUBGRAPH_TOKEN", "POPERATIONS_TOKEN", "GTYPE_TOKEN",
    "EDGE_TOKEN", "COMMA_TOKEN", "SEMICOLON_TOKEN", "COLOR_TOKEN", "IF_TOKEN", "LOOP_TOKEN", "OPERATION_TOKEN", "ARROW_TOKEN",
    "GSEARCH_TOKEN", "COLON_TOKEN", "EOF_TOKEN"
};

/**
 * Decides the next token type and calls the appropriate function.
*/
void next_token() {
    // clear the content of the current_token
    if (current_token != NULL) {
        free(current_token->token);
        free(current_token);
    }

    // re-allocate the space for current_token
    current_token = malloc(sizeof(TokenData));

    int has_space = 0;

    // read the next token
    if ((CURRENT_CHAR = getc(PROGRAM_File)) != EOF) {
        CURRENT_CHAR = tolower(CURRENT_CHAR); // GraphEx is case insensitive
        CURRENT_COLUMN++;
        while (isSpace() == 1) {
            CURRENT_CHAR = getc(PROGRAM_File);
            has_space = 1;
        }
        if (CURRENT_CHAR == EOF)
        {
            current_token->token = "EOF";
            current_token->type = EOF_TOKEN;
        }
        else if (isalpha(CURRENT_CHAR) != 0)
            {
                readWord();
            }else
            {
                if (isdigit(CURRENT_CHAR) != 0)
                {
                    readNum();
                }else
                {
                    if(CURRENT_CHAR == '%'){
                        readTag();
                    }
                    else
                    {
                        if (CURRENT_CHAR == '#'){
                            readColor();
                        }
                        else 
                            if(isSpace() == 0){ // if CURRENT_CHAR is not a space
                                readSpecialChar();
                            }
                    } 
                }  
            }
    }

    if (CURRENT_CHAR == EOF) {
        current_token->token = "eof";
        current_token->type = EOF_TOKEN;
        current_token->start_ln = CURRENT_ROW;
        if (!has_space)
            CURRENT_COLUMN++;
        current_token->start_col = CURRENT_COLUMN;
    }

    printf("%s | %s\n", current_token->token, token_map[(int) (current_token->type)]);
    return;
}

/**
 * Reads the next word token in the file and stores its data in the current_token variable.
*/
void readWord() {

    int col = CURRENT_COLUMN;
    int line = CURRENT_ROW;

    char token[25];
    token[0] = tolower(CURRENT_CHAR);
    
    int i = 1;
    char car;
    // read the word
    while ((car = fgetc(PROGRAM_File)) != EOF) {
        if (isalpha(car) || isdigit(car)) {
            car = tolower(car);
            CURRENT_COLUMN++;
            token[i]= car;
            CURRENT_CHAR = car;
            i++;
        }
        else {
            // Point on the last char of the word
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            break;
        }
    }
    token[i]='\0';    

    // Store the token
    current_token->token = malloc(strlen(token) + 1);
    strcpy(current_token->token, token);

    // Verify if the token is a keyword or just an ID
    current_token->type = isKeyword(token);
    current_token->start_ln = line;
    current_token->start_col= col;

    return;
}

/**
 * Checks if the given token is a keyword or an identifier.
 * 
 * @param token The token that has been read.
 * @return The corresponding token type value.
*/
int isKeyword(char *token) {
    if(strcmp(token, "main") == 0) return MAIN_TOKEN;
    if(strcmp(token, "directed") == 0) return GTYPE_TOKEN;
    if(strcmp(token, "undirected") == 0) return GTYPE_TOKEN;
    if(strcmp(token, "if") == 0) return IF_TOKEN;
    if(strcmp(token, "traverse") == 0) return LOOP_TOKEN;
    if(strcmp(token, "dfs") == 0) return GSEARCH_TOKEN;
    if(strcmp(token, "bfs") == 0) return GSEARCH_TOKEN;
    if(strcmp(token, "printall") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "printnodes") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "getchemin") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "getweight") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "getnode") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "exists") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "mincost") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "nombrechromatique") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "colorier") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "colorergraph") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "plot") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "dijkstra") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "bellman") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "dijkstrageneralise") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "kruskal") == 0) return OPERATION_TOKEN;
    if(strcmp(token, "prime") == 0) return OPERATION_TOKEN;

    return ID_TOKEN;
} 

/**
 * Reads the next number token in the file and stores its data in the current_token variable.
*/
void readNum() {

    int col = CURRENT_COLUMN;
    int line = CURRENT_ROW;

    char token[20]; // zone to put the number 
    token[0] = CURRENT_CHAR;
    
    char car;
    int i = 1;

    //read the number
    while ((car = fgetc(PROGRAM_File)) != EOF) {
        if (isdigit(car)) {
            CURRENT_COLUMN++;
            token[i]= car;
            CURRENT_CHAR = car;
            i++;
        }
        else {
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            break;
        }
    }
    token[i]='\0';

    // store the value and the type of the current token
    current_token->token = malloc(strlen(token) + 1);
    strcpy(current_token->token, token);

    current_token->type = NUM_TOKEN;
    current_token->start_ln = line;
    current_token->start_col = col;

    return;
}

/**
 * Reads the next tag token (%...) in the file and stores its data in the current_token variable.
*/
void readTag() {

    int col = CURRENT_COLUMN;
    int line = CURRENT_ROW;

    char token[25];
    token[0] = CURRENT_CHAR;
    
    int i = 1;
    char car;

    // read the word
    while ((car = fgetc(PROGRAM_File)) != EOF) {
        if (isalpha(car) || isdigit(car)) {
            car = tolower(car);
            CURRENT_COLUMN++;
            token[i]= car;
            CURRENT_CHAR = car;
            i++;
        }
        else {
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            break;
        }
    }
    token[i]='\0';

    // Stock the token
    current_token->token = malloc(strlen(token) + 1);
    strcpy(current_token->token, token);

    current_token->type = isTag(token);

    current_token->start_ln = line;
    current_token->start_col = col;

    if ((int) (current_token->type) == -1) {
        generateError();
    }

    return;
}

/**
 * Checks if the given token is a valid tag token.
 * 
 * @param token The token that has been read.
 * @return The corresponding token type if the token is a valid tag, -1 if not.
*/
int isTag(char* token) {
    if(strcmp(token, "%type") == 0) return PTYPE_TOKEN;
    if(strcmp(token, "\%declare") == 0) return PDECLARE_TOKEN;
    if(strcmp(token, "\%subgraph") == 0) return PSUBGRAPH_TOKEN;
    if(strcmp(token, "\%operations") == 0) return POPERATIONS_TOKEN;

    return -1;
}

/**
 * Reads the next color token in the file and stores its data in the current_token variable.
*/
void readColor() {

    int col = CURRENT_COLUMN;
    int line = CURRENT_ROW;

    char token[25];
    token[0] = CURRENT_CHAR;
    
    int i = 1;
    char car;

    // read the word
    while ((car = fgetc(PROGRAM_File)) != EOF) {
        if (isalpha(car) || isdigit(car)) {
            car = tolower(car);
            CURRENT_COLUMN++;
            token[i]= car;
            CURRENT_CHAR = car;
            i++;
        }
        else {
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            break;
        }
    }
    token[i]='\0';

    // Stock the token
    current_token->token = malloc(strlen(token) + 1);
    strcpy(current_token->token, token);

    int iscoleur = isColor(token);
    current_token->type = iscoleur ;

    current_token->start_ln = line;
    current_token->start_col= col;

    if (iscoleur == -1)
    {
        generateError();
    }

    return;
}

/**
 * Checks if the given token is a valid color token.
 * 
 * @param token The token that has been read.
 * @return The corresponding token type if the token is a color tag, -1 if not.
*/
int isColor(char* token) {
    if (strcmp(token, "#red") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#blue") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#green") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#yellow") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#black") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#pink") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#purple") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#orange") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#white") == 0) return COLOR_TOKEN;
    if (strcmp(token, "#gray") == 0) return COLOR_TOKEN;

    return -1;
}

/**
 * Checks if the current character is a space and increments CURRENT_COLUMN and CURRENT_ROW accordingly.
 * 
 * @return 1 if the current character is a space character, 0 if not.
*/
int isSpace() {
    if(CURRENT_CHAR == ' ') {CURRENT_COLUMN++; return 1;}
    if(CURRENT_CHAR == '\t') {CURRENT_COLUMN += 8; return 1;}
    if(CURRENT_CHAR == '\n'){
        CURRENT_COLUMN = 1;
        CURRENT_ROW++;
        return 1; 
    } 
    return 0;
}

/**
 * Reads the next special character and stores the data in the current_token variable.
*/
void readSpecialChar() {
    int col = CURRENT_COLUMN;
    int line = CURRENT_ROW;
    if(CURRENT_CHAR == ';'){
        current_token->token = ";";
        current_token->type = SEMICOLON_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    }
    if(CURRENT_CHAR == ',') {
        current_token->token = ",";
        current_token->type = COMMA_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    } 
    if(CURRENT_CHAR == '{'){
        current_token->token = "{";
        current_token->type = OB_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    }
    if(CURRENT_CHAR == '}'){
        current_token->token = "}";
        current_token->type = CB_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    }
    if(CURRENT_CHAR == '('){
        current_token->token = "(";
        current_token->type = OP_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    } 
    if(CURRENT_CHAR == ')'){
        current_token->token = ")";
        current_token->type = CP_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    }
    if(CURRENT_CHAR == ':'){
        current_token->token = ":";
        current_token->type = COLON_TOKEN;
        current_token->start_ln = line;
        current_token->start_col= col;
        return;
    } 
    if(CURRENT_CHAR == '=') {
        char car;
        car = fgetc(PROGRAM_File);
        if (car == '>') {
            CURRENT_COLUMN++;
            CURRENT_CHAR = car;
            current_token->token = "=>";
            current_token->type = ARROW_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
        }
        else {
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            current_token->token = "=";
            current_token->type = EQ_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
        }

        return;
    } 
    if (CURRENT_CHAR == '<')
    {
        char car;
        car = fgetc(PROGRAM_File);
        if (car == '>') {
            CURRENT_COLUMN++;
            CURRENT_CHAR = car;
            current_token->token = "<>";
            current_token->type = NEQ_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
        }
        if (car == '=') {
            CURRENT_COLUMN++;
            CURRENT_CHAR = car;
            current_token->token = "<=";
            current_token->type = LEQ_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
        }
        else {
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            current_token->token = "<";
            current_token->type = LT_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
        }

        return;
    }
    if (CURRENT_CHAR == '>')
    {
        char car;
        car = fgetc(PROGRAM_File);
        if (car == '=') {
            CURRENT_COLUMN++;
            CURRENT_CHAR = car;
            current_token->token = ">=";
            current_token->type = BEQ_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
            return;
        }
        else {
            fseek(PROGRAM_File, -1L, SEEK_CUR);
            current_token->token = ">";
            current_token->type = GT_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
        }
        
        return;
    }
    if(CURRENT_CHAR == '-'){
        char car;
        car = fgetc(PROGRAM_File);
        if(car == '>'){
            CURRENT_COLUMN++;
            CURRENT_CHAR = car;
            current_token->token = "->";
            current_token->type = EDGE_TOKEN;
            current_token->start_ln = line;
            current_token->start_col= col;
            return;
        } 
    }
    
    char token[3];
    token[0]= CURRENT_CHAR;
    token[1]='\0';

    strcpy(current_token->token, token);
    current_token->type = -1;
    current_token->start_ln = line;
    current_token->start_col= col;

    generateError();
    return;
}

/**
 * Prints the lexical error with the error line and column mention.
*/
void generateError() {
    printf("Lexical Error : invalid token %s at line %d, char %d\n", current_token->token, current_token->start_ln, current_token->start_col);
    exit(0);
}
