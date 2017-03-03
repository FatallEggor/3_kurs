void kuzn_Xfunc(uint8_t* block, uint8_t* raund_key);
void kuzn_Cfunc(int i, uint8_t *C);
void kuzn_Ffunc(uint8_t *C, int i, uint8_t[10][16]);
void kuzn_efunc(uint8_t *block, uint8_t[10][16]);
void kuzn_dfunc(uint8_t *block, uint8_t[10][16]);

int kuzn_bl_gen_en(uint8_t *block, FILE *input_f);
int kuzn_bl_gen_de(uint8_t *block, FILE *input_f);
int kuzn_sync_gen(uint8_t *sync, FILE *sync_f);
int kuzn_keys_gen(uint8_t[10][16], FILE *key_f);

int kuzn_simple_mode_en(uint8_t [10][16], FILE *input_f, FILE *output_f);
int kuzn_OFB_mode_en(uint8_t[10][16], uint8_t *sync, FILE *input_f, FILE *output_f);
int kuzn_simple_mode_de(uint8_t [10][16], FILE *input_f, FILE *output_f);
int kuzn_OFB_mode_de(uint8_t[10][16], uint8_t *sync, FILE *input_f, FILE *output_f);
