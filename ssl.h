/*
 *   Filename: ssl.h
 *
 * Created on: 15 April 2016
 *     Author: Synergiance
 *
 * This is a module performs an SSL handshake and encrypts/decrypts
 * traffic using the OpenSSL library.
 */

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#ifndef SSL_H_
#define SSL_H_

class SslObj
{
public:
    SslObj();
    bool initialize(int fd);
    bool send(char *data, len);
    bool recv(char *data, len);
    virtual ~SslObj();
private:
    BIO        *sockbio = NULL;
    X509          *cert = NULL;
    X509_NAME *certname = NULL;
    const SSL_METHOD *method;
    SSL *ssl;
    SSL_CTX *ctx;
    int sockfd;
    bool operational;
};

#endif /* SSL_H_ */
