#include <list>
#include <vector>
#include <cstring>
#include <string>
#include <pthread.h>
#include <jni.h>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <dlfcn.h>
#include "obfuscate.h"
#include "get_device_api_level_inlines.h"
#include "Logger.h"
#include "classes_dex.h"
#include "KittyMemory/MemoryPatch.hpp"
#include "KittyMemory/KittyInclude.hpp"
#include <And64InlineHook/And64InlineHook.hpp>

#define HOOK(absaddress, ptr, orig) A64HookFunction((void *)absaddress, (void *)ptr, (void **)&orig)
#define PATCH(absaddress, hex) patchOffset(absaddress, hex)

using namespace std;

int DEX_VERSION = 1;

uintptr_t getAbsoluteAddress(ElfScanner libelf, string offset) {
    return libelf.base() + static_cast<uintptr_t>(stoull(offset, nullptr, 16));
}

void patchOffset(uintptr_t absaddress, string hexBytes) {
    MemoryPatch patch = MemoryPatch::createWithHex(absaddress, hexBytes);
    if (patch.isValid()) patch.Modify();
}

struct Vector2 {
	float x,y;
	
	Vector2() : x(0), y(0) {}
	Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}
};

struct Vector3 {
	float x,y,z;
	
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}
};

struct Color {
	float r,g,b,a;
	
	Color() : r(0), g(0), b(0), a(0) {}
	Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}
};

// ---------- Jni Dex Loader ----------
bool fileExists(const string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

string getFilesDirPath(JNIEnv* env, jobject context) {
	jclass contextClass = env->GetObjectClass(context);
	jmethodID getFilesDir = env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;");
	jobject filesDir = env->CallObjectMethod(context, getFilesDir);
	
	jclass fileClass = env->FindClass("java/io/File");
	jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
	jstring pathStr = (jstring)env->CallObjectMethod(filesDir, getAbsolutePath);
	
	const char* cPath = env->GetStringUTFChars(pathStr, nullptr);
	string result(cPath);
	env->ReleaseStringUTFChars(pathStr, cPath);
	
	env->DeleteLocalRef(pathStr);
	env->DeleteLocalRef(filesDir);
	env->DeleteLocalRef(fileClass);
	env->DeleteLocalRef(contextClass);
	
	return result;
}

bool writeBytesToFile(const std::string& path, const unsigned char* data,unsigned int len) {
	FILE* f = fopen(path.c_str(), "wb");
	if (!f) return false;
	
	size_t written = fwrite(data, 1, len, f);
	fclose(f);
	return written == len;
}

int readVersionFile(const std::string& path) {
	FILE* f = fopen(path.c_str(), "r");
	if (!f) return -1; // no version file = treat as outdated/missing
	int version = -1;
	fscanf(f, "%d", &version);
	fclose(f);
	return version;
}

bool writeVersionFile(const std::string& path, int version) {
	FILE* f = fopen(path.c_str(), "w");
	if (!f) return false;
	fprintf(f, "%d", version);
	fclose(f);
	return true;
}

static string getCodeCacheDirPath(JNIEnv* env, jobject context) {
	jclass contextClass = env->GetObjectClass(context);
	jmethodID getCodeCacheDir = env->GetMethodID(contextClass, "getCodeCacheDir", "()Ljava/io/File;");
	jobject cacheDir = env->CallObjectMethod(context, getCodeCacheDir);
	
	jclass fileClass = env->FindClass("java/io/File");
	jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
	jstring pathStr = (jstring)env->CallObjectMethod(cacheDir, getAbsolutePath);
	
	const char* cPath = env->GetStringUTFChars(pathStr, nullptr);
	string result(cPath);
	env->ReleaseStringUTFChars(pathStr, cPath);
	
	env->DeleteLocalRef(pathStr);
	env->DeleteLocalRef(cacheDir);
	env->DeleteLocalRef(fileClass);
	env->DeleteLocalRef(contextClass);
	
	return result;
}


// dex loading: two paths
// API 26+: load straight from memory, no file, no versioning needed
// (bytes are always the current build's bytes, nothing to go stale)
jobject getContextClassLoader(JNIEnv* env, jobject context) {
	jclass contextClass = env->GetObjectClass(context);
	jmethodID getClassLoaderMethod = env->GetMethodID(contextClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject loader = env->CallObjectMethod(context, getClassLoaderMethod);
	env->DeleteLocalRef(contextClass);
	return loader;
}

// API 26+
jobject createInMemoryDexLoader(JNIEnv* env, jobject context) {
	jobject byteBuffer = env->NewDirectByteBuffer((void*)classes_dex, classes_dex_len);
	
	jclass dexLoaderClass = env->FindClass("dalvik/system/InMemoryDexClassLoader");
	jmethodID ctor = env->GetMethodID(dexLoaderClass, "<init>",
		"(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
	
	jobject parentClassLoader = getContextClassLoader(env, context);
	
	jobject dexLoader = env->NewObject(dexLoaderClass, ctor, byteBuffer, parentClassLoader);
	
	env->DeleteLocalRef(byteBuffer);
	env->DeleteLocalRef(parentClassLoader);
	env->DeleteLocalRef(dexLoaderClass);
	
	return dexLoader;
}


// API <26
jobject createFileDexLoader(JNIEnv* env, jobject context) {
	string filesDir = getFilesDirPath(env, context);
	string dexPath = filesDir + "/classes.dex";
	string versionPath = filesDir + "/classes.dex.version";
	
	int existingVersion = readVersionFile(versionPath);
	bool needsWrite = !fileExists(dexPath) || existingVersion != DEX_VERSION;
	
	if (needsWrite) {
		if (!writeBytesToFile(dexPath, classes_dex, classes_dex_len)) {
			return nullptr;
		}
		writeVersionFile(versionPath, DEX_VERSION);
	}
	
	string optimizedDir = getCodeCacheDirPath(env, context);
	jclass dexLoaderClass = env->FindClass("dalvik/system/DexClassLoader");
	jmethodID ctor = env->GetMethodID(dexLoaderClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
	
	jstring dexPathStr = env->NewStringUTF(dexPath.c_str());
	jstring optimizedDirStr = env->NewStringUTF(optimizedDir.c_str());
	
	jobject parentClassLoader = getContextClassLoader(env, context);
	
	jobject dexLoader = env->NewObject(dexLoaderClass, ctor, dexPathStr, optimizedDirStr, (jstring)nullptr, parentClassLoader);
	
	env->DeleteLocalRef(dexPathStr);
	env->DeleteLocalRef(optimizedDirStr);
	env->DeleteLocalRef(parentClassLoader);
	env->DeleteLocalRef(dexLoaderClass);
	
	return dexLoader;
}


