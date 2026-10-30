#include "Includes/Helper.h"

void Toast(JNIEnv *env, jobject thiz, const char *text, int length) {
    jstring jstr = env->NewStringUTF(text);
    jclass toast = env->FindClass(OBFUSCATE("android/widget/Toast"));
    jmethodID methodMakeText =env->GetStaticMethodID(toast,OBFUSCATE("makeText"),OBFUSCATE("(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;"));
    jobject toastobj = env->CallStaticObjectMethod(toast, methodMakeText,thiz, jstr, length);
    jmethodID methodShow = env->GetMethodID(toast, OBFUSCATE("show"), OBFUSCATE("()V"));
    env->CallVoidMethod(toastobj, methodShow);
}

void setText(JNIEnv *env, jobject obj, const char* text){
    //https://stackoverflow.com/a/33627640/3763113
    //A little JNI calls here. You really really need a great knowledge if you want to play with JNI stuff
    //Html.fromHtml("");
    jclass html = (*env).FindClass(OBFUSCATE("android/text/Html"));
    jmethodID fromHtml = (*env).GetStaticMethodID(html, OBFUSCATE("fromHtml"), OBFUSCATE("(Ljava/lang/String;)Landroid/text/Spanned;"));

    //setText("");
    jclass textView = (*env).FindClass(OBFUSCATE("android/widget/TextView"));
    jmethodID setText = (*env).GetMethodID(textView, OBFUSCATE("setText"), OBFUSCATE("(Ljava/lang/CharSequence;)V"));

    //Java string
    jstring jstr = (*env).NewStringUTF(text);
    (*env).CallVoidMethod(obj, setText,  (*env).CallStaticObjectMethod(html, fromHtml, jstr));
}

jobjectArray GetFeatureList(JNIEnv *env, jobject thiz, jobject ctx, jobject title, jobject subtitle) {
	setText(env, title, OBFUSCATE("<b>Aadil Mods</b>"));

    //Set sub title
    setText(env, subtitle, OBFUSCATE("<b>Modded by Aadil Mods | https://github.com/aadilmodsofficial</b>"));
    //Toast Example
    Toast(env, ctx, OBFUSCATE("Modded by Aadil Mods"), 0); //0 - Long, 1 - Short
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Mods"), //Not counted
            OBFUSCATE("5_Toggle_No death"),
            OBFUSCATE("10_Button_Start Invcibility (30 sec duration)"),
            OBFUSCATE("15_SeekBar_Score multiplier_1_100"),
            OBFUSCATE("20_SeekBar_Coins multiplier_1_1000"),
            OBFUSCATE("Category_Examples"), //Not counted
            OBFUSCATE("Toggle_The toggle"),
            OBFUSCATE(
                    "100_Toggle_True_The toggle 2"), //This one have feature number assigned, and switched on by default
            OBFUSCATE("110_Toggle_The toggle 3"), //This one too
            OBFUSCATE("SeekBar_The slider_1_100"),
            OBFUSCATE("SeekBar_Kittymemory slider example_1_5"),
            OBFUSCATE("Spinner_The spinner_Items 1,Items 2,Items 3"),
            OBFUSCATE("Button_The button"),
            OBFUSCATE("ButtonLink_The button with link_https://www.youtube.com/"), //Not counted
            OBFUSCATE("ButtonOnOff_The On/Off button"),
            OBFUSCATE("CheckBox_The Check Box"),
            OBFUSCATE("InputValue_Input number"),
            OBFUSCATE("InputValue_1000_Input number 2"), //Max value
			OBFUSCATE("1111_InputLValue_Input long number"),
            OBFUSCATE("InputLValue_1000000000000_Input long number 2"), //Max value
            OBFUSCATE("InputText_Input text"),
            OBFUSCATE("RadioButton_Radio buttons_OFF,Mod 1,Mod 2,Mod 3"),

            //Create new collapse
            OBFUSCATE("Collapse_Collapse 1"),
            OBFUSCATE("CollapseAdd_Toggle_The toggle"),
            OBFUSCATE("CollapseAdd_Toggle_The toggle"),
            OBFUSCATE("123_CollapseAdd_Toggle_The toggle"),
            OBFUSCATE("122_CollapseAdd_CheckBox_Check box"),
            OBFUSCATE("CollapseAdd_Button_The button"),

            //Create new collapse again
            OBFUSCATE("Collapse_Collapse 2_True"),
            OBFUSCATE("CollapseAdd_SeekBar_The slider_1_100"),
            OBFUSCATE("CollapseAdd_InputValue_Input number"),

			
            OBFUSCATE("Category_Settings"),
            OBFUSCATE("-3_Toggle_Auto size vertically"),
            OBFUSCATE("-4_Button_Hide Menu"),
            OBFUSCATE("RichTextView_This is text view, not fully HTML."
                      "<b>Bold</b> <i>italic</i> <u>underline</u>"
                      "<br />New line <font color='red'>Support colors</font>"
                      "<br/><big>bigger Text</big>"),
            OBFUSCATE("RichWebView_<html><head><style>body{color: white;}</style></head><body>"
                      "This is WebView, with REAL HTML support!"
                      "<div style=\"background-color: darkblue; text-align: center;\">Support CSS</div>"
                      "<marquee style=\"color: green; font-weight:bold;\" direction=\"left\" scrollamount=\"5\" behavior=\"scroll\">This is <u>scrollable</u> text</marquee>"
                      "</body></html>"),
    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

jstring Icon(JNIEnv *env, jobject thiz) {

    //Use https://www.base64encode.org/ to encode your image to base64
    return env->NewStringUTF(
            OBFUSCATE("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"));
}
