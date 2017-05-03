/*
 *   Filename: ssl.cpp
 *
 * Created on: 15 April 2016
 *     Author: Synergiance
 *
 * This is a module performs an SSL handshake and encrypts/decrypts
 * traffic using the OpenSSL library.
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "ssl.h"

SslObj::SslObj() {
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    // Pre-emptively set true, will be changed if something fails
    operational = true;

    sockbio = BIO_new(BIO_s_file());
    if(SSL_library_init() < 0) operational = false;
    method = SSLv23_client_method();
    if ( (ctx = SSL_CTX_new(method)) == NULL) operational = false;

    if (operational) {
        SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
        ssl = SSL_new(ctx);
    }
    sockfd = 0;
}

SslObj::~SslObj() {
    if (sockfd != 0) {
        SSL_free(ssl);
        X509_free(cert);
    }
    if (operational) {
        SSL_CTX_free(ctx);
    }
}

bool SslObj::initialize(int fd) {
    if (!operational) return false;
    if (fd == 0) return false;
    SSL_set_fd(ssl, fd);
    if ( SSL_connect(ssl) != 1 ) return false;
    sockfd = fd;
    cert = SSL_get_peer_certificate(ssl);
    certname = X509_NAME_new();
    certname = X509_get_subject_name(cert);
    return true;
}

bool SslObj::send(char *data, len) {
    //
}

bool SslObj::recv(char *data, len) {
    //
}
