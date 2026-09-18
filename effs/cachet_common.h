#ifndef CACHET_COMMON_H
#define CACHET_COMMON_H

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CACHET_ERR_AUTH 401u
#define CACHET_ERR_FORMAT 400u

static char *cachet_strdup(const char *src, size_t n) {
  char *out = (char *)malloc(n + 1);
  memcpy(out, src, n);
  out[n] = '\0';
  return out;
}

static int cachet_b64url_encode(Env e, const unsigned char *in, size_t in_len, char **out, size_t *out_len) {
  size_t b64_len = 4 * ((in_len + 2) / 3);
  char *b64 = (char *)io_mem(malloc(b64_len + 1));
  int written = EVP_EncodeBlock((unsigned char *)b64, in, (int)in_len);
  if (written < 0) {
    free(b64);
    return -1;
  }
  size_t j = 0;
  for (size_t i = 0; i < (size_t)written; i++) {
    if (b64[i] == '+') {
      b64[j++] = '-';
    } else if (b64[i] == '/') {
      b64[j++] = '_';
    } else if (b64[i] != '=') {
      b64[j++] = b64[i];
    }
  }
  b64[j] = '\0';
  *out = b64;
  *out_len = j;
  return 0;
}

static int cachet_b64url_decode(Env e, const char *in, size_t in_len, unsigned char **out, size_t *out_len) {
  char *b64 = (char *)io_mem(malloc(in_len + 4));
  size_t j = 0;
  for (size_t i = 0; i < in_len; i++) {
    if (in[i] == '-') {
      b64[j++] = '+';
    } else if (in[i] == '_') {
      b64[j++] = '/';
    } else {
      b64[j++] = in[i];
    }
  }
  while (j % 4 != 0) {
    b64[j++] = '=';
  }
  b64[j] = '\0';
  size_t cap = (j / 4) * 3;
  unsigned char *buf = (unsigned char *)io_mem(malloc(cap));
  int written = EVP_DecodeBlock(buf, (const unsigned char *)b64, (int)j);
  free(b64);
  if (written < 0) {
    free(buf);
    return -1;
  }
  while (written > 0 && buf[written - 1] == '\0') {
    written--;
  }
  *out = buf;
  *out_len = (size_t)written;
  return 0;
}

static int cachet_hmac_sha256(
  Env e,
  const char *message,
  size_t message_len,
  const char *secret,
  size_t secret_len,
  unsigned char **out,
  size_t *out_len
) {
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digest_len = 0;
  if (HMAC(EVP_sha256(), secret, (int)secret_len, (const unsigned char *)message, message_len, digest, &digest_len) == NULL) {
    return -1;
  }
  *out = (unsigned char *)io_mem(malloc(digest_len));
  memcpy(*out, digest, digest_len);
  *out_len = digest_len;
  return 0;
}

static int cachet_ct_eq(const char *a, const char *b, size_t n) {
  unsigned char diff = 0;
  for (size_t i = 0; i < n; i++) {
    diff |= (unsigned char)(a[i] ^ b[i]);
  }
  return diff == 0;
}

static int cachet_json_string(const char *json, const char *key, char *out, size_t out_sz) {
  char pattern[64];
  snprintf(pattern, sizeof(pattern), "\"%s\":\"", key);
  const char *start = strstr(json, pattern);
  if (start == NULL) {
    out[0] = '\0';
    return 0;
  }
  start += strlen(pattern);
  const char *end = strchr(start, '"');
  if (end == NULL) {
    return -1;
  }
  size_t n = (size_t)(end - start);
  if (n + 1 > out_sz) {
    return -1;
  }
  memcpy(out, start, n);
  out[n] = '\0';
  return 0;
}

static int cachet_json_u32(const char *json, const char *key, uint32_t *out) {
  char pattern[64];
  snprintf(pattern, sizeof(pattern), "\"%s\":", key);
  const char *start = strstr(json, pattern);
  if (start == NULL) {
    *out = 0;
    return 0;
  }
  start += strlen(pattern);
  return sscanf(start, "%u", out) == 1 ? 0 : -1;
}

static int cachet_jwt_parts(const char *token, char **header, char **payload, char **signature) {
  const char *dot1 = strchr(token, '.');
  if (dot1 == NULL) {
    return -1;
  }
  const char *dot2 = strchr(dot1 + 1, '.');
  if (dot2 == NULL) {
    return -1;
  }
  *header = cachet_strdup(token, (size_t)(dot1 - token));
  *payload = cachet_strdup(dot1 + 1, (size_t)(dot2 - dot1 - 1));
  *signature = cachet_strdup(dot2 + 1, strlen(dot2 + 1));
  return 0;
}

#endif
