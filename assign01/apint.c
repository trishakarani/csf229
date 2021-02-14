/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "apint.h"
#include <stdio.h>
#include <math.h>

/* Parameters: val (unsigned 64 bit value) 
 * Declare and initialize new ApInt using val 
 * */
ApInt *apint_create_from_u64(uint64_t val) {
        /* TODO: implement */
        ApInt *ap = (ApInt*) malloc(sizeof(ApInt));
        assert(ap != NULL); //check memory allocation
        ap->len = 1;
        ap->flags = 1; //1 = 0/+, 0 = - , unsigned, so always +
        ap->data = (uint64_t *)calloc(1, sizeof(uint64_t));
        ap->data[0] = val;
        return ap;
}

/* 
 * char to int conversion for hex values
 * used in apint_create_from_hex
 * returns -1 if invalid hex character 
 * */
int charToInt(char c) {
        int new_value;
        switch(c) {
                case 'a': case 'A':
                        new_value = 10;
                        break;
                case 'b': case 'B':
                        new_value = 11;
                        break;
                case 'c': case 'C':
                        new_value = 12;
                        break;
                case 'd': case 'D':
                        new_value = 13;
                        break;
                case 'e': case 'E':
                        new_value = 14;
                        break;
                case 'f': case 'F':
                        new_value = 15;
                        break;
                default:
                        if (c >= '0' && c <= '9') {
                                new_value = (int) (c - '0');
                        } else {
                                new_value = -1;
                        }
                        break;
        }
        return new_value;
}
/* 
 * Determines if string is all 0
 * Return 1: if all 0, 0 if not all 0
 * Used in apint_create_from_hex
 */
int hex_is_zero(const char *hex) {
	for (int i=0; hex[i]; i++) {
		if (hex[i] != '0') {
			return 0; 
		}
	}
	return 1; 
}

/* 
 * Parameters: ApInt to be filled with data from hex string, hex string, start index for hex string
 * Returns the same ApInt pointer or NULL if invalid hex character
 * Used in apint_create_from_hex
 */
ApInt * fill_data_from_hex(ApInt * ap, const char * hex, int start) {
	int string_counter = strlen(hex) - 1; //subtract 1 for final position
	uint64_t new_value = 0, mult_counter = 1;
	int char_to_int_value = 0, hex_counter = 0, data_counter = 0;
	char current_char;
	while(string_counter >= start) { //start reading from highest elem of string until start position
                current_char = *(hex + string_counter);

                char_to_int_value = charToInt(current_char);
                if (char_to_int_value < 0) { //invalid character (not hex)
                        apint_destroy(ap); //free data in ap
                        return NULL; 
                } else {
                        new_value = char_to_int_value; //valid hex
                }
                new_value = new_value * mult_counter; //set to next set of bits
                mult_counter *= 16;
                if (hex_counter == 15) { 
			ap->data[data_counter] = ap->data[data_counter] + new_value;
                        if (string_counter > start) {
                                data_counter++;
                        }
                        mult_counter = 1;
                        hex_counter = 0;
                } else {
			ap->data[data_counter] = ap->data[data_counter] + new_value;
                        hex_counter++; //move to next hex elem
                }
                string_counter--;
        }
	return ap;
}

/* 
 * Converts string of hex values to ApInt 
 * Returns pointer to ApInt instance, NULL if invalid hex string
 */
ApInt *apint_create_from_hex(const char *hex) {
        ApInt *ap = (ApInt*) malloc(sizeof(ApInt));
        assert(ap != NULL); //check memory allocation
        ap->flags = 1;
        int start = 0;
        if (*hex == '-') { //neg hex value
                ap->flags = 0; //1 = 0/+, 0 = -
                start++; //if neg, start reading hex at next char
        }
	if (hex_is_zero(hex+start)) { //if 0, only assign 1 length data
		set_zero_data(ap); 
		return ap; 
	}

        while(*(hex + start) == '0') { //ignore leading zeros
                start++;
        }

	ap->len = (strlen(hex) - start) / 16;
        if ((strlen(hex) - start) % 16 > 0) { //account for leftover bits
                ap->len = ap->len + 1;
        }

        ap->data = (uint64_t *)calloc(ap->len, sizeof(uint64_t));
	return fill_data_from_hex(ap, hex, start);
}

/*
 * Destructor, frees memory in data and ApInt
 */
void apint_destroy(ApInt *ap) {
        /* TODO: implement */
        free(ap->data);
        free(ap);
}

/* 
 * Determines if ApInt data is equal to 0 
 * Loops through all elements of ApInt data array
 * Returns 0 if apint is positive or negative
 * Returns 1 if apint is 0 
 * */
int apint_is_zero(const ApInt *ap) {
        unsigned int i;
        //all elements of data must be 0
        for (i=0; i<ap->len; i++) {
                if(ap->data[i] != 0) {
                        return 0;
                }
        }
        return 1;
}

/* 
 * Determines if ApInt is negative
 * Returns 0 if apint is 0 or positive
 * Returns 1 if apint is negative 
 * */
int apint_is_negative(const ApInt *ap) {
        if (apint_is_zero(ap) == 1) {
                return 0;
        }
        return (ap->flags == 1) ? 0 : 1;
}

/* 
 * Returns the bits at position n of ApInt data array
 * Returns 0 if n is out of bounds 
 */
uint64_t apint_get_bits(const ApInt *ap, unsigned n) {
        if (n > ap->len - 1) {
                return 0;
        }
        return ap->data[n];
}

/*
 * Determines position of highest bit set to 1 in ApInt 
 * Returns -1 if no highest bit (value is 0)
 */
int apint_highest_bit_set(const ApInt *ap) {
        uint64_t highest = 0x8000000000000000UL; //highest bit
        uint64_t current_elem;
        int data_pos = ap->len - 1;
        for (; data_pos >= 0; data_pos--) {
                current_elem = ap->data[data_pos];
        //while loop starts at highest bit and decrements until match found
                for (int count = 63; count >= 0; count--) {
                        if (highest & current_elem) { //if true, then current_elem is highest bit
                                return data_pos * 64 + count;
                        }
                        highest = highest  >> 1; //move to next bit on the right
                }
        }
        return -1;
}

/*
 * Converts uint64_t to hex char
 */
char intToChar(uint64_t i) {
        char new_value = '0';

        switch(i) {
                case 10:
                        new_value = 'a';
                        break;
                case 11:
                        new_value = 'b';
                        break;
                case 12:
                        new_value = 'c';
                        break;
                case 13:
                        new_value = 'd';
                        break;
                case 14:
                        new_value = 'e';
                        break;
                case 15:
                        new_value = 'f';
                        break;
                default:
                        new_value = (char) i + '0';
                        break;
        }
        return new_value;
}

/* 
 * Reverses given hex string 
 * Format to remove leading zeros, include negative sign, null terminator
 * */
char * rev_hex_string(const ApInt *ap, char * hex) {
	
	int is_neg = ap->flags == 0 ? 1 : 0;

        //reverse string
        uint32_t total_hex_len = 16 * ap->len;
        char * rev_hex = calloc(total_hex_len , sizeof(char));
        for (uint32_t i = 0; i < total_hex_len; i++) {
                rev_hex[i] = hex[total_hex_len -1 - i];
        }

	//remove leading 0s
        uint32_t start = 0;
        while(*(rev_hex + start) == '0') {
                start++;
        }

        char * final_hex = calloc( ap->len*16 - start + 1 + is_neg, sizeof(char));
        size_t f_hex_count = 0;
        if (is_neg) {
                final_hex[f_hex_count++] = '-';
        }

        for (uint32_t i = start; i < total_hex_len; i++) {
                final_hex[f_hex_count++] = *(rev_hex + i);
        }

        final_hex[f_hex_count] = '\0';

	free(rev_hex); 
	return final_hex; 
}

/* 
 * Converts ApInt data to a char array of the hex value
 */

char *apint_format_as_hex(const ApInt *ap) {
        if (apint_is_zero(ap) == 1) {
                char *hex = malloc(2*sizeof(char));
                hex[0] = '0';
                hex[1] = '\0';
                return hex;
        }
        char *hex = calloc((16 * ap->len), sizeof(char));
	memset(hex, '0', 16 * ap->len * sizeof(char));
        int hex_length = 0;
        uint64_t div, rem;

        for (uint16_t i=0; i< ap->len; i++) {
                div = ap->data[i] / 16;
                rem = ap->data[i] % 16;
                hex_length = 16*i;
                hex[hex_length++] = intToChar(rem);
                while(div > 0) {
                        rem = div % 16;
                        div = div / 16;
                        hex[hex_length++] = intToChar(rem);
                }
        }

        char * final = rev_hex_string(ap, hex);
	free(hex); 
	return final;
}

/* 
 * Creates new instance of ApInt with same data value and opposite flag
 * If 0, flag remains 1
 */
ApInt *apint_negate(const ApInt *ap) {
        uint32_t i;
        int is_zero = 1; //1 if true, 0 if false

        ApInt *ap2 = (ApInt*) malloc(sizeof(ApInt));
        assert(ap2 != NULL);    //check if memory allocated

        //copy length and data of ap to negated ap
        ap2->len = ap->len;
        ap2->data = (uint64_t *)calloc(ap2->len, sizeof(uint64_t));
        for (i=0; i<ap->len; i++) {
                ap2->data[i] = ap->data[i];
                if(ap->data[i] != 0) {
                        is_zero = 0;
                }
        }

        if (is_zero == 1) {
                ap2->flags = ap->flags; //flag 1 for data == 0
        } else {
                ap2->flags = (ap->flags == 0) ? 1: 0; //input opposite flag
        }
        return ap2;
}

/* 
 * Allocates memory for ApInt data array 
 * Sets flag, len, data for condition 0
 * Flag is 1 for 0
 */
void set_zero_data(ApInt *ap) {
	ap->len = 1;
	ap->data = (uint64_t *)calloc(ap->len, sizeof(uint64_t));
	ap->data[0] = 0UL;
	ap->flags = 1;
}

/*
 * Perform mathematical subtraction for two ApInt instances 
 */
ApInt *calc_sub(const ApInt *a, const ApInt *b, ApInt *diff) {
        int a_greater = left_greater(a, b);
        const ApInt *greater = a;
      	const ApInt *less = b;
        if (a_greater == 0) { //subtraction of equal values is 0
		set_zero_data(diff); 
                return diff;
        } else if (a_greater == -1) {
                greater = b;
                less = a;
        }
        uint64_t next_borrow = 0, prev_borrow = 0;

        diff->flags = greater->flags; //flags and len from larger value
        diff->len = greater->len;
        diff->data = (uint64_t *)calloc(diff->len, sizeof(uint64_t));
        for (uint32_t pos = 0; pos < greater->len; pos++) {
                if (pos <= less->len - 1) {
                        if (greater->data[pos] < less->data[pos] + prev_borrow) { //overflow
                                next_borrow = 1;
                        } else {
                                next_borrow = 0;
                        }

                        diff->data[pos] = greater->data[pos] - less->data[pos] - prev_borrow;
                } else { //if less is shorter than greater 
			if (greater->data[pos] - prev_borrow > greater->data[pos]) { //overflow
				next_borrow = 1; 
			} else {
				next_borrow = 0; 
			}
                        diff->data[pos] = greater->data[pos] - prev_borrow;
                }
		
		prev_borrow = next_borrow; 
        }

        return diff;
}

/* 
 * Performs mathematical addition 
 */
ApInt *calc_add(const ApInt *a, const ApInt *b, ApInt *sum) {
        int a_greater = left_greater(a, b);
        const ApInt *greater = a;
        const ApInt *less = b;
        if (apint_is_zero(a) == 1 && apint_is_zero(b) == 1) { //addition of equal values
		set_zero_data(sum); 
                return sum;
        }
        if (a_greater == -1) {
                greater = b;
                less = a;
        }
        uint64_t next_carry = 0, prev_carry = 0;
        sum->flags = greater->flags; //set flags and len of greater
        sum->len = greater->len;
        sum->data = (uint64_t *)calloc(sum->len, sizeof(uint64_t));
	uint64_t sum_value; 
        for (uint32_t pos = 0; pos < greater->len; pos++) {
                if (pos <= less->len - 1) {
                        sum_value = greater->data[pos] + less->data[pos] + prev_carry;
                        if (sum_value < greater->data[pos] || sum_value < less->data[pos]) {
                                next_carry = 1;
                        } else {
                                next_carry = 0;
                        }

                } else {
			sum_value = greater->data[pos] + prev_carry; 
			if (sum_value < greater->data[pos]) {
				next_carry = 1; 
			} else {
				next_carry = 0; 
			}
                }
                sum->data[pos] = sum_value;
                prev_carry = next_carry;
        }

	//allocate additional data for carry over
        if (prev_carry == 1) {
                sum->data = (uint64_t *)realloc(sum->data, (sum->len + 1)*sizeof(uint64_t));
                sum->data[sum->len] = 1UL;
		sum->len = sum->len + 1;
        }

        return sum;
}

/* 
 * Returns addition of two ApInt instances
 */
ApInt *apint_add(const ApInt *a, const ApInt *b) {
        ApInt *result = (ApInt*) malloc(sizeof(ApInt));
        assert(result != NULL); //check memory allocation
        if (a->flags == b->flags) { //if both neg/pos, then data is sum
                result = calc_add(a, b, result);
        } else { //if opposite flags, then data is difference
               result = calc_sub(a, b, result);
	}
        return result;
}

/*
 * Returns subtraction of two ApInt instances
 */
ApInt *apint_sub(const ApInt *a, const ApInt *b) {
	ApInt *neg_b = apint_negate(b); 
	ApInt *result = apint_add(a, neg_b); //subtraction is addition of neg number
	apint_destroy(neg_b); 
	return result; 
}

/* 
 * Determines if left is greater in magnitude than right
 * Returns 1: left magnitude greater, -1: right magnitude greater, 0: equal magnitude
 */
int left_greater(const ApInt *left, const ApInt *right) {
        int length = left->len;

	//compare length, greater length = greater magnitude
	if (left->len > right->len) { 
		return 1; 
	}

	if (left->len < right->len) {
		return -1; 
	}

	//length is equal, compare each data value starting at highest bits
        for (int i=length-1; i>=0; i--) {
                if (left->data[i] > right->data[i]) {
                        return 1;
                } else if (left->data[i] < right->data[i]) {
                        return -1;
                }
        }

        return 0;

}

/* 
 * Determines if left is greater than right (includes signs)
 * Returns 1: left > right, -1: right > left, 0: right == left
 */
int apint_compare(const ApInt *left, const ApInt *right) {
        if (left->flags > right->flags) {
                return 1; //left pos, right neg
        }
        if (left->flags < right->flags) {
                return -1; //left neg, right pos
        }
        if (left->flags == 0) { //both neg
		return -1 * left_greater(left, right); 
	}
	if (left->flags == 1) { //both pos
		return left_greater(left, right); 
        }
        return 0;
}

/* 
 * Left shift by 1
 * Uses apint_lshift_n 
 */
ApInt *apint_lshift(ApInt *ap) {

	return apint_lshift_n(ap, 1); 

}

/* 
 * Performs shifts of elements of data array 
 * Left shift by multiples of 64 bits
 */
ApInt *full_left_shifts(ApInt *ap, ApInt *ap_shift, unsigned full_shifts) {
	for (uint32_t i=0; i<ap_shift->len; i++) {
                if (i < full_shifts) {
                        ap_shift->data[i] = 0UL;
                } else {
                        ap_shift->data[i] =  ap->data[i - full_shifts];
                }
        }
       	return ap_shift;
        
}

/* 
 * Performs bit by bit shifts of data array
 * Left shift of bits less than 64
 */
ApInt *bit_left_shifts(ApInt *ap, ApInt *ap_shift, unsigned indiv_shifts) {
	uint64_t prev_overflow = 0UL, this_overflow = 0UL, compare_with = 0x8000000000000000UL; 
	uint64_t right_shift = compare_with;
	for (unsigned i=0; i<indiv_shifts-1; i++) {
        	 right_shift = right_shift >> 1;
        	 compare_with = compare_with | right_shift;
	}
	for (uint32_t i=0; i<ap_shift->len; i++) {
		if (i < ap->len) {
			this_overflow = compare_with & ap->data[i];
			ap_shift->data[i] = ap->data[i] << indiv_shifts;
			ap_shift->data[i] = ap_shift->data[i] | prev_overflow;
		} else {
			ap_shift->data[i] = prev_overflow; 
		}
		prev_overflow = this_overflow >> (64-indiv_shifts);
	}

	return ap_shift;
}

/* 
 * Returns new ApInt instance of shifted left n times
 */
ApInt *apint_lshift_n(ApInt *ap, unsigned n) {
	ApInt *ap_shift = (ApInt*) malloc(sizeof(ApInt));
        assert(ap_shift != NULL); //check memory allocation
        ap_shift->flags  = ap->flags;

	int highest_bit = apint_highest_bit_set(ap); 
	int full_shifts = (n+1+highest_bit)/64;
	int indiv_shifts = (n+1+highest_bit) % 64; 
       	ap_shift->len = full_shifts; 
	if (indiv_shifts > 0) { //allocate an additional element of data array
	       ap_shift->len += 1; 
	}	       
	ap_shift->data = (uint64_t *)calloc(ap_shift->len, sizeof(uint64_t));

	if (n % 64 == 0) { //only shift by multiples of 64
		return full_left_shifts(ap, ap_shift, n/64); 
	} else if (n / 64 > 0) { //shift by multiples of 64, then bit-by-bit
		full_left_shifts(ap, ap_shift, n/64);
		bit_left_shifts(ap_shift, ap_shift, n%64);
		return ap_shift;
	} else { //only shift for n<64, bit-by-bit
		bit_left_shifts(ap, ap_shift, n); 
	}
	return ap_shift; 

}
