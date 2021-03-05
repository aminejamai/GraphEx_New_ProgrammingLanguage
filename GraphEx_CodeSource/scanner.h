/**
 * @file
 * @brief Scanner header file.
*/

#ifndef SCANNER_H_
#define SCANNER_H_ 

#define TOKEN_COUNT 29

/**
 * Enumeration of the different tokens that constitute the GraphEx grammar.
*/
typedef enum {
    ID_TOKEN, NUM_TOKEN, OP_TOKEN, CP_TOKEN, EQ_TOKEN, NEQ_TOKEN, 
    GT_TOKEN, LT_TOKEN, LEQ_TOKEN, BEQ_TOKEN, OB_TOKEN, CB_TOKEN, MAIN_TOKEN, 
    PTYPE_TOKEN, PDECLARE_TOKEN, PSUBGRAPH_TOKEN, POPERATIONS_TOKEN, GTYPE_TOKEN, 
    EDGE_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, COLOR_TOKEN, IF_TOKEN, LOOP_TOKEN,
    OPERATION_TOKEN, ARROW_TOKEN, GSEARCH_TOKEN, COLON_TOKEN, EOF_TOKEN
} TokenType;

/**
 * Defined type based on a struct holding various informations on a token.
*/
typedef struct {
    char* token;
    TokenType type;
    int start_ln;
    int start_col;
} TokenData;

TokenData* current_token; /** Pointer on the current token. */

FILE *PROGRAM_File; /** Pointer on the current file. */
char CURRENT_CHAR; /** Current character in the buffer. */
int CURRENT_ROW; /** Keeps count of the current line in the file. */
int CURRENT_COLUMN; /** Keeps count of the character in the current line. */

void next_token();
void readWord();
int isKeyword(char*);
void readNum();
void readTag();
int isTag(char*);
void readColor();
int isColor(char*);
int isSpace();
void readSpecialChar();
void generateError();

#endif