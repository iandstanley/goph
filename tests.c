
/*
 * Test suite for testing libgopher functions.
 */

#include	<assert.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<syslog.h>

#include 	"gopher.h"
#include 	"error.h"
#include 	"private.h"

#define 	RUNNING(X)	printf("Running %s\n", X);
#define		TEST_PASS	printf("¬");

void test_create_server() {
	Server *s = create_server();

	assert(s != NULL);

	free(s);
	TEST_PASS 
}


void test_set_server_settings() {
	Server * s = create_server();

	_set_hostname(s, "myhost");
	assert(strcmp(s->hostname, "myhost") == 0);

	_set_listening(s, "127.0.0.1");
	assert(strcmp(s->listening, "127.0.0.1") == 0);

	_set_port(s, "7070");
	assert(strcmp(s->port, "7070") == 0);

	_set_docroot(s, "/tmp");
	assert(strcmp(s->docroot, "/tmp") == 0);

	free(s);
	TEST_PASS
}

void test_configure_server() {
	Server *s = create_server();
	assert(s != NULL);

	char args[9][13] = {"testsuite", "-n", "myhost", "-p", "7070", "-l", "127.0.0.1", "-r", "/tmp"};
	char *p[9];
	for (int i = 0; i < 9; i++) { p[i] = args[i]; }

	configure_server(s, 9, p);

	assert(strncmp(s->hostname, "myhost", sizeof("myhost")) == 0);
	assert(strncmp(s->listening, "127.0.0.1", sizeof("127.0.0.1")) == 0);
	assert(strncmp(s->docroot, "/tmp", sizeof("/tmp")) == 0);
	assert(strcmp(s->port, "7070") == 0);

	free(s);
	TEST_PASS
}

void test_closesyslog() {
	_close_syslog();
	syslog(LOG_ALERT, "syslog closed by test_syslog()");
	TEST_PASS
}
void test_opensyslog() {
	_open_syslog();
	syslog(LOG_ALERT, "syslog opened by test_syslog()");
	TEST_PASS
}
void test_syslog() {
	test_opensyslog();
	test_closesyslog();
	TEST_PASS
}

void test_strip_rn() {

	char neither[] = "no_linefeed";
	char lf[] = "with_linefeed\n";
	char lf_stripped[] = "with_linefeed";
	char cr[] = "with_carriage\r";
	char cr_stripped[] = "with_carriage";
	char both[] = "both\r\n";
	char both_stripped[] = "both";

	assert(strcmp(_strip_rn(neither), neither) == 0);
	assert(strcmp(_strip_rn(lf), lf_stripped) == 0);
	assert(strcmp(_strip_rn(cr), cr_stripped) == 0);
	assert(strcmp(_strip_rn(both), both_stripped) == 0);

	TEST_PASS
}

void test_parse_url() {

	char * root = "/\r\n";
	char * demo = "/demo";
	char * gophermap = "/gophermap";


	assert(strcmp(_parse_url("/"), "/var/gopher/") == 0);

	assert(strcmp(_parse_url("/demo"), "/var/gopher/demo") == 0);

	assert(strcmp(_parse_url("/gophermap"), "/var/gopher/gophermap") == 0);

	TEST_PASS
}

void test_full_url() {

	char * url = "/gophermap";
	//char * str = strcat(GOPHER_ROOT, "/gophermap");
	char * str = malloc(20 * sizeof(char));

	sprintf(str,"/var/gopher%s", "/gophermap");


	assert(strcmp(_full_url(url), str) == 0);
	
	TEST_PASS
}

/******************************************************/

int
main()
{

	/******************************************************/
	/* ALL functions defined above should be called below */
	/******************************************************/

	test_create_server();

	test_set_server_settings();

	test_configure_server();

	test_syslog();

	test_strip_rn();

	test_parse_url();

	test_full_url();

	printf("\nALL TESTS PASS\n");
}
