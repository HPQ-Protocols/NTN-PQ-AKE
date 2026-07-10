enum MessageType {
    M1_MSG = 1;
    M2_MSG = 2;
    M3_MSG = 3;
    M4_MSG = 4;
}

packet AKEMessage {
    int type @enum(MessageType);
    int payloadBytes;   // Lưu kích thước phần mật mã thô (Kyber PK, CT, ...)
    int overheadBytes;  // Lưu kích thước phần rác/định dạng (TLS Header, Extension, ...)
}