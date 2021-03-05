/**
 * @file
 * @brief Parser source file.
*/

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

int parse_subgraph();
int parse_declare();
void parse_main();
void parse_graph();

/**
 * Constant char* array for mapping the token type to the corresponding error name.
*/
const char* const token_error_map[] = {
    "", "", "(", ")", "=", "<>", ">", "<", "<=", ">=", "{", "}", "main", "%type", "%declare", "%subgraph", "%operations",
    "directed or token undirected", "->", ",", ";", "color", "if", "traverse", "operation", "=>", "dfs or token bfs", ":", "EOF"
};

/**
 * Prints the syntax error corresponding to the expected type with the error line and column mention.
 * 
 * @param expected_token The expected token type that was failed to match.
*/
void syntax_error(const TokenType expected_token) { 
    const char *expected = NULL;
    if (expected_token == ID_TOKEN)
        expected = "identifier";
    else if (expected_token == NUM_TOKEN)
        expected = "number";
    else if ((int) expected_token < TOKEN_COUNT)
        expected = token_error_map[expected_token];
    const char *received = NULL;
    if (current_token != NULL) {
        if (current_token->type == ID_TOKEN)
            received = "identifier";
        else if (current_token->type == NUM_TOKEN)
            received = "number";
        else
            received = current_token->token;
    }
    printf("Syntax Error: expected token %s but got %s at line %d, char %d\n",
        expected, received, current_token->start_ln, current_token->start_col);
}


/**
 * Prints the syntax error corresponding to the expected type with the error line and column mention.
 * 
 * @param expected_token The expected token type that was failed to match.
 * @return 1 if the current token type matchs the expected type, 0 if not.
*/
int match(const TokenType type_to_match) {
    if (current_token != NULL && current_token->type == type_to_match)
        return 1;
    return 0;
}

/**
 * Checks if the current token is a valid instruction.
 * 
 * @return 1 if valid instruction, 0 if not.
*/
int is_instruction() {
    return match(OPERATION_TOKEN) || match(LOOP_TOKEN) || match(IF_TOKEN);
}

/**
 * Checks if the current token is a valid operation parameter.
 * 
 * @return 1 if valid operation parameter, 0 if not.
*/
int is_operation_param() {
    return match(OPERATION_TOKEN) || match(ID_TOKEN) || match(COLOR_TOKEN);
}

/**
 * Checks if the current token is a valid condition expression.
 * 
 * @return 1 if valid condition expression, 0 if not.
*/
int is_expression() {
    return match(NUM_TOKEN) || match(OPERATION_TOKEN);
}

/**
 * Checks if the current token is a valid comparison operator.
 * 
 * @return 1 if valid comparison operator, 0 if not.
*/
int is_compare_op() {
    return match(EQ_TOKEN) || match(NEQ_TOKEN) || match(GT_TOKEN) || match(LT_TOKEN)
        || match(BEQ_TOKEN) || match(LEQ_TOKEN);
}

/**
 * Parses the next token and calls parse_graph() or parse_main() correspondingly.
 * If the parsed token is neither an identifier nor a main token, an error is printed and the parser halts.
*/
void parse_program() {
    next_token();
    if (!match(EOF_TOKEN)) {
        if (match(ID_TOKEN))
            parse_graph();
        else if (match(MAIN_TOKEN))
            parse_main();
        else {
            printf("Syntax Error: expected an identifier or keyword main but got %s at line %d, char %d",
                current_token->token, current_token->start_ln, current_token->start_col);
            return;
        }
    }
}

/**
 * Parses the graph type (%type) delcaration and calls the parse_subgraph() function.
 * 
 * @return 0 if a syntax error is found, else returns parse_subgraph() value.
*/
int parse_graph_type() {
    if (!match(PTYPE_TOKEN)) {
        syntax_error(PTYPE_TOKEN);
        return 0;
    }
    next_token();
    if (!match(OB_TOKEN)) {
        syntax_error(OB_TOKEN);
        return 0;
    }
    next_token();
    if (!match(GTYPE_TOKEN)) {
        syntax_error(GTYPE_TOKEN);
        return 0;
    }
    next_token();
    if (!match(CB_TOKEN)) {
        syntax_error(CB_TOKEN);
        return 0;
    }
    next_token();
    return parse_subgraph();
}

/**
 * Parses subgraphs declarations if next token matches %subgraph. Calls parse_declare() at the end.
 * 
 * @return 0 if a syntax error is found, else returns parse_declare() value.
*/
int parse_subgraph() {
    if (match(PSUBGRAPH_TOKEN)) { // Subgraph instances are optional
        next_token();
        if (!match(ID_TOKEN)) {
            syntax_error(ID_TOKEN);
            return 0;
        }
        while (match(ID_TOKEN)) {
            next_token();
            if (!match(COLON_TOKEN)) {
                syntax_error(COLON_TOKEN);
                return 0;
            }
            next_token();
            if (!match(ID_TOKEN)) {
                syntax_error(ID_TOKEN);
                return 0;
            }
            next_token();
            while (match(COMMA_TOKEN)) {
                next_token();
                if (!match(ID_TOKEN)) {
                    syntax_error(ID_TOKEN);
                    return 0;
                }
                next_token();
            }
            if (!match(SEMICOLON_TOKEN)) {
                syntax_error(SEMICOLON_TOKEN);
                return 0;
            }
            next_token();
        }
    }
    return parse_declare();
}

/**
 * Parses nodes & edges declarations.
 * 
 * @return 0 if a syntax error is found, 1 if not.
*/
int parse_declare() {
    if (!match(PDECLARE_TOKEN)) {
        syntax_error(PDECLARE_TOKEN);
        return 0;
    }
    next_token();
    if (!match(ID_TOKEN)) {
        syntax_error(ID_TOKEN);
        return 0;
    }
    while (match(ID_TOKEN)) {
        next_token();
        while (match(EDGE_TOKEN)) {
            int is_subgraph = 0;
            next_token();
            if (!match(ID_TOKEN)) {
                syntax_error(ID_TOKEN);
                return 0;
            }
            next_token();
            if (match(OP_TOKEN)) { // Subgraph call
                next_token();
                if (match(ID_TOKEN)) // Optional node parameter
                    next_token();
                if (!match(CP_TOKEN)) {
                    syntax_error(CP_TOKEN);
                    return 0;
                }
                is_subgraph = 1;
                next_token();
            }
            if (match(COMMA_TOKEN)) { // Optional weight
                next_token();
                if (!match(NUM_TOKEN)) {
                    syntax_error(NUM_TOKEN);
                    return 0;
                }
                next_token();
            }
            if (is_subgraph) // Can't consider the whole subgraph as a root to another graph/node
                break;
        }
        if (!match(SEMICOLON_TOKEN)) {
            syntax_error(SEMICOLON_TOKEN);
            return 0;
        }
        next_token();
    }
    return 1;
}

/**
 * Parses a single operation call, stoping at the closing parenthesis token.
 * Recursively calls itself if one of the operation parameters is also an operation.
 * 
 * @return 0 if a syntax error is found, 1 if not.
*/
int parse_operation_call() {
    if (!match(OPERATION_TOKEN)) {
        syntax_error(OPERATION_TOKEN);
        return 0;
    }
    next_token();
    if (!match(OP_TOKEN)) {
        syntax_error(OP_TOKEN);
        return 0;
    }
    next_token();
    if (is_operation_param()) {
        if (match(OPERATION_TOKEN)) { // Recursively call operation as a parameter of an another
            if (!parse_operation_call())
                return 0;
        }
        next_token();
        while (match(COMMA_TOKEN)) {
            next_token();
            if (!is_operation_param()) {
                printf("Syntax Error: expected operation parameter but got %s at line %d, chat%d\n",
                    current_token->token, current_token->start_ln, current_token->start_col);
                return 0;
            }
            if (match(OPERATION_TOKEN)) {
                if (!parse_operation_call())
                    return 0;
            }
            next_token();
        }
    }
    if (!match(CP_TOKEN)) {
        syntax_error(CP_TOKEN);
        return 0;
    }
    return 1;
}

/**
 * Parses successive valid instructions (operation call, if clause or traverse clause).
 * 
 * @return 0 if a syntax error is found, 1 if not.
*/
int operations_routine() {
    while (is_instruction()) {
        if (match(OPERATION_TOKEN)) {
            if (!parse_operation_call())
                return 0;
            next_token();
            if (!match(SEMICOLON_TOKEN)) {
                syntax_error(SEMICOLON_TOKEN);
                return 0;
            }
        }
        else if (match(LOOP_TOKEN)) {
            next_token();
            if (!match(OP_TOKEN)) {
                syntax_error(OP_TOKEN);
                return 0;
            }
            next_token();
            if (match(ID_TOKEN)) { // Optionally select what graph to traverse. If left out, main graph is traversed.
                next_token();
                if (!match(COMMA_TOKEN)) {
                    syntax_error(COMMA_TOKEN);
                    return 0;
                }
                next_token();
            }
            if (!match(GSEARCH_TOKEN)) {
                syntax_error(GSEARCH_TOKEN);
                return 0;
            }
            next_token();
            if (!match(COMMA_TOKEN)) {
                syntax_error(COMMA_TOKEN);
                return 0;
            }
            next_token();
            if (!match(OP_TOKEN)) {
                syntax_error(OP_TOKEN);
                return 0;
            }
            for (int i = 0; i < 3; i++) {
                next_token();
                if (!match(ID_TOKEN)) {
                    syntax_error(ID_TOKEN);
                    return 0;
                }
                next_token();
                if (i < 2 && !match(COMMA_TOKEN)) {
                    syntax_error(COMMA_TOKEN);
                    return 0;
                }
            }
            if (!match(CP_TOKEN)) {
                syntax_error(CP_TOKEN);
                return 0;
            }
            next_token();
            if (!match(ARROW_TOKEN)) {
                syntax_error(ARROW_TOKEN);
                return 0;
            }
            next_token();
            if (!match(OB_TOKEN)) {
                syntax_error(OB_TOKEN);
                return 0;
            }
            next_token();
            if (!operations_routine())
                return 0;
            if (!match(CB_TOKEN)) {
                syntax_error(CB_TOKEN);
                return 0;
            }
            next_token();
            if (!match(CP_TOKEN)) {
                syntax_error(CP_TOKEN);
                return 0;
            }
            next_token();
            if (!match(SEMICOLON_TOKEN)) {
                syntax_error(SEMICOLON_TOKEN);
                return 0;
            }
        }
        else { // an IF_TOKEN is read
            next_token();
            if (!match(OP_TOKEN)) {
                syntax_error(OP_TOKEN);
                return 0;
            }
            next_token();
            if (!is_expression()) { // expression == either a number or an operation call
                printf("Syntax Error: expected an expression but got %s at line %d, char %d\n",
                    current_token->token, current_token->start_ln, current_token->start_col);
                return 0;
            }
            if (match(OPERATION_TOKEN)) {
                if (!parse_operation_call())
                    return 0;
            }
            next_token();
            if (is_compare_op()) {
                next_token();
                if (!is_expression()) {
                    printf("Syntax Error: expected an expression but got %s at line %d, char %d\n",
                        current_token->token, current_token->start_ln, current_token->start_col);
                    return 0;
                }
                if (match(OPERATION_TOKEN)) {
                    if (!parse_operation_call())
                        return 0;
                }
                next_token();
            }
            if (!match(CP_TOKEN)) {
                syntax_error(CP_TOKEN);
                return 0;
            }
            next_token();
            if (!match(OB_TOKEN)) {
                syntax_error(OB_TOKEN);
                return 0;
            }
            next_token();
            if (!operations_routine()) {
                return 0;
            }
            if (!match(CB_TOKEN)) {
                syntax_error(CB_TOKEN);
                return 0;
            }
        }
        next_token();
    }
    return 1;
}

/**
 * Parses an operations block.
 * 
 * @return 0 if a syntax error is found, 1 if not.
*/
int parse_operations() {
    if (!match(POPERATIONS_TOKEN)) {
        syntax_error(POPERATIONS_TOKEN);
        return 0;
    }
    next_token();
    if (!is_instruction()) { // At least one instruction should be written (Predefined operation, if clause or traverse clause)
        printf("Syntax Error: expected an operation or instruction but got %s at line %d, char %d",
            current_token->token, current_token->start_ln, current_token->start_col);
        return 0;
    }
    return operations_routine();
}

/**
 * Parses a graph declaration. Calls parse_program() at the end.
*/
void parse_graph() {
    if (!match(ID_TOKEN)) {
        syntax_error(ID_TOKEN);
        return;
    }
    next_token();
    if (!match(OB_TOKEN)) {
        syntax_error(OB_TOKEN);
        return;
    }
    next_token();
    if (!parse_graph_type()) // Already calls parse_subgraph and parse_delcare, and points on the next token
        return;
    if (!match(CB_TOKEN)) {
        syntax_error(CB_TOKEN);
        return;
    }
    parse_program();
}

/**
 * Parses a main block. Calls parse_operations() to parse the operations block.
*/
void parse_main() {
    if (!match(MAIN_TOKEN)) {
        syntax_error(MAIN_TOKEN);
        return;
    }
    next_token();
    if (!match(OB_TOKEN)) {
        syntax_error(OB_TOKEN);
        return;
    }
    next_token();
    if (!parse_graph_type())
        return;
    if (!parse_operations())
        return;
    if (!match(CB_TOKEN)) {
        syntax_error(CB_TOKEN);
        return;
    }
    next_token();
    if (!match(EOF_TOKEN)) {
        syntax_error(EOF_TOKEN);
        return;
    }
}
