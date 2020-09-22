#include <stdio.h>

#include "ntcalc.h"

/*
* Evalutes the parse tree
*/
unsigned int eval_tree(struct parse_node_st *np) {
    unsigned int retval = 0;
    int signed_val = 0;
    unsigned int r = 0;
    unsigned int l = 0;
    int left;
    int right;

    switch (np->type) {
        case EX_INTVAL:
            retval = np->intval.value;
            break;
        case EX_OPER1:
            retval = eval_tree(np->oper1.expr);
            switch (np->oper1.oper) {
                case OP_MINUS:
                    retval = -retval;
                    break;
                case OP_NOT:
                    retval = ~retval;
                    break;
                default:
                    printf("eval: unsupported unary operator\n");
            }
            break;
        case EX_OPER2:
            r = eval_tree(np->oper2.right);
            l = eval_tree(np->oper2.left);
            switch (np->oper2.oper) {
                case OP_PLUS:
                    retval = l + r;
                    break;
                case OP_MINUS:
                    retval = l - r;
                    break;
                case OP_MULT:
                    retval = l * r;
                    break;
                case OP_DIV:
                    retval = l / r;
                    break;
                case OP_LSL:
                    retval = l << r;
                    break;
                case OP_LSR:
                	retval = l >> r;
                	break;
                case OP_ASR:
                	left = l;
               		right = r;
                	signed_val = left >> right; 
                	return  signed_val;
				case OP_AND:
					retval = l & r;
					break;
				case OP_OR:
					retval = l | r;
					break;
				case OP_XOR:
					retval = l ^ r;
					break;
                default:
                    printf("eval: unsupported binary operator\n");
            }
            break;
    }
    return retval;
}
