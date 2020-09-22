/* parse.c - parsing and parse tree construction */

#include <ctype.h>

#include "ntcalc.h"


struct parse_node_st * parse_operand(struct parse_table_st *pt,
                                    struct scan_table_st *st);
struct parse_node_st * parse_expression(struct parse_table_st *pt,
                                        struct scan_table_st *st);

/*
* Initializes the parse_table_st structure
*/
void parse_table_init(struct parse_table_st *pt) {
    pt->len = 0;
    pt->next = 0;
}

/*
* creates a new parse_node_st and adds it to the parse node table
*/
struct parse_node_st * parse_node_new(struct parse_table_st *pt) {
    struct parse_node_st *np;

    np = &(pt->table[pt->len]);
    pt->len += 1;

    return np;
}

/*
* Prints out an error message and then exits gracefully
*/
void parse_error(char *err) {
    printf("parse_error: %s\n", err);
    exit(-1);
}

/*
* converts a string decimal value into long long in order to check for overflow
*/
long long parse_overflow(char * value){
	long long retval = 0;
	int place_val = 1;
	for (int i = strlen(value) - 1; i >= 0; i--) {

        char ch = tolower(value[i]);
        long long digit;
        
        if (ch >= '0' && ch <= '9'){
            digit = ch - '0';
		}
		
        retval += digit * place_val;
        place_val *= 10;
    }
    return retval;
}

/*
* Parses an operand from the scan_table and adds it to the Parse_table. 
* It also checks if the value will overflow for TK_INTLIT
*/
struct parse_node_st * parse_operand(struct parse_table_st *pt,
                                    struct scan_table_st *st) {
    struct scan_token_st *tp;
    struct parse_node_st *np, *np1;
    if (scan_table_accept(st, TK_LPAREN)) {
        np = parse_expression(pt, st);
        if (!scan_table_accept(st, TK_RPAREN)) {
            parse_error("Missing right paren");
        }
    } else if (scan_table_accept(st, TK_INTLIT)) {
        tp = scan_table_get(st, -1);
        np = parse_node_new(pt);
        np->type = EX_INTVAL;
        long long overflow = parse_overflow(tp->value);
        if(overflow > 4294967295){
        	printf("uint32 overflow: %s\n", tp->value);
        	exit(1);
        }
        np->intval.value = overflow;
    } else if (scan_table_accept(st, TK_BINLIT)) {
        tp = scan_table_get(st, -1);
        np = parse_node_new(pt);
        np->type = EX_INTVAL;        
        np->intval.value = conv_str_to_uint32(tp->value, 2);
    } else if (scan_table_accept(st, TK_HEXLIT)) {
        tp = scan_table_get(st, -1);
        np = parse_node_new(pt);
        np->type = EX_INTVAL;        
        np->intval.value = conv_str_to_uint32(tp->value, 16);
    } else if (scan_table_accept(st, TK_MINUS)) {
        tp = scan_table_get(st, -1);
        np1 = parse_node_new(pt);
        np1->type = EX_OPER1;
        np1->oper1.oper = OP_MINUS;
        np1->oper1.expr = parse_operand(pt, st);
        np = np1;
    } else if (scan_table_accept(st, TK_NOT)) {
        tp = scan_table_get(st, -1);
        np1 = parse_node_new(pt);
        np1->type = EX_OPER1;
        np1->oper1.oper = OP_NOT;
        np1->oper1.expr = parse_operand(pt, st);
        np = np1;
    } else {
       parse_error("Bad expression");
    }
    return np;
}

/*
* switches the scan_token id with the parse operand id
*/
enum parse_oper_enum parse_get_oper(enum scan_token_enum id) {
    enum parse_oper_enum rv;
    switch (id) {
        case TK_PLUS:
            rv = OP_PLUS; break;
        case TK_MINUS:
            rv = OP_MINUS; break;
        case TK_MULT:
            rv = OP_MULT; break;
        case TK_DIV:
            rv = OP_DIV; break;
        case TK_LSL:
            rv = OP_LSL; break;
		case TK_LSR:
			rv = OP_LSR; break;
		case TK_ASR:
			rv = OP_ASR; break;
		case TK_AND:
			rv = OP_AND; break;
		case TK_OR:
			rv = OP_OR; break;
		case TK_XOR:
			rv = OP_XOR; break;
        case TK_NOT:
            rv = OP_NOT; break;
        default:
            parse_error("Invalid operator");
    }
    return rv;
}

/*
* Parses a scan table expression into the parse table
*/
struct parse_node_st * parse_expression(struct parse_table_st *pt,
                                        struct scan_table_st *st) {
    struct scan_token_st *tp;
    struct parse_node_st *np, *np1;

    np = parse_operand(pt, st);

    while (true) {
        tp = scan_table_get(st, 0);
        if ((tp->id == TK_PLUS) || (tp->id == TK_MINUS) ||
            (tp->id == TK_MULT) || (tp->id == TK_DIV) || 
            (tp->id == TK_LSL)  || (tp->id == TK_LSR) ||
            (tp->id == TK_ASR)  || (tp->id == TK_AND) ||
            (tp->id == TK_OR)   || (tp->id == TK_XOR)) {
            scan_table_accept(st, TK_ANY);
            np1 = parse_node_new(pt);
            np1->type = EX_OPER2;
            np1->oper2.oper = parse_get_oper(tp->id);
            np1->oper2.left = np;
            np1->oper2.right = parse_operand(pt, st);
            np = np1;
        } else {
            break;
        }
    }

    return np;
}

/*
 * Parse tree pretty printing
 */

char *parse_oper_strings[] = {"PLUS", "MINUS", "MULT", "DIV", "LSL", "LSR", "ASR", "AND", "OR", "XOR", "NOT"};

void parse_tree_print_indent(int level) {
    level *= 2;
    for (int i = 0; i < level; i++) {
        printf(".");
    }
}

void parse_tree_print_expr(struct parse_node_st *np, int level) {
    parse_tree_print_indent(level);
    printf("EXPR ");

    if (np->type == EX_INTVAL) {
        printf("INTVAL %u\n", np->intval.value);
    } else if (np->type == EX_OPER1) {
        printf("OPER1 %s\n", parse_oper_strings[np->oper1.oper]);
        parse_tree_print_expr(np->oper1.expr, level+1);
    } else if (np->type == EX_OPER2) {
        printf("OPER2 %s\n", parse_oper_strings[np->oper2.oper]);
        parse_tree_print_expr(np->oper2.left, level+1);
        parse_tree_print_expr(np->oper2.right, level+1);
    }
}

void parse_tree_print(struct parse_node_st *np) {
    parse_tree_print_expr(np, 0);
}
