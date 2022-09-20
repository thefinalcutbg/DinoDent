#include "Base64Convert.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <cstring>


static void Encode(const char* in, size_t in_len,
                     char** out, size_t* out_len)
{
    BIO* buff, * b64f;
    BUF_MEM* ptr;

    b64f = BIO_new(BIO_f_base64());
    buff = BIO_new(BIO_s_mem());
    buff = BIO_push(b64f, buff);

    BIO_set_flags(buff, BIO_FLAGS_BASE64_NO_NL);
    BIO_set_close(buff, BIO_CLOSE);
    BIO_write(buff, in, in_len);
    BIO_flush(buff);

    BIO_get_mem_ptr(buff, &ptr);
    (*out_len) = ptr->length;
    (*out) = (char*)malloc(((*out_len) + 1) * sizeof(char));
    memcpy(*out, ptr->data, (*out_len));
    (*out)[(*out_len)] = '\0';

    BIO_free_all(buff);
}

static void Decode(const char* in, size_t in_len,
                    char** out, size_t* out_len) 
{
    BIO* buff, * b64f;

    b64f = BIO_new(BIO_f_base64());
    buff = BIO_new_mem_buf((void*)in, in_len);
    buff = BIO_push(b64f, buff);
    (*out) = (char*)malloc(in_len * sizeof(char));

    BIO_set_flags(buff, BIO_FLAGS_BASE64_NO_NL);
    BIO_set_close(buff, BIO_CLOSE);
    (*out_len) = BIO_read(buff, (*out), in_len);
    (*out) = (char*)realloc((void*)(*out), ((*out_len) + 1) * sizeof(char));
    (*out)[(*out_len)] = '\0';

    BIO_free_all(buff);
}

std::string Base64Convert::encode(const char* in, size_t in_len)
{
    char* out = nullptr;
    size_t outLength = 0;

    Encode(in, in_len, &out, &outLength);

    std::string result(out, outLength);

    free(out);

    return result;
}
#include <QString>
std::string Base64Convert::decode(const std::string& input)
{
    return
    QString(QByteArray::fromBase64(input.data())).toStdString();
}
