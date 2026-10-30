#include <list>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <string>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Menu.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

ElfScanner gtargetElf;

/*
 * Choose how the mod menu should be loaded:
 *
 * true = Standalone APK Mode
 *        The mod menu is loaded directly from the APK itself.
 *        Use this when building a standalone mod menu APK that runs
 *        independently, without Aadil Virtual.
 *
 * false = Aadil Virtual Mode
 *         The mod menu is loaded when the game is launched through
 *         Aadil Virtual.
 *
 * IMPORTANT:
 * This setting must match the launch environment.
 * If set to true, the game will crash when launched through Aadil Virtual.
 * If set to false, the game will crash when launched directly from the APK.
 */
bool LOAD_FROM_APK = true;


bool noDeath;
int scoreMul = 1, coinsMul = 1;

struct MemPatches {
    // let's assume we have patches for these functions for whatever game
    // boolean get_canShoot() function
    MemoryPatch noDeath;
    // etc...
} gPatches;

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/



bool btnPressed = false;

//CharacterPlayer
void (*StartInvcibility)(void *instance, float duration);

void (*old_Update)(void *instance);

void Update(void *instance) {
    if (instance != nullptr) {
        if (btnPressed) {
            StartInvcibility(instance, 30);
            btnPressed = false;
        }
    }
    return old_Update(instance);
}

void (*old_AddScore)(void *instance, int score);
void AddScore(void *instance, int score) {
    return old_AddScore(instance, score * scoreMul);
}

void (*old_AddCoins)(void *instance, int count);
void AddCoins(void *instance, int count) {
    return old_AddCoins(instance, count * coinsMul);
}



// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded

    do {
        sleep(1);
        // getElfBaseMap can also find lib base even if it was loaded from zipped base.apk
        gtargetElf = ElfScanner::createWithPath(targetLibName);
    } while (!gtargetElf.isValid());

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

    //Il2Cpp: Use RVA offset
    StartInvcibility = (void (*)(void *, float)) getAbsoluteAddress(gtargetElf, OBFUSCATE("0x107A3BC"));

    HOOK(getAbsoluteAddress(gtargetElf, OBFUSCATE("0x107A2E0")), AddScore, old_AddScore);
    HOOK(getAbsoluteAddress(gtargetElf, OBFUSCATE("0x107A2FC")), AddCoins, old_AddCoins);
    HOOK(getAbsoluteAddress(gtargetElf, OBFUSCATE("0x1078C44")), Update, old_Update);

    gPatches.noDeath = MemoryPatch::createWithHex(getAbsoluteAddress(gtargetElf, OBFUSCATE("0x1079728")), "C0 03 5F D6");

    PATCH(getAbsoluteAddress(gtargetElf, OBFUSCATE("0x10709AC")), "E05F40B2 C0035FD6");

    LOGI(OBFUSCATE("Done"));
    return nullptr;
}



void Changes(JNIEnv *env, jobject clazz, jobject obj, jint featNum, jboolean boolean, jint value, jlong lvalue, jstring jstr) {

    switch (featNum) {
        case 0:
        {
            if (boolean)
                gPatches.noDeath.Modify();
            else
                gPatches.noDeath.Restore();
            break;
        }
        case 1:
            btnPressed = true;
            break;
        case 2:
            scoreMul = value;
            break;
        case 3:
            coinsMul = value;
            break;
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("GetFeatureList"), OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;IZIJLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/aadil/mods/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

bool registerMenuNatives(JNIEnv* env, jobject dexLoader) {
	jmethodID loadClassMethod = env->GetMethodID(env->GetObjectClass(dexLoader), OBFUSCATE("loadClass"), OBFUSCATE("(Ljava/lang/String;)Ljava/lang/Class;"));
	
	jstring className = env->NewStringUTF(OBFUSCATE("com.aadil.mods.Menu"));
	jobject appClassObj = env->CallObjectMethod(dexLoader, loadClassMethod, className);
	
	if (env->ExceptionCheck() || !appClassObj) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
	
	jclass appClass = (jclass)appClassObj;
	
	JNINativeMethod methods[] = {
		{OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
        {OBFUSCATE("GetFeatureList"), OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
        {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;IZIJLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
	};
	
	jint result = env->RegisterNatives(appClass, methods, sizeof(methods) / sizeof(methods[0]));
	
	env->DeleteLocalRef(className);
	env->DeleteLocalRef(appClassObj);
	
	if (result != 0) return false;
	
	return true;
}

bool loadDexAndStart(JNIEnv* env, jobject context, jobject activity) {
	jobject dexLoader = nullptr;
	
	if (api_level() >= 26) {
		dexLoader = createInMemoryDexLoader(env, context);
	} else {
		dexLoader = createFileDexLoader(env, context);
	}
	
	if (!dexLoader || env->ExceptionCheck()) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
	
	if (!registerMenuNatives(env, dexLoader)) return false;
	
	jmethodID loadClassMethod = env->GetMethodID(env->GetObjectClass(dexLoader), OBFUSCATE("loadClass"), OBFUSCATE("(Ljava/lang/String;)Ljava/lang/Class;"));
	
	jstring className = env->NewStringUTF(OBFUSCATE("com.aadil.mods.Main"));
	jobject loadedClassObj = env->CallObjectMethod(dexLoader, loadClassMethod, className);
	
	if (env->ExceptionCheck() || !loadedClassObj) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
	jclass loadedClass = (jclass)loadedClassObj;
	
	jmethodID startMethod = env->GetStaticMethodID(loadedClass, OBFUSCATE("Start"), OBFUSCATE("(Landroid/content/Context;)V"));
	
	if (!startMethod) {
		env->ExceptionClear();
		return false;
	}
	
	env->CallStaticVoidMethod(loadedClass, startMethod, activity);
	
	env->DeleteLocalRef(className);
	env->DeleteLocalRef(loadedClassObj);
	env->DeleteLocalRef(dexLoader);
	
	return true;
}

void nativeInit(JNIEnv* env, jclass clz, jobject ctx, jobject activity, jstring currentActivity) {
	if (LOAD_FROM_APK) return;
	
	jobject context = ctx ? ctx : activity;
	
	/*
	// For games with multiple activities, check the current game activity before starting the menu.
	
	const char* activityName = env->GetStringUTFChars(currentActivity, nullptr);
	if (strcmp(activityName, "com.unity3d.player.UnityPlayerActivity") == 0) loadDexAndStart(env, context, activity);
	
    env->ReleaseStringUTFChars(currentActivity, activityName);
    */

    // For games with only one activity, call it directly without checking the activity.
    loadDexAndStart(env, context, activity);
}

int RegisterVirtual(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("nativeInit"),  OBFUSCATE("(Landroid/content/Context;Landroid/app/Activity;Ljava/lang/String;)V"), reinterpret_cast<void *>(nativeInit)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/aadil/virtual/helper/AadilGuest"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (LOAD_FROM_APK) {
		if (RegisterMenu(env) != 0) return JNI_ERR;
	} else {
		if (RegisterVirtual(env) != 0) return JNI_ERR;
	}
    return JNI_VERSION_1_6;
}
