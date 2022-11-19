#include <planet/asset_manager.hpp>

#include <felspar/exceptions.hpp>

#include <android/log.h>
#include <jni.h>
#include <SDL.h>


namespace {
    jclass jAsset = {};
    jobject jAssetManager = {};
    jmethodID loader = {};
}


extern "C" JNIEXPORT void JNICALL Java_com_kirit_planet_android_Asset_useManager(
        JNIEnv *env, jobject, jclass am) {
    jAsset = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/kirit/planet/android/Asset")));
    jAssetManager = env->NewGlobalRef(am);
    loader = env->GetStaticMethodID(
            jAsset, "loader",
            "(Landroid/content/res/AssetManager;Ljava/lang/String;)[B");
}


std::vector<std::byte> planet::asset_manager::file_data(
        std::filesystem::path const &fn,
        felspar::source_location const &loc) const {
    JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
    if (not jAsset or not jAssetManager) {
        throw felspar::stdexcept::runtime_error{
                "useManager has not been called during application start up"};
    }
    auto const assetfn = std::filesystem::path{"share"} / fn;
    jstring asset{reinterpret_cast<jstring>(
            env->NewLocalRef(env->NewStringUTF(assetfn.c_str())))};
    jobject load_result(
            env->CallStaticObjectMethod(jAsset, loader, jAssetManager, asset));
    jbyteArray *bytes(reinterpret_cast<jbyteArray *>(&load_result));
    if (*bytes == nullptr) {
        throw felspar::stdexcept::runtime_error{
                "Asset could not be loaded from the asset manager\n"
                + fn.native()};
    } else {
        std::size_t const length = env->GetArrayLength(*bytes);
        std::vector<std::byte> buffer(length);
        env->GetByteArrayRegion(
                *bytes, 0, length, reinterpret_cast<jbyte *>(buffer.data()));
        return buffer;
    }
}
