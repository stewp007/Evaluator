/* conv.h - performs conversions between string/uint and between bases */

#include <ctype.h>

#include "ntcalc.h"

/*
* Converts a str to a typ uint32_t. The base refers to the number base the string represents
*/
uint32_t conv_str_to_uint32(char *str, uint32_t base) {
    uint32_t retval = 0;
    uint32_t place_val = 1;
    
    while(*str == '0' && *str != '\0'){
    	str++;
    }
    if((strlen(str) > 8 && base == 16) || (strlen(str) > 32 && base == 2)){
    	printf("Uint32 overflow: %s\n", str);
    	exit(1);
    }
    for (int i = strlen(str) - 1; i >= 0; i--) {

        char ch = tolower(str[i]);
        uint32_t digit;

        if(ch >= 'a' && ch <= 'f'){
        	digit = ch - 'W';
        }
        
        if (ch >= '0' && ch <= '9'){
            digit = ch - '0';
		}
		
        retval += digit * place_val;
        place_val *= base;
    }
    return retval;
}

/*
* takes a binary string and inverts it (bitwise NOT ~). then converts the new string into a uint32_t.
* helper function for conv_uint32_to_decstr.
*/
uint32_t conv_bitwise_not(char * bin, int width){
	char not_bin[width];
	int i;
	for(i = 0; i < width; i++){
		if(bin[i] == '1'){
			not_bin[i] = '0';
		}else{
			not_bin[i] = '1';
		}
	}
	not_bin[i] = '\0';
	return conv_str_to_uint32(not_bin, 2);
	
}

/*
* converts a uin32_t into a decimal string. Works on positive and negative numbers
*/
void conv_uint32_to_decstr(struct ntlang_config_st *nc, uint32_t value, 
                           char *result_str) {
    int div, rem;
    int len, i, j = 0;
    int msb = nc->width-1;
    char tmp[SCAN_INPUT_LEN];
    
    conv_uint32_to_binstr(nc->width, value, tmp);
    value = conv_str_to_uint32(tmp+2, 2);
    
    if (value == 0) {
        tmp[len++] = '0';
    }else if(tmp[(int)strlen(tmp)-msb-1] == '1' && !nc->unsigned_output){ 
    /*Handles Negative Numbers */
        result_str[0] = '-';
        j++;
        int sign_value = conv_bitwise_not(tmp+2, nc->width);
        sign_value += 1;
            while(sign_value != 0){
            	div = sign_value / 10;
            	rem = sign_value % 10;
            	if(rem > 0){
            		tmp[len] = '0' + rem;
            	}else{
            		tmp[len] = '0' - rem;
            	}
            	len++;
            	sign_value = div;
            }
    }else {
    /*Handles Positive numbers*/
        while (value != 0) {
            div = value / 10;
            rem = value % 10;
            tmp[len] = '0' + rem;
            len += 1;
            value = div;
        }
    }

    for (i = len; i > 0; i--, j++) {
        result_str[j] = tmp[i-1];
    }
    result_str[j] = '\0';
}

/*
* Converts a uint32_t into a binary string
*/
void conv_uint32_to_binstr(int width, uint32_t value, 
                           char *result_str) {
    int i, j;
    
    result_str[0] = '0';
    result_str[1] = 'b';
    j = 2;
    for (i = width - 1; i >= 0; i -= 1) {
        if (((value >> i) & 0b1) == 1)
            result_str[j] = '1';
        else
            result_str[j] = '0';
        j += 1;      
    }
    
    result_str[j] = '\0';
}

/*
* Converts a uint32_t into a hexadecimal string
*/
void conv_uint32_to_hexstr(int width, uint32_t value, 
                           char *result_str) {
    int i, j, c = 0, len;
    uint32_t div, rem;
    char tmp[SCAN_INPUT_LEN];
    
    result_str[0] = '0';
    result_str[1] = 'x';
    j = 2;
    len = 0;
    
    if (value == 0) {
    	tmp[len++] = '0';
    } else {
        while (value != 0 || c < width/4) {
	    	div = value / 16;
            rem = value % 16;
            if(rem <= 9){
            	tmp[len] = rem + 48;
            }else{
            	tmp[len] = rem + 55;
            }
            len += 1;
            value = div;
            c++;
        }
    }
    
    for (i = width/4; i >= 0; i--, j++) {
        result_str[j] = tmp[i-1];
    }
    result_str[j] = '\0';
}

/*
* Converts a uint32_t into a string of either base 2, 10, or 16
*/
void conv_uint32_to_str(struct ntlang_config_st *nc, uint32_t value,
                        char *result_str) {
    switch (nc->base) {
        case 2:
            conv_uint32_to_binstr(nc->width, value, result_str);
            break;
        case 10:
            conv_uint32_to_decstr(nc, value, result_str);
            break;
        case 16:
            conv_uint32_to_hexstr(nc->width, value, result_str);
            break;
        default:
        	printf("Unsupported base: %d", nc->base);
    }
}
