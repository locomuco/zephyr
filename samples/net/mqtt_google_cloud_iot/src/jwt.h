#ifndef _JWT_H_
#define _JWT_H_

int create_jwt(char *project_id, const unsigned char *private_key, char *output, size_t output_len, size_t *token_len);

typedef struct {
	size_t header_len;
	size_t payload_len;
	char sig[400];
	size_t length;
}jwt_signature;

#endif