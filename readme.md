# Base64

- `void*` byte stream to `char*` base64 string and viceversa
- Single header file library
- Parallel encoding/decoding using OpenPM 
- Easy to use API: no initializations, no unneded stuff
- $O(n)$ time and $O(n)$ space
- Compatible with >= C99

```c
char *b64_enc(const void *data, size_t size, size_t *new_size);
void *b64_dec(const char *data, size_t size, size_t *new_size);
```

# Sha256

No implemented
