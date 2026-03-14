# Base64

- `void*` byte stream to `char*` base64 string
- Single file library
- Parallel encoding/decoding using OpenPM 
- Easy to use API: no initializations, no unneded stuff
- $O(n)$ time and $O(n)$ space

```c
char *b64_enc(const void *data, size_t size, size_t *new_size);
void *b64_dec(const char *data, size_t size, size_t *new_size);
```

Note: Decoding is not implemented yet

# Sha256

No implemented
