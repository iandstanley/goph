
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

extern Server server;


void test_set_server_settings() {

	_set_hostname("myhost");
	assert(strcmp(server.hostname, "myhost") == 0);
	TEST_PASS 

	_set_listening("127.0.0.1");
	assert(strcmp(server.listening, "127.0.0.1") == 0);
	TEST_PASS 

	_set_port("7070");
	assert(strcmp(server.port, "7070") == 0);
	TEST_PASS 

	_set_docroot("/tmp");
	assert(strcmp(server.docroot, "/tmp") == 0);
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
	TEST_PASS

	assert(strcmp(_strip_rn(lf), lf_stripped) == 0);
	TEST_PASS

	assert(strcmp(_strip_rn(cr), cr_stripped) == 0);
	TEST_PASS

	assert(strcmp(_strip_rn(both), both_stripped) == 0);
	TEST_PASS
}

void test_parse_url() {

	char * url = calloc(REQUEST_URL_SIZE+1, sizeof(char));

	_parse_url(url, "/");
	assert(strcmp(url, "/var/gopher/gophermap") == 0);
	TEST_PASS

	_parse_url(url, "/demo");
	assert(strcmp(url, "/var/gopher/demo") == 0);
	TEST_PASS

	_parse_url(url, "/gophermap");
	assert(strcmp(url, "/var/gopher/gophermap") == 0);
	TEST_PASS
}

void test_last_char() {
	char url1[] ="/var/gopher/";
	char url2[] ="/var/gopher/gophermap";
	char url3[] ="/var/gopher/gophermap.sample";

	assert(_last_char(url1) == '/');
	TEST_PASS

	assert(_last_char(url2) == 'p');
	TEST_PASS

	assert(_last_char(url3) == 'e');
	TEST_PASS
}

void test_propend_gophermap() {
	char * url = malloc(20);
	sprintf(url, "%s", "/var/gopher/");

	assert( strcmp(_propend_gophermap(url), "/var/gopher/gophermap") == 0 );
	TEST_PASS
}


void test_load_config() {
	_load_config(CONFIG_FILE);

	assert(strncmp(server.listening, "0.0.0.0", strlen("0.0.0.0")) == 0);
	assert(strncmp(server.port, "7777", strlen("7777")) == 0);
	assert(strncmp(server.docroot, "/var/gopher", strlen("/var/gopher")) == 0);

	TEST_PASS
}

/******************************************************/

int
main()
{
	test_set_server_settings();
	test_syslog();
	test_strip_rn();
	test_parse_url();
	test_last_char();
	test_propend_gophermap();
	test_load_config();

	printf("\nALL TESTS PASS\n");
}
