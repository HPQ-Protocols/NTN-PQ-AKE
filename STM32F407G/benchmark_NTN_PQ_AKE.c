uint32_t benchmark_NTN_PQ_AKE_v40(void)
{
	// volatile uint32_t start;
	// volatile uint32_t end;
	uint32_t start, end;
    uint32_t cycles = 0;

    /* ==========================================================
       Long-term PSK
       ========================================================== */
    uint8_t psk[32];
    memset(psk, 0x11, sizeof(psk));

    /* ==========================================================
       Alice
       ========================================================== */
    uint8_t pkA[CRYPTO_PUBLICKEYBYTES];
    uint8_t skA[CRYPTO_SECRETKEYBYTES];

    uint8_t nonceA[16];
    uint8_t nonceB[16];

    uint32_t ctrA = 1;
    uint32_t ctrB = 1;

    /* ==========================================================
       Bob
       ========================================================== */
    uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
    uint8_t ssB[CRYPTO_BYTES];

    /* ==========================================================
       Alice
       ========================================================== */
    uint8_t ssA[CRYPTO_BYTES];

    /* ==========================================================
       Buffers
       ========================================================== */
    uint8_t session_ctx[32];

    uint8_t tokenA[32];

    uint8_t Ktemp_B[32];
    uint8_t Ktemp_A[32];

    uint8_t confirmB[32];
    uint8_t confirmA[32];

    uint8_t session_ready[32];

    uint8_t Ksession_A[32];
    uint8_t Ksession_B[32];

    uint8_t transcript[96];

    size_t offset;

    /************************************************************
     * M1 : Alice -> Bob
     ************************************************************/
    start = DWT->CYCCNT;

    randombytes(nonceA, sizeof(nonceA));

    crypto_kem_keypair(pkA, skA);

    hmac_sha3_256(
        tokenA,
        psk,
        sizeof(psk),
        pkA,
        CRYPTO_PUBLICKEYBYTES);

    end = DWT->CYCCNT;
    cycles += (end - start);

    /************************************************************
     * M2 : Bob -> Alice
     ************************************************************/
    start = DWT->CYCCNT;

    randombytes(nonceB, sizeof(nonceB));

    crypto_kem_enc(
        ct,
        ssB,
        pkA);

    offset = 0;

    memcpy(session_ctx + offset, nonceA, 16);
    offset += 16;

    memcpy(session_ctx + offset, nonceB, 16);
    offset += 16;

    sha3_256(
        session_ctx,
        session_ctx,
        offset);

    hkdf_sha3(
        Ktemp_B,
        ssB,
        CRYPTO_BYTES,
        psk,
        sizeof(psk));

    hmac_sha3_256(
        confirmB,
        Ktemp_B,
        32,
        session_ctx,
        32);

    end = DWT->CYCCNT;
    cycles += (end - start);

    /************************************************************
     * M3 : Alice -> Bob
     ************************************************************/
    start = DWT->CYCCNT;

    crypto_kem_dec(
        ssA,
        ct,
        skA);

    hkdf_sha3(
        Ktemp_A,
        ssA,
        CRYPTO_BYTES,
        psk,
        sizeof(psk));

    uint8_t confirmB_check[32];

    hmac_sha3_256(
        confirmB_check,
        Ktemp_A,
        32,
        session_ctx,
        32);

    if (memcmp(confirmB, confirmB_check, 32) != 0)
    {
        memset(skA,0,sizeof(skA));
        memset(ssA,0,sizeof(ssA));
        memset(ssB,0,sizeof(ssB));
        memset(Ktemp_A,0,sizeof(Ktemp_A));
        memset(Ktemp_B,0,sizeof(Ktemp_B));
        return 0;
    }

    hmac_sha3_256(
        confirmA,
        Ktemp_A,
        32,
        confirmB,
        32);

    memset(skA,0,sizeof(skA));

    end = DWT->CYCCNT;
    cycles += (end - start);

    /************************************************************
     * M4 : Bob Final ACK
     ************************************************************/
    start = DWT->CYCCNT;

    uint8_t confirmA_check[32];

    hmac_sha3_256(
        confirmA_check,
        Ktemp_B,
        32,
        confirmB,
        32);

    if (memcmp(confirmA, confirmA_check, 32) != 0)
    {
        memset(ssA,0,sizeof(ssA));
        memset(ssB,0,sizeof(ssB));
        memset(Ktemp_A,0,sizeof(Ktemp_A));
        memset(Ktemp_B,0,sizeof(Ktemp_B));
        return 0;
    }

    hmac_sha3_256(
        session_ready,
        Ktemp_B,
        32,
        confirmA,
        32);

    end = DWT->CYCCNT;
    cycles += (end - start);

    /************************************************************
     * Final Session Key Derivation
     ************************************************************/
    start = DWT->CYCCNT;

    offset = 0;

    memcpy(transcript + offset, tokenA, 32);
    offset += 32;

    memcpy(transcript + offset, confirmB, 32);
    offset += 32;

    memcpy(transcript + offset, confirmA, 32);
    offset += 32;

    sha3_256(
        transcript,
        transcript,
        offset);

    hkdf_sha3(
        Ksession_A,
        Ktemp_A,
        32,
        transcript,
        32);

    hkdf_sha3(
        Ksession_B,
        Ktemp_B,
        32,
        transcript,
        32);

    end = DWT->CYCCNT;
    cycles += (end - start);

    /************************************************************
     * Secure Cleanup
     ************************************************************/
    memset(skA,0,sizeof(skA));

    memset(ssA,0,sizeof(ssA));
    memset(ssB,0,sizeof(ssB));

    memset(Ktemp_A,0,sizeof(Ktemp_A));
    memset(Ktemp_B,0,sizeof(Ktemp_B));

    memset(session_ctx,0,sizeof(session_ctx));

    memset(confirmA,0,sizeof(confirmA));
    memset(confirmB,0,sizeof(confirmB));

    memset(session_ready,0,sizeof(session_ready));

    memset(Ksession_A,0,sizeof(Ksession_A));
    memset(Ksession_B,0,sizeof(Ksession_B));

    memset(transcript,0,sizeof(transcript));

    (void)ctrA;
    (void)ctrB;

    return cycles;
}