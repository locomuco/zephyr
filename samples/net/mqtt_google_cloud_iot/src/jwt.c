#include "mbedtls/base64.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"

#include <misc/printk.h>
#include <stdio.h>
#include <string.h>

#include "jwt.h"

// openssl req -x509 -nodes -newkey rsa:2048 -keyout rsa_private.pem \
//     -days 1000000 -out rsa_cert.pem -subj "/CN=unused"

static const unsigned char *private_key =
"-----BEGIN PRIVATE KEY-----\r\n"
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCnoHgbhivOJGb1\r\n"
"wSf+nJN4kmDf6Pk9ryNyPPUX0quUUghlJequz8dQfbRpZgKyHMtHFq30w/sjff5L\r\n"
"+5HrQZNTgVO3/bFvegNfU9GMjkj9wroda913nN68BjGJOAep9s0VZOz+TsI5rZDg\r\n"
"souyVAZAANzP2Od9vrq/7+auVXpwJZFyqt0b3rRaVqjodfFkjxnJ65Hxp2eEJbto\r\n"
"udwgAb7l6x0HGIiIuTdJAJS3T7d2ygxnVBSzWe4M055JhXnslByuzGeGycWAQZCh\r\n"
"nwEoN3lZ7eiVIXIWbL4PbeG/o2pWbqMptg7LVvw9C/r7upndmU6qHl5IVqFOdGbl\r\n"
"7fjzLqyPAgMBAAECggEBAJMuy0cfoAb4346Zc0yUnsgXfwgbv/zxcuYzr+CjEhTB\r\n"
"kq19hXrI5fE+7tI8x9EshzB8MXHhQvn+/vIoig9PHNUNkTzUfJ+tOpjROAV6SYzH\r\n"
"FjQPKf6Etu3RYl1h3RjXo2tNmuoQrWmHhNZG73I9ZN994mR9x4UZm1A8jQg19bPR\r\n"
"XOK6Qap5R8svBMwMIqe68xurKrUpT24sfeGCL5KtOqQh6ZEGe7SAGMFv6IBMmL1U\r\n"
"YQNgaazGGt9nUYCNoy8TrsZI4vE7hj+tIr0tL4dJVHU/VTeKa0zI8ZiLSRpncpRf\r\n"
"wcGOenDURGRHK8ycMgTVOEsVxO0hUM3OIMtrT8AvYkECgYEA1MnD5zmqV4WaBo5r\r\n"
"9CKzTS4IB+OArYmcvm1smx+rwqg4r6FtHdVdSknA0Caw3yxwtQ0u+ZN+CKUwRj8r\r\n"
"cv7WAbb93MvME4bskxES+zrYxtF2wTL4alsU0hSC8wTJmRiADHANL1PFTlbibsxl\r\n"
"lqe+XFFd/zxS+ZiDGifsssyZhkUCgYEAyarmKG+pjQkqm5yNiUmOhTiis4QvmoTt\r\n"
"gypaBqW80TTZsYoK4ldprP/HrZKgXuJCtpesiiSFssVoi4EeroBzRNAyS7DayJhV\r\n"
"cUIJEOeZuYz38ZN9Zt4q1hXortsX1C3Mm8Xv7b3mpTZwFkIEVfPyF9JfI5AY4ERz\r\n"
"SAB4GG/5LsMCgYAj+0I/NUcSqarT9P1YOQ3svR3qh5PYxG5W3RYCXSakMeH0AO38\r\n"
"n3m4+CO8ju5VrqQYWXRaC5DCUXyKRGBKf8EcZR8kWOnXW3dhnv+AsAC8Detw2/eZ\r\n"
"tHEe3Q0Pnv2f/76XW9M+74Frr9F2PuXmxdKwtamZYU/+mMWChMHHskU7qQKBgDQa\r\n"
"N0STNNb/W0bA98BUhMHelAqVSEc/uK1+r6uv/kx+vw+jfpRjySWtH8W0cEKTanEK\r\n"
"VsWKEhDR9+SzA15KSX2p3JS5qIzS7qBIU1dOvuS1iwma9XcyRxbwqiEi8dEb1BUa\r\n"
"2pYtzf05R1Q40i3yzMVi0Lm7bPHkPXSIyn5lz5dJAoGBAMElds6LnF/TbYKNSiPg\r\n"
"UZK7KXjYCEQla77rC+PFIHRPCGDIwh6y0k6MKPJw0P7WUoeUlido6XduBhWYwrj3\r\n"
"8n3ht9nk6YfZlquASAaA75sTnjNn1HEozz98CRs+uEKRBAipOwcJNJkSYF6Etppz\r\n"
"ubHCdQNhkEJKm8sjtaZjIYOu\r\n"
"-----END PRIVATE KEY-----\r\n";


static int sign_rs256(char *payload, size_t payload_len,
                      const unsigned char *priv_key, jwt_signature *jwt);

const char payload[] = "{\"iat\":1521982343,\"exp\":1521984343,\"aud\":\"locomuco-c53df\"}";
const char header[] = "{\"alg\":\"RS256\",\"typ\":\"JWT\"}";

int create_jwt(char *project_id, const unsigned char *private_key, char *output,
               size_t output_len, size_t *token_len) {
  int ret;
  char buffer[400];
  jwt_signature jwt;
  size_t buflen = 400;
  size_t header_len = 0;
  size_t payload_len = 0;

  //snprintf(buffer);

  ret = mbedtls_base64_encode(buffer, buflen, &header_len, header,
                              sizeof(header) - 1);
  if (ret != 0) {
    printk("error encoding header");
    return ret;
  }

  buffer[header_len] = '.';

  ret = mbedtls_base64_encode(&buffer[header_len + 1], buflen - header_len,
                              &payload_len, payload, sizeof(payload) - 1);
  if (ret != 0) {
    printk("error encoding payload %d", ret);
    return ret;
  }

  buffer[header_len + payload_len] = '.';

  ret = sign_rs256(buffer, header_len + payload_len, private_key, &jwt);
  if (ret != 0) {
    printk("signing failed %d", ret);
    return ret;
  }

  ret = mbedtls_base64_encode(&buffer[header_len + payload_len + 1], buflen,
                              &payload_len, jwt.sig, jwt.length);
  if (ret != 0) {
    printk("error base64 %d\n", ret);
  }
  printk("signature %s\n", buffer);
  return ret;
}

static int sign_rs256(char *payload, size_t payload_len,
                      const unsigned char *priv_key, jwt_signature *jwt) {
  int ret = 0;
  unsigned char hash[32];

  mbedtls_rsa_context *rsa = NULL;
  mbedtls_pk_context pk_ctx;
  mbedtls_sha256_context sha256;

  /* create hash */
  mbedtls_sha256(payload, payload_len, hash, 0);
  mbedtls_sha256_free(&sha256);

  /* read/parse private key */
  mbedtls_pk_init(&pk_ctx);

  if ((ret = mbedtls_pk_parse_key(&pk_ctx, priv_key, strlen(priv_key) + 1, NULL,
                                  0)) != 0) {
    printk("parsing failed %d", ret);
    goto exit;
  }

  rsa = mbedtls_pk_rsa(pk_ctx);

  if ((ret = mbedtls_rsa_check_privkey(rsa)) != 0) {
    printk("keycheck failed %d", ret);
    goto exit;
  }

  jwt->length = rsa->len;
  memset(jwt->sig, 0, (jwt->length + 1));
  if ((ret = mbedtls_rsa_rsassa_pkcs1_v15_sign(
           rsa, NULL, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, 32, hash,
           jwt->sig)) != 0) {
    printk("signing failed %d", ret);
  }
exit:
  mbedtls_pk_free(&pk_ctx);
  mbedtls_rsa_free(rsa);
  return ret;
}
