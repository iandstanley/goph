#pragma once

/***********************************************************
            _            _          __                      
 _ __  _ __(_)_   ____ _| |_ ___   / _|_   _ _ __   ___ ___ 
| '_ \| '__| \ \ / / _` | __/ _ \ | |_| | | | '_ \ / __/ __|
| |_) | |  | |\ V / (_| | ||  __/ |  _| |_| | | | | (__\__ \
| .__/|_|  |_| \_/ \__,_|\__\___| |_|  \__,_|_| |_|\___|___/
|_|                                                         
***********************************************************/

void _open_syslog(); 
void _close_syslog(); 
void _usage();

void _create_config();
void _load_config(char * configfile);
void _set_docroot(char * root);
void _set_listening(char *ip);
void _set_port(char * p);
void _set_hostname(char * newhost);
void _parse_url(char * url, char * buf);
char * _full_url(char * buf);
char * _strip_rn(char * buf);
char * _validate(char * buf);
void _process_request(int client_socket, char * buf);
void _send_document(int client_socket, char * doc);
char _last_char(char * url);
char * _propend_gophermap(char * url);
