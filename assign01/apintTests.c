/*
 * Unit tests for arbitrary-precision integer data type
 *
 * These tests are by no means comprehensive.  You will need to
 * add more tests of your own!  In particular, make sure that
 * you have tests for more challenging situations, such as
 *
 * - large values
 * - adding/subtracting/comparing values with different lengths
 * - special cases (carries when adding, borrows when subtracting, etc.)
 * - etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apint.h"
#include "tctest.h"

typedef struct {
	ApInt *ap0;
	ApInt *ap1;
	ApInt *ap110660361;
	ApInt *max1;
	ApInt *minus1;
	ApInt *minus_max1;
       	ApInt *minus2; 	
	ApInt *ap2; 
	/* TODO: add additional fields of test fixture */
} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

void testCreateFromU64(TestObjs *objs);
void testHighestBitSet(TestObjs *objs);
void testCompare(TestObjs *objs);
void testFormatAsHex(TestObjs *objs);
void testAdd(TestObjs *objs);
void testSub(TestObjs *objs);
void testAddNoHex(TestObjs *objs); 
void testSubNoHex(TestObjs *objs); 
void testCreateFromHex(); 
void testLeftShiftOne(TestObjs *objs);
void testLeftShiftN(TestObjs *objs);
/* TODO: add more test function prototypes */

int main(int argc, char **argv) {
	TEST_INIT();

	if (argc > 1) {
		/*
		 * name of specific test case to execute was provided
		 * as a command line argument
		 */
		tctest_testname_to_execute = argv[1];
	}

	TEST(testCreateFromU64);
	TEST(testHighestBitSet);
	TEST(testCompare);
	TEST(testFormatAsHex);
	TEST(testAdd);
	TEST(testAddNoHex); 
	TEST(testSub);
	TEST(testSubNoHex); 
	TEST(testCreateFromHex);
       	TEST(testLeftShiftOne);
	TEST(testLeftShiftN); 	
	/* TODO: use TEST macro to execute more test functions */

	TEST_FINI();
}

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));
	objs->ap0 = apint_create_from_u64(0UL);
	objs->ap1 = apint_create_from_u64(1UL);
	objs->ap110660361 = apint_create_from_u64(110660361UL);
	objs->max1 = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL);
	objs->minus1 = apint_negate(objs->ap1);
	objs->minus_max1 = apint_negate(objs->max1); 
	objs->ap2 = apint_create_from_u64(2UL);
	objs->minus2 = apint_negate(objs->ap2); 
	/* TODO: initialize additional members of test fixture */

	return objs;
}

void cleanup(TestObjs *objs) {
	apint_destroy(objs->ap0);
	apint_destroy(objs->ap1);
	apint_destroy(objs->ap110660361);
	apint_destroy(objs->max1);
	apint_destroy(objs->minus1);
	apint_destroy(objs->minus_max1); 
	apint_destroy(objs->minus2); 
	apint_destroy(objs->ap2); 
	/* TODO: destroy additional members of test fixture */

	free(objs);
}

void testCreateFromU64(TestObjs *objs) {
	ASSERT(0UL == apint_get_bits(objs->ap0, 0));
	ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
}


void testHighestBitSet(TestObjs *objs) {
	ApInt *a; 
	ASSERT(-1 == apint_highest_bit_set(objs->ap0));
	ASSERT(0 == apint_highest_bit_set(objs->ap1));
	ASSERT(26 == apint_highest_bit_set(objs->ap110660361));
	ASSERT(63 == apint_highest_bit_set(objs->max1));
	
	a = apint_create_from_hex("98e2566a4f03c5a76b8aae935759304beedba68692afc477f5aa151248d37483575ac902da12f9595e13ae8c359c9554"); 
	ASSERT(383 == apint_highest_bit_set(a)); 
	apint_destroy(a); 
	
	a = apint_create_from_hex("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	ASSERT(-1 == apint_highest_bit_set(a)); 
	apint_destroy(a); 

	a = apint_create_from_hex("-98e2566a4f03c5a76b8aae935759304beedba68692afc477f5aa151248d37483575ac902da12f9595e13ae8c359c9554");
        ASSERT(383 == apint_highest_bit_set(a));
        apint_destroy(a);
}

void testCompare(TestObjs *objs) {
	ApInt *a; 
	ApInt *b;

	/* 1 > 0 */
	ASSERT(apint_compare(objs->ap1, objs->ap0) > 0);
	/* 0 < 1 */
	ASSERT(apint_compare(objs->ap0, objs->ap1) < 0);
	/* 110660361 > 0 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap0) > 0);
	/* 110660361 > 1 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap1) > 0);
	/* 0 < 110660361 */
	ASSERT(apint_compare(objs->ap0, objs->ap110660361) < 0);
	/* 1 < 110660361 */
	ASSERT(apint_compare(objs->ap1, objs->ap110660361) < 0);
	/* -1 < 1 */ 
	ASSERT(apint_compare(objs->minus1, objs->ap1) < 0); 
	/* 1 > -1 */ 
	ASSERT(apint_compare(objs->ap1, objs->minus1) > 0); 
	/* -1 < 0 */
	ASSERT(apint_compare(objs->minus1, objs->ap0) < 0);
       	/* 0 > -1 */
	ASSERT(apint_compare(objs->ap0, objs->minus1) > 0);
	/* 110660361 > -1 */
	ASSERT(apint_compare(objs->ap110660361, objs->minus1) > 0);
	/* -1 < 110660361 */
	ASSERT(apint_compare(objs->minus1, objs->ap110660361) < 0);
	/* minus_max1 < -1 */ 
	ASSERT(apint_compare(objs->minus_max1, objs->minus1) < 0); 
	/* -1 > minus_max1 */
	ASSERT(apint_compare(objs->minus1, objs->minus_max1) > 0);
	/* -1 = -1 */
	ASSERT(apint_compare(objs->minus1, objs->minus1) == 0); 	
	/* 1 = 1 */
	ASSERT(apint_compare(objs->ap1, objs->ap1) == 0); 
	/* 0 - 0 */ 
	ASSERT(apint_compare(objs->ap0, objs->ap0) == 0); 

	/* f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130 > d6250cfe8185987fbc8 */
	a = apint_create_from_hex("f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130"); 
	b = apint_create_from_hex("d6250cfe8185987fbc8"); 
	ASSERT(apint_compare(a, b) > 0); 
	apint_destroy(a); 
	apint_destroy(b); 

	/* -f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130 < d6250cfe8185987fbc8 */
        a = apint_create_from_hex("-f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130");
        b = apint_create_from_hex("d6250cfe8185987fbc8");
        ASSERT(apint_compare(a, b) < 0);
        apint_destroy(a);
        apint_destroy(b);

	/* -f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130 < -d6250cfe8185987fbc8 */
        a = apint_create_from_hex("-f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130");
        b = apint_create_from_hex("-d6250cfe8185987fbc8");
        ASSERT(apint_compare(a, b) < 0);
        apint_destroy(a);
        apint_destroy(b);

	a = apint_create_from_hex("-f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130");	
	b = apint_create_from_hex("-f70156b2dbeed0a8e03910fac6b48d1e0a6ff33168b03e05d992cfc6e5130");
	ASSERT(apint_compare(a, b) == 0);
        apint_destroy(a);
        apint_destroy(b);
}

void testFormatAsHex(TestObjs *objs) {
	char *s;
	ApInt *b; 

	s = apint_format_as_hex(objs->ap0); 
	ASSERT(0 == strcmp("0", s ));
	free(s);

	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(objs->ap1))));
	free(s);

	ASSERT(0 == strcmp("6988b09", (s = apint_format_as_hex(objs->ap110660361))));
	free(s);

	ASSERT(0 == strcmp("ffffffffffffffff", (s = apint_format_as_hex(objs->max1))));
	free(s);

	b = apint_create_from_hex("-7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	s = apint_format_as_hex(b);
	ASSERT(0 == strcmp("-7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d", s)); 
	free(s); 
	apint_destroy(b); 

	b = apint_create_from_hex("-efefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefef"); 
	s = apint_format_as_hex(b);
	ASSERT(0 == strcmp(s, "-efefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefef")); 
	free(s); 
	apint_destroy(b); 

	b = apint_create_from_hex("-ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12");
	s = apint_format_as_hex(b);
	ASSERT(0 == strcmp(s, "-ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12ef12")); 
	free(s); 
	apint_destroy(b); 

	b = apint_create_from_hex("005e0"); 
	ASSERT(0 == strcmp("5e0", (s=apint_format_as_hex(b)))); 
	apint_destroy(b); 
	free(s); 

	b = apint_create_from_hex("-7e6b"); 
	ASSERT(0 == strcmp("-7e6b", s=apint_format_as_hex(b))); 
	free(s); 
	apint_destroy(b); 
}

void testAddNoHex(TestObjs *objs) {
	ApInt *sum; 

	/* 0 + 0 = 0 */
	sum = apint_add(objs->ap0, objs->ap0); 
	ASSERT(sum->data[0] == 0);
       	ASSERT(sum->flags == 1); 	
	apint_destroy(sum);

	/* 0 + 1 = 1 */
	sum = apint_add(objs->ap0, objs->ap1); 
	ASSERT(sum->data[0] == 1);
       	ASSERT(sum->flags == 1); 	
	apint_destroy(sum);

	/* 0 + -1 = -1 */
	sum = apint_add(objs->ap0, objs->minus1); 
	ASSERT(sum->data[0] == 1); 
	ASSERT(sum->flags == 0); 
	apint_destroy(sum);


	/* -1 + 1 = 0 */
	sum = apint_add(objs->minus1, objs->ap1); 
	ASSERT(sum->data[0] == 0); 
	ASSERT(sum->flags == 1); 
	apint_destroy(sum); 
	
	/* -1 + -1 = -2 */
	sum = apint_add(objs->minus1, objs->minus1); 
	ASSERT(sum->data[0] == 2);
	ASSERT(sum->flags == 0); 
	apint_destroy(sum);
	
	/* -2 + -1 = -3 */
	sum = apint_add(objs->minus2, objs->minus1); 
	ASSERT(sum->data[0] == 3);
	ASSERT(sum->flags == 0); 
	apint_destroy(sum);
	

	/* -2 + 1 = -1 */
	sum = apint_add(objs->minus2, objs->ap1); 
	ASSERT(sum->data[0] == 1);
	ASSERT(sum->flags == 0); 
	apint_destroy(sum);
}

void testAdd(TestObjs *objs) {
	ApInt *sum;
	ApInt *a; 
	ApInt *b; 
	char *s;

	// 0 + 0 = 0 //
	sum = apint_add(objs->ap0, objs->ap0);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// 1 + 0 = 1 //
	sum = apint_add(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// 1 + 1 = 2 //
	sum = apint_add(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("2", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// 110660361 + 1 = 110660362 //
	sum = apint_add(objs->ap110660361, objs->ap1);
	ASSERT(0 == strcmp("6988b0a", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// FFFFFFFFFFFFFFFF + 1 = 10000000000000000 //
	sum = apint_add(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("10000000000000000", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	//539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e + f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52 = 539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0 //
	a = apint_create_from_hex("539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e"); 
	b = apint_create_from_hex("f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52"); 
	sum = apint_add(a, b); 
	ASSERT(0 == strcmp("539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0", (s = apint_format_as_hex(sum)))); 
	apint_destroy(a); 
	apint_destroy(b); 
	apint_destroy(sum); 
	free(s); 

	//539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e + f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52 = 539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0 //
	a = apint_create_from_hex("539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e"); 
	b = apint_create_from_hex("f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52"); 
	sum = apint_add(b, a); 
	ASSERT(0 == strcmp("539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0", (s = apint_format_as_hex(sum)))); 
	apint_destroy(sum);
        apint_destroy(a);
        apint_destroy(b);
	free(s); 

	//446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f + -fc70d28a3a58af237e01831253dfa8119 = 446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36 //
	a = apint_create_from_hex("446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f"); 
	b = apint_create_from_hex("-fc70d28a3a58af237e01831253dfa8119"); 
	sum = apint_add(a, b); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(sum)), "446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36")); 
	apint_destroy(sum); 
	apint_destroy(a);
        apint_destroy(b);
	free(s); 

	//446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f + -fc70d28a3a58af237e01831253dfa8119 = 446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36 //
	a = apint_create_from_hex("446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f"); 
	b = apint_create_from_hex("-fc70d28a3a58af237e01831253dfa8119"); 
	sum = apint_add(b, a); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(sum)), "446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36")); 
	apint_destroy(a);
        apint_destroy(b);
	apint_destroy(sum); 
	free(s);

	a = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981"); 
	b = apint_create_from_hex("-33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981"); 
	sum = apint_add(a, b); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(sum)), "0")); 
        apint_destroy(a);
        apint_destroy(b);
        apint_destroy(sum);
        free(s);
       
       	a = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981");    
        b = apint_create_from_hex("-33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981");   
        sum = apint_add(b, a);
        ASSERT(0 == strcmp((s = apint_format_as_hex(sum)), "0"));
        apint_destroy(a);
        apint_destroy(b);
        apint_destroy(sum);
        free(s);

        //-539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e + -f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52 = -539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0 //
        a = apint_create_from_hex("-539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e");
        b = apint_create_from_hex("-f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52");
        sum = apint_add(b, a);
        ASSERT(0 == strcmp("-539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0", (s = apint_format_as_hex(sum))));
        apint_destroy(sum);
        apint_destroy(a);
        apint_destroy(b);
        free(s);



}

void testSubNoHex(TestObjs *objs) {
	ApInt *diff; 

	// 0 - 0 = 0 //
	diff = apint_sub(objs->ap0, objs->ap0); 
	ASSERT(diff->data[0] == 0);
       	ASSERT(diff->flags == 1); 	
	apint_destroy(diff);

	// 0 - 1 = -1 //
	diff = apint_sub(objs->ap0, objs->ap1); 
	ASSERT(diff->data[0] == 1);
       	ASSERT(diff->flags == 0); 	
	apint_destroy(diff);
	
	// 0 - -1 = 1 //
	diff = apint_sub(objs->ap0, objs->minus1); 
	ASSERT(diff->data[0] == 1);
       	ASSERT(diff->flags == 1); 	
	apint_destroy(diff);

	// -1 - 0 = -1 //
	diff = apint_sub(objs->minus1, objs->ap0); 
	ASSERT(diff->data[0] == 1);
       	ASSERT(diff->flags == 0); 	
	apint_destroy(diff);

	// 0 - -1 = 1 //
	diff = apint_sub(objs->ap0, objs->minus1); 
	ASSERT(diff->data[0] == 1); 
	ASSERT(diff->flags == 1); 
	apint_destroy(diff);


	
	// -1 - 1 = -2 //
	diff = apint_sub(objs->minus1, objs->ap1); 
	ASSERT(diff->data[0] == 2); 
	ASSERT(diff->flags == 0); 
	apint_destroy(diff); 
	
	// -1 - -1 = 0 //
	diff = apint_sub(objs->minus1, objs->minus1); 
	ASSERT(diff->data[0] == 0);
	ASSERT(diff->flags == 1); 
	apint_destroy(diff);
	
	// -2 - -1 = -1 //
	diff = apint_sub(objs->minus2, objs->minus1); 
	ASSERT(diff->data[0] == 1);
	ASSERT(diff->flags == 0); 
	apint_destroy(diff);
	

	// -2 - 1 = -3 //
	diff = apint_sub(objs->minus2, objs->ap1); 
	ASSERT(diff->data[0] == 3);
	ASSERT(diff->flags == 0); 
	apint_destroy(diff);



}

void testCreateFromHex() {
	ApInt *a, *b; 
	char *s; 

       	uint64_t value = 0x7e6b; 	
	a = apint_create_from_hex("7e6b"); 
	ASSERT(a->data[0] == value); 
	apint_destroy(a); 
	
	a = apint_create_from_hex("-0"); 
	ASSERT(a->data[0] == 0UL); 
	ASSERT(a->flags == 1); 
	ASSERT(a->len == 1);
	apint_destroy(a); 

	a = apint_create_from_hex("-000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"); 
	ASSERT(a->len == 1);
	ASSERT(a->data[0] == 0UL); 
	ASSERT(a->flags == 1); 
	apint_destroy(a); 

	a = apint_create_from_hex("-00000000000ef2345abde789878"); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(a)),"-ef2345abde789878"));
	ASSERT(a->len == 1); 
	ASSERT(a->flags == 0); 
	apint_destroy(a);
	free(s);

	b = apint_create_from_hex("ffffffffffffffff"); 
	value = 0xffffffffffffffff; 
	ASSERT(b->data[0] == value); 
	apint_destroy(b); 


	b = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	ASSERT(b->len == 5); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(b)),"7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d"));
	apint_destroy(b); 
	free(s);

	b = apint_create_from_hex("005e0"); 
	value = 0x005e0;
	ASSERT(b->data[0] == value); 
	apint_destroy(b); 

	b = apint_create_from_hex("-7e6b"); 
	value = 0x7e6b; 
	ASSERT(b->data[0] == value); 
	ASSERT(b->flags == 0); 
	apint_destroy(b); 
}
void testSub(TestObjs *objs) {
	ApInt *a, *b, *diff;
	char *s;

	// subtracting 1 from ffffffffffffffff is fffffffffffffffe //
	diff = apint_sub(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("fffffffffffffffe", (s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	free(s);

	// subtracting 0 from 1 is 1 //
	diff = apint_sub(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap1));
	apint_destroy(diff);
	free(s);

	// subtracting 1 from 1 is 0 //
	diff = apint_sub(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap0));
	apint_destroy(diff);
	free(s);

	// subtracting 1 from 0 is -1 //
	diff = apint_sub(objs->ap0, objs->ap1);
	ASSERT(0 == (strcmp("-1", (s = apint_format_as_hex(diff)))));
	ASSERT(0 == apint_compare(diff, objs->minus1));
	apint_destroy(diff);
	free(s);

	// test involving larger values //
	a = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	b = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	// test involving larger values (with a negative difference) //
	a = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	b = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("-7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	//-539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e - f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52 = 539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0 //
	a = apint_create_from_hex("-539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e"); 
	b = apint_create_from_hex("f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52"); 
	diff = apint_sub(a, b); 
	ASSERT(0 == strcmp("-539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0", (s = apint_format_as_hex(diff)))); 
	apint_destroy(a); 
	apint_destroy(b); 
	apint_destroy(diff); 
	free(s); 

	//539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e + f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52 = 539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0 //
	a = apint_create_from_hex("539de8758b19e823b1badcccc9d587172a8117e2466f06c15bfd8ca26033661b8377b6795060c5feefab6975ec86634e"); 
	b = apint_create_from_hex("-f2229c93c3f42f893e398c4ca6e5b120dfb7c8d386f626d9aa08010543c52"); 
	diff = apint_sub(b, a); 
	ASSERT(0 == strcmp("-539de8758b19e823b1badcccc9d587172a903a0c0fab46045491703b24fdd4769585b1f5dd9935615d4609f5fcda9fa0", (s = apint_format_as_hex(diff)))); 
	apint_destroy(diff);
        apint_destroy(a);
        apint_destroy(b);
	free(s); 

	//446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f - fc70d28a3a58af237e01831253dfa8119 = 446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36 //
	a = apint_create_from_hex("446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f"); 
	b = apint_create_from_hex("fc70d28a3a58af237e01831253dfa8119"); 
	diff = apint_sub(a, b); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(diff)), "446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36")); 
	apint_destroy(diff); 
	apint_destroy(a);
        apint_destroy(b);
	free(s); 

	//446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f + -fc70d28a3a58af237e01831253dfa8119 = 446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36 //
	a = apint_create_from_hex("446572ec28bf1ce7ed6295b4cca3fcc4fe8e1d33f3fae4f"); 
	b = apint_create_from_hex("fc70d28a3a58af237e01831253dfa8119"); 
	diff = apint_sub(b, a); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(diff)), "-446572ec28bf1beb7c900b7a73f4d946fd0b0ae01452d36")); 
	apint_destroy(a);
        apint_destroy(b);
	apint_destroy(diff); 
	free(s);

	a = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981"); 
	b = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981"); 
	diff = apint_sub(a, b); 
	ASSERT(0 == strcmp((s = apint_format_as_hex(diff)), "0")); 
        apint_destroy(a);
        apint_destroy(b);
        apint_destroy(diff);
        free(s);
       
       	a = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981");    
        b = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981");   
        diff = apint_sub(b, a);
        ASSERT(0 == strcmp((s = apint_format_as_hex(diff)), "0"));
        apint_destroy(a);
        apint_destroy(b);
        apint_destroy(diff);
        free(s);
}

void testLeftShiftOne(TestObjs *objs) {
	ApInt *shift;
       	ApInt *a; 	
	char *s; 
	
	shift = apint_lshift(objs->ap0); 
	ASSERT(0 == strcmp(s = apint_format_as_hex(shift), "0")); 
	apint_destroy(shift); 
	free(s); 


	shift = apint_lshift(objs->ap1); 
	ASSERT(0 == strcmp(s = apint_format_as_hex(shift), "2")); 
	apint_destroy(shift); 
	free(s); 
	
	a = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL); 
        shift = apint_lshift(a);
        ASSERT(shift->len == (2));
        s = apint_format_as_hex(shift);
        ASSERT(0 == strcmp(s, "1fffffffffffffffe"));
        apint_destroy(shift);
        apint_destroy(a);
        free(s);


}

void testLeftShiftN(TestObjs *objs) {
	ApInt *shift; 
	ApInt *a; 
	char *s;

	shift = apint_lshift_n(objs->ap2, 66); 
	ASSERT(0 == strcmp(s = apint_format_as_hex(shift), "80000000000000000"));
	apint_destroy(shift); 
	free(s);

	shift = apint_lshift_n(objs->ap2, 3);
        ASSERT(0 == strcmp(s = apint_format_as_hex(shift), "10"));
        apint_destroy(shift);
        free(s);

	shift = apint_lshift_n(objs->ap1, 128);
	s = apint_format_as_hex(shift);
        ASSERT(0 == strcmp(s,"100000000000000000000000000000000")); 
        apint_destroy(shift);
        free(s);

	shift = apint_lshift_n(objs->ap1, 0);
        s = apint_format_as_hex(shift);
        ASSERT(0 == strcmp(s,"1"));
        apint_destroy(shift);
        free(s);

	a = apint_create_from_hex("33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981");
	shift = apint_lshift_n(a, 832); 
        ASSERT(shift->len == (13 + a->len));
        s = apint_format_as_hex(shift);
        ASSERT(0 == strcmp(s, "33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe11989810000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        apint_destroy(shift);
	apint_destroy(a); 
        free(s);

        a = apint_create_from_hex("-33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe1198981");
        shift = apint_lshift_n(a, 832);
        ASSERT(shift->len == (13 + a->len));
        s = apint_format_as_hex(shift);
        ASSERT(0 == strcmp(s, "-33d94c8042117410238aca99b998b81886c63ecdabf4d7eba99db79acc7c9fa7a3dc8663f75536bb4e1fe11989810000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        ASSERT(shift->flags == 0); 
	apint_destroy(shift);
        apint_destroy(a);
        free(s);

}
