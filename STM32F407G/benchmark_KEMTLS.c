void benchmark_KEMTLS(void) {
    // Biến cho Ephemeral KEM (Thỏa thuận khóa ngắn hạn)
    static uint8_t ephem_pk[CRYPTO_PUBLICKEYBYTES], ephem_sk[CRYPTO_SECRETKEYBYTES];
    static uint8_t ephem_ct[CRYPTO_CIPHERTEXTBYTES], ephem_ss_server[CRYPTO_BYTES], ephem_ss_client[CRYPTO_BYTES];

    // Biến cho Long-term KEM (Xác thực thực thể bằng KEM - Thay thế Certificate)
    static uint8_t auth_pk[CRYPTO_PUBLICKEYBYTES], auth_sk[CRYPTO_SECRETKEYBYTES];
    static uint8_t auth_ct[CRYPTO_CIPHERTEXTBYTES], auth_ss_server[CRYPTO_BYTES], auth_ss_client[CRYPTO_BYTES];

    // SỬA LỖI 2: Cặp khóa xác thực tĩnh được sinh từ trước (Nằm sẵn trong cấu hình thiết bị)
    crypto_kem_keypair(auth_pk, auth_sk);

    uint32_t start, end;
    start = get_cycles();

    // --- PHA 1: Ephemeral KEM Exchange (Thay thế ECDHE) ---
    crypto_kem_keypair(ephem_pk, ephem_sk);
    crypto_kem_enc(ephem_ct, ephem_ss_server, ephem_pk);
    crypto_kem_dec(ephem_ss_client, ephem_ct, ephem_sk);

    // --- PHA 2: Long-term KEM Auth (Đúng chuẩn KEMTLS) ---
    // SỬA: BỎ HOÀN TOÀN crypto_kem_keypair(auth_pk, auth_sk). Handshake chỉ có Encaps và Decaps
    crypto_kem_enc(auth_ct, auth_ss_client, auth_pk);
    crypto_kem_dec(auth_ss_server, auth_ct, auth_sk);

    end = get_cycles();
    result_kemtls = end - start;
}
