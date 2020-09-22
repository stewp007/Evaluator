/* scan.c - token scanner */

#include "ntcalc.h"

char *scan_token_strings[] = SCAN_TOKEN_STRINGS;

/*
* prints out an error message when scanning
*/
void scan_error(char *err) {
    printf("scan_error: %s\n", err);
    exit(-1);
}

/*
* initializes the scan_table_st struct
*/
void scan_table_init(struct scan_table_st *st) {
    st->len = 0;
    st->next = 0;
}

/*
* creates a new scan token and adds it to the scan table
*/
struct scan_token_st * scan_table_new_token(struct scan_table_st *st) {
    struct scan_token_st *tp;

    tp = &st->table[st->len];
    st->len += 1;

    return tp;
}

/*
* Reads a token starting at p for len characters.
*  Update the given token with the token string and token id.
*/
char * scan_read_token(struct scan_token_st *tp, char *p, int len,
                       enum scan_token_enum id) {
    int i;

    tp->id = id;
    for (i = 0; i < len; i++) {
        tp->value[i] = *p;
        p += 1;
    }
    tp->value[i] = '\0';
    return p;
}

/*
* Checks wether a char is a digit character
*/
bool scan_is_digit(char c) {
    return c >= '0' && c <= '9';
}

/*
* Checks wether a char is a hexadecimal letter character
*/
bool scan_is_hex_letter(char c){
	return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/*
* Checks wether a char is a whitespace character
*/
bool scan_is_whitespace(char c) {
    return (c == ' ' || c == '\t');
}

/*
* scans over whitespace starting and p and 
* finishing either when no more whitespace or reached end
*/
char * scan_whitespace(char *p, char *end) {
    while (scan_is_whitespace(*p) && (p < end)) {
        p += 1;
    }
    return p;
}

/*
* Scans through p until either no more integers or reached end. 
* Updates value with each found integer
*/
char * scan_integer(char *p, char *end, struct scan_token_st *tp) {
    int i = 0;
    while (scan_is_digit(*p) && p < end) {
        tp->value[i] = *p;
        p += 1;
        i += 1;
    }
    tp->value[i] = '\0';
    tp->id = TK_INTLIT;
    return p;
}

/*
* Checks wether a char is a binary digit character
*/
bool scan_is_bin_digit(char c) {
    return c == '0' || c ==  '1';
}

/*
* Checks wether a char is a hexadecimal digit character (number or letter)
*/
bool scan_is_hex_digit(char c) {
    return scan_is_digit(c) || scan_is_hex_letter(c);
}

/*
* Scans through p to create a binary scan token
*/
char *scan_binary_literal(char *p, char *end, struct scan_token_st *tp) {
    int i = 0;
    while (scan_is_bin_digit(*p) && p < end) {
        tp->value[i] = *p;
        p += 1;
        i += 1;
    }
    tp->value[i] = '\0'; // == 0
    tp->id = TK_BINLIT;
    return p;
}

/*
* Scans through p to create a hexadecimal scan token
*/
char *scan_hex_literal(char *p, char *end, struct scan_token_st *tp) {
    int i = 0;
    while (scan_is_hex_digit(*p) && p < end) {
        tp->value[i] = *p;
        p += 1;
        i += 1;
    }
    tp->value[i] = '\0'; // == 0
    tp->id = TK_HEXLIT;
    return p;
}

/*
* Scans through p to create and add different scan tokens to the scan table
*/
char * scan_token(char *p, char *end, struct scan_token_st *tp) {
    if (p == end) {
        p = scan_read_token(tp, p, 0, TK_EOT);
    } else if (scan_is_whitespace(*p)) {
        p = scan_whitespace(p, end);
        p = scan_token(p, end, tp);
    } else if (*p == '0' && *(p+1) == 'b') {
        p = scan_binary_literal(p + 2, end, tp);
    } else if (*p == '0' && *(p+1) == 'x') {
    	p = scan_hex_literal(p + 2, end, tp);
    }else if (scan_is_digit(*p)) {
        p = scan_integer(p, end, tp);
    } else if (*p == '+') {
        p = scan_read_token(tp, p, 1, TK_PLUS);
    } else if (*p == '-') {
        p = scan_read_token(tp, p, 1, TK_MINUS);
    } else if (*p == '*') {
        p = scan_read_token(tp, p, 1, TK_MULT);
    } else if (*p == '/') {
        p = scan_read_token(tp, p, 1, TK_DIV);
    } else if (*p == '~') {
        p = scan_read_token(tp, p, 1, TK_NOT);
    } else if (*p == '<' && *(p+1) == '<') {
        p = scan_read_token(tp, p, 2, TK_LSL);
    }else if (*p == '>' && *(p+1) == '>') {
        p = scan_read_token(tp, p, 2, TK_LSR);
    }else if (*p == '>' && *(p+1) == '-'){
    	p = scan_read_token(tp, p, 2, TK_ASR);
    }else if (*p == '^') {
    	p = scan_read_token(tp, p, 1, TK_XOR);
    }else if (*p == '&') {
        p = scan_read_token(tp, p, 1, TK_AND);
    }else if (*p == '|') {
        p = scan_read_token(tp, p, 1, TK_OR);
    }else if (*p == '(') {
        p = scan_read_token(tp, p, 1, TK_LPAREN);
    } else if (*p == ')') {
        p = scan_read_token(tp, p, 1, TK_RPAREN);        
    } else {
        scan_error("Invalid character");
    }

    return p;
}

/*
* Scans throught the program input and creates the scan tokens, stopping once 
* TK_EOT is created and all of the input has been scanned
*/
void scan_table_scan(struct scan_table_st *st, char *input, int len) {
    struct scan_token_st *tp;
    char *p = input;
    char *end = p + len;

    while(true) {
        tp = scan_table_new_token(st);
        p = scan_token(p, end, tp);
        if (tp->id == TK_EOT) {
            break;
        }
    }
}

/* pretty printing of the scan_token_table structure once scanning is completed */
void scan_token_print(struct scan_token_st *t) {
    printf("%s(\"%s\")\n", scan_token_strings[t->id], t->value);
}

void scan_table_print(struct scan_table_st *st) {
    int i;

    for (i = 0; i < st->len; i++) {
        scan_token_print(&st->table[i]);
    }
}

/*
* gets a scan token from the scan table
*/
struct scan_token_st * scan_table_get(struct scan_table_st *st, int i) {
    return &(st->table[st->next + i]);
}

/*
* Returns true or false if the scan token gotten from the scan table is not the type of token expected
*/
bool scan_table_accept(struct scan_table_st *st, enum scan_token_enum tk_expected) {
    struct scan_token_st *tp;
    enum scan_token_enum tk_input;
    
    bool match = true;

    tp = scan_table_get(st, 0);
    tk_input = tp->id;

    if ((tk_expected != TK_ANY) && (tk_input != tk_expected)) {
        match = false;
    }

    if (match) {
        st->next += 1;
    }

    return match;
}
