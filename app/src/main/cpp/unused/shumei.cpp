//void *(*psub_3EE48)(void *data, int len, void *encode, int encodeLen);
//
//void *sub_3EE48(void *data, int len, void *encode, int encodeLen) {
//    LOGI("on hook");
//    LOGI("shumei: %s", data);
//    log2file(xbyl::format_string("shumei: %s", data));
//    return psub_3EE48(data, len, encode, encodeLen);
//}

//shumei抓包
//    if (!jniTrace.init) {
//        WhenSoInitHook("libsmsdk.so", [&](const string &path, void *addr, const string &funcType) {
////            DobbyHook((char *) addr + 0x3EE48, (dobby_dummy_func_t) sub_3EE48,
////                      (dobby_dummy_func_t *) &psub_3EE48);
//            MapsHelper maps;
//            maps.refresh("libsmsdk.so");
//            char *targetBase = (char *) maps.get_module_base("libsmsdk.so");
//            void *targetAddr = targetBase + 0x59AE0;
//            LOGI("will hook decode code, base: %p, %p", targetAddr, *(uint64_t *) targetAddr);
//            DobbyInstrument(targetAddr,
//                            [](void *address, DobbyRegisterContext *ctx) {
//                                LOGI("on hook");
//                                LOGI("shumei: %s", ctx->general.regs.x0);
//                                log2file(xbyl::format_string("shumei: %s\n", ctx->general.regs.x0));
//                            });
//        });
//    }