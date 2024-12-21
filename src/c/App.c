#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <jni.h>
#include <stdlib.h>
#include <stdio.h>

void RenderDebugLayout(){
    CLAY(
    CLAY_RECTANGLE({ .color = {255,255,255,255} }),
    CLAY_LAYOUT({
        .sizing = {
            .width = CLAY_SIZING_GROW(),
            .height = CLAY_SIZING_GROW()
        },
        .padding = {x: 4, y: 4}
    })
    ) {
        CLAY(
        CLAY_RECTANGLE({ .color = {255,0,0,255} }),
        CLAY_LAYOUT({
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_GROW()
            },
        })
        ) {}
    }
};

// Global variable to store the Clay Arena
Clay_Arena g_clayArena;
int g_screenWidth;
int g_screenHeight;

// Helper function to create a Java Clay_String object
jobject createJavaClayString(JNIEnv *env, Clay_String clayString) {
    jclass clayStringClass = (*env)->FindClass(env, "ClayJava/Clay_String");
    if (clayStringClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_String\n");
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(env, clayStringClass, "<init>", "(ILjava/lang/String;)V");
    if (constructor == NULL) {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_String constructor\n");
        return NULL;
    }

    jstring stringValue = (*env)->NewStringUTF(env, clayString.chars);
    jobject clayStringObj = (*env)->NewObject(env, clayStringClass, constructor, clayString.length, stringValue);
    (*env)->DeleteLocalRef(env, stringValue);
    return clayStringObj;
}

// Helper function to create a Java Clay_BoundingBox object
jobject createJavaClayBoundingBox(JNIEnv *env, Clay_BoundingBox boundingBox) {
    jclass boundingBoxClass = (*env)->FindClass(env, "ClayJava/Clay_BoundingBox");
    if (boundingBoxClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_BoundingBox\n");
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(env, boundingBoxClass, "<init>", "(FFFF)V");
    if (constructor == NULL) {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_BoundingBox constructor\n");
        return NULL;
    }
    return (*env)->NewObject(env, boundingBoxClass, constructor, boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);
}

// Helper function to create Java Color object
jobject createJavaClayColor(JNIEnv *env, Clay_Color color) {
    jclass colorClass = (*env)->FindClass(env, "ClayJava/Clay_Color");
    if (colorClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_Color\n");
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(env, colorClass, "<init>", "(FFFF)V");
    if (constructor == NULL) {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_Color constructor\n");
        return NULL;
    }
    return (*env)->NewObject(env, colorClass, constructor, color.r, color.g, color.b, color.a);
}

// Helper function to create Java CornerRadius object
jobject createJavaClayCornerRadius(JNIEnv *env, Clay_CornerRadius cornerRadius) {
    jclass cornerRadiusClass = (*env)->FindClass(env, "ClayJava/Clay_CornerRadius");
    if (cornerRadiusClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_CornerRadius\n");
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(env, cornerRadiusClass, "<init>", "(FFFF)V");
    if (constructor == NULL) {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_CornerRadius constructor\n");
        return NULL;
    }
    return (*env)->NewObject(env, cornerRadiusClass, constructor, cornerRadius.topLeft, cornerRadius.topRight, cornerRadius.bottomLeft, cornerRadius.bottomRight);
}

// Helper function to create a Java Clay_RenderCommandType enum
jobject createJavaClayRenderCommandType(JNIEnv *env, Clay_RenderCommandType commandType) {
    jclass enumClass = (*env)->FindClass(env, "ClayJava/Clay_RenderCommandType");
    if (enumClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_RenderCommandType\n");
        return NULL;
    }
    jfieldID enumField;
    switch (commandType) {
        case CLAY_RENDER_COMMAND_TYPE_NONE:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_NONE", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_RECTANGLE", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_BORDER:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_BORDER", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_TEXT:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_TEXT", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_IMAGE:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_IMAGE", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_SCISSOR_START", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_SCISSOR_END", "LClayJava/Clay_RenderCommandType;");
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
            enumField = (*env)->GetStaticFieldID(env, enumClass, "CLAY_RENDER_COMMAND_TYPE_CUSTOM", "LClayJava/Clay_RenderCommandType;");
            break;
        default:
            fprintf(stderr, "Error: Unknown Clay_RenderCommandType\n");
            return NULL;
    }
    if (enumField == NULL) {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_RenderCommandType enum constant\n");
        return NULL;
    }
    return (*env)->GetStaticObjectField(env, enumClass, enumField);
}

// Helper function to create a Java Clay_ElementConfigUnion object (enum + Object approach)
jobject createJavaClayElementConfigUnion(JNIEnv *env, Clay_ElementConfigUnion config, Clay__ElementConfigType configType) {
    jclass configUnionClass = (*env)->FindClass(env, "ClayJava/Clay_ElementConfigUnion");
    if (configUnionClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_ElementConfigUnion\n");
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(env, configUnionClass, "<init>", "(LClayJava/Clay_ElementConfigType;Ljava/lang/Object;)V");
    if (constructor == NULL) {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_ElementConfigUnion constructor with (Clay_ElementConfigType, Object) signature\n");
        return NULL;
    }

    jclass configTypeEnumClass = (*env)->FindClass(env, "ClayJava/Clay_ElementConfigType");
    if (configTypeEnumClass == NULL) {
        fprintf(stderr, "Error: Could not find class ClayJava/Clay_ElementConfigType\n");
        return NULL;
    }

    jobject configObject = NULL;
    jfieldID enumField = NULL;

    switch (configType) {
        case CLAY__ELEMENT_CONFIG_TYPE_RECTANGLE: {
            jclass rectangleConfigClass = (*env)->FindClass(env, "ClayJava/Clay_RectangleElementConfig");
            if (rectangleConfigClass == NULL) { fprintf(stderr, "Error: Could not find class ClayJava/Clay_RectangleElementConfig\n"); return NULL; }
            jmethodID rectConstructor = (*env)->GetMethodID(env, rectangleConfigClass, "<init>", "(LClayJava/Clay_Color;LClayJava/Clay_CornerRadius;)V");
            if (rectConstructor == NULL) { fprintf(stderr, "Error: Could not find ClayJava/Clay_RectangleElementConfig constructor\n"); return NULL; }
            jobject colorObj = createJavaClayColor(env, config.rectangleElementConfig->color);
            jobject cornerRadiusObj = createJavaClayCornerRadius(env, config.rectangleElementConfig->cornerRadius);
            configObject = (*env)->NewObject(env, rectangleConfigClass, rectConstructor, colorObj, cornerRadiusObj);
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_RECTANGLE", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        case CLAY__ELEMENT_CONFIG_TYPE_BORDER_CONTAINER: {
            jclass borderConfigClass = (*env)->FindClass(env, "ClayJava/Clay_BorderElementConfig");
            if (borderConfigClass == NULL) { fprintf(stderr, "Error: Could not find class ClayJava/Clay_BorderElementConfig\n"); return NULL; }
            jmethodID borderConstructor = (*env)->GetMethodID(env, borderConfigClass, "<init>", "(LClayJava/Clay_Border;LClayJava/Clay_Border;LClayJava/Clay_Border;LClayJava/Clay_Border;LClayJava/Clay_Border;LClayJava/Clay_CornerRadius;)V");
            if (borderConstructor == NULL) { fprintf(stderr, "Error: Could not find ClayJava/Clay_BorderElementConfig constructor\n"); return NULL; }
            jclass borderClass = (*env)->FindClass(env, "ClayJava/Clay_Border");
            jmethodID singleBorderConstructor = (*env)->GetMethodID(env, borderClass, "<init>", "(ILClayJava/Clay_Color;)V");
            jobject leftBorder = (*env)->NewObject(env, borderClass, singleBorderConstructor, config.borderElementConfig->left.width, createJavaClayColor(env, config.borderElementConfig->left.color));
            jobject rightBorder = (*env)->NewObject(env, borderClass, singleBorderConstructor, config.borderElementConfig->right.width, createJavaClayColor(env, config.borderElementConfig->right.color));
            jobject topBorder = (*env)->NewObject(env, borderClass, singleBorderConstructor, config.borderElementConfig->top.width, createJavaClayColor(env, config.borderElementConfig->top.color));
            jobject bottomBorder = (*env)->NewObject(env, borderClass, singleBorderConstructor, config.borderElementConfig->bottom.width, createJavaClayColor(env, config.borderElementConfig->bottom.color));
            jobject betweenBorder = (*env)->NewObject(env, borderClass, singleBorderConstructor, config.borderElementConfig->betweenChildren.width, createJavaClayColor(env, config.borderElementConfig->betweenChildren.color));
            jobject cornerRadius = createJavaClayCornerRadius(env, config.borderElementConfig->cornerRadius);
            configObject = (*env)->NewObject(env, borderConfigClass, borderConstructor, leftBorder, rightBorder, topBorder, bottomBorder, betweenBorder, cornerRadius);
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_BORDER_CONTAINER", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        case CLAY__ELEMENT_CONFIG_TYPE_TEXT: {
            jclass textConfigClass = (*env)->FindClass(env, "ClayJava/Clay_TextElementConfig");
            if (textConfigClass == NULL) { fprintf(stderr, "Error: Could not find class ClayJava/Clay_TextElementConfig\n"); return NULL; }
            jmethodID textConstructor = (*env)->GetMethodID(env, textConfigClass, "<init>", "(LClayJava/Clay_Color;IISIIILClayJava/Clay_TextElementConfigWrapMode;)V");
            if (textConstructor == NULL) { fprintf(stderr, "Error: Could not find ClayJava/Clay_TextElementConfig constructor\n"); return NULL; }
            jobject textColorObj = createJavaClayColor(env, config.textElementConfig->textColor);
            jclass textWrapModeClass = (*env)->FindClass(env, "ClayJava/Clay_TextElementConfigWrapMode");
            jfieldID wrapModeField;
            jobject wrapModeObj = NULL;
            switch (config.textElementConfig->wrapMode) {
                case CLAY_TEXT_WRAP_WORDS: wrapModeField = (*env)->GetStaticFieldID(env, textWrapModeClass, "CLAY_TEXT_WRAP_WORDS", "LClayJava/Clay_TextElementConfigWrapMode;"); break;
                case CLAY_TEXT_WRAP_NEWLINES: wrapModeField = (*env)->GetStaticFieldID(env, textWrapModeClass, "CLAY_TEXT_WRAP_NEWLINES", "LClayJava/Clay_TextElementConfigWrapMode;"); break;
                case CLAY_TEXT_WRAP_NONE: wrapModeField = (*env)->GetStaticFieldID(env, textWrapModeClass, "CLAY_TEXT_WRAP_NONE", "LClayJava/Clay_TextElementConfigWrapMode;"); break;
                default: fprintf(stderr, "Error: Unknown Clay_TextElementConfigWrapMode\n"); return NULL;
            }
            if (wrapModeField) wrapModeObj = (*env)->GetStaticObjectField(env, textWrapModeClass, wrapModeField);
            configObject = (*env)->NewObject(env, textConfigClass, textConstructor, textColorObj, config.textElementConfig->fontId, config.textElementConfig->fontSize, config.textElementConfig->letterSpacing, config.textElementConfig->lineHeight, wrapModeObj);
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_TEXT", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        case CLAY__ELEMENT_CONFIG_TYPE_IMAGE: {
            jclass imageConfigClass = (*env)->FindClass(env, "ClayJava/Clay_ImageElementConfig");
            if (imageConfigClass == NULL) { fprintf(stderr, "Error: Could not find class ClayJava/Clay_ImageElementConfig\n"); return NULL; }
            jmethodID imageConstructor = (*env)->GetMethodID(env, imageConfigClass, "<init>", "(LClayJava/Clay_Dimensions;)V");
            if (imageConstructor == NULL) { fprintf(stderr, "Error: Could not find ClayJava/Clay_ImageElementConfig constructor\n"); return NULL; }
            jclass dimensionsClass = (*env)->FindClass(env, "ClayJava/Clay_Dimensions");
            jmethodID dimensionsConstructor = (*env)->GetMethodID(env, dimensionsClass, "<init>", "(FF)V");
            jobject dimensionsObj = (*env)->NewObject(env, dimensionsClass, dimensionsConstructor, config.imageElementConfig->sourceDimensions.width, config.imageElementConfig->sourceDimensions.height);
            configObject = (*env)->NewObject(env, imageConfigClass, imageConstructor, dimensionsObj);
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_IMAGE", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        case CLAY__ELEMENT_CONFIG_TYPE_FLOATING_CONTAINER: {
            // Implement similar logic for FloatingContainer
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_FLOATING_CONTAINER", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        case CLAY__ELEMENT_CONFIG_TYPE_SCROLL_CONTAINER: {
            // Implement similar logic for ScrollContainer
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_SCROLL_CONTAINER", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        case CLAY__ELEMENT_CONFIG_TYPE_CUSTOM: {
            // Implement similar logic for Custom
            enumField = (*env)->GetStaticFieldID(env, configTypeEnumClass, "CLAY__ELEMENT_CONFIG_TYPE_CUSTOM", "LClayJava/Clay_ElementConfigType;");
            break;
        }
        default:
            fprintf(stderr, "Error: Unknown Clay__ElementConfigType\n");
            return NULL;
    }

    jobject enumValue = NULL;
    if (enumField) {
        enumValue = (*env)->GetStaticObjectField(env, configTypeEnumClass, enumField);
    } else {
        fprintf(stderr, "Error: Could not find ClayJava/Clay_ElementConfigType enum constant for type %d\n", configType);
        return NULL;
    }

    jobject configUnionObj = (*env)->NewObject(env, configUnionClass, constructor, enumValue, configObject);

    return configUnionObj;
}

// The JNI function for the hello method
JNIEXPORT void JNICALL Java_App_hello(JNIEnv * env, jobject thisObj, jstring name) {
    const char *nativeName = (*env)->GetStringUTFChars(env, name, 0);
    if (nativeName == NULL) {
        return; // Handle if memory allocation failed
    }

    // Print the greeting
    printf("Hello %s!\n", nativeName);

    (*env)->ReleaseStringUTFChars(env, name, nativeName);
}

// JNI function for Clay_Create
JNIEXPORT void JNICALL Java_App_Clay_1Create(JNIEnv * env, jobject thisObj) {
    if (g_screenWidth <= 0 || g_screenHeight <= 0) {
        fprintf(stderr, "Error: Layout dimensions must be set before creating Clay environment.\n");
        return;
    }

    uint64_t totalMemorySize = Clay_MinMemorySize();
    void* memory = malloc(totalMemorySize);
    if (memory == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for Clay Arena.\n");
        return;
    }
    g_clayArena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, memory);
    Clay_Initialize(g_clayArena, (Clay_Dimensions) { g_screenWidth, g_screenHeight });
}

// JNI function for Clay_Destroy
JNIEXPORT void JNICALL Java_App_Clay_1Destroy(JNIEnv * env, jobject thisObj) {
    if (g_clayArena.memory) {
        free(g_clayArena.memory);
        g_clayArena.memory = NULL;
    }
}

// JNI function for setLayoutDimensions
JNIEXPORT void JNICALL Java_App_setLayoutDimensions(JNIEnv * env, jobject thisObj, jint width, jint height) {
    g_screenWidth = width;
    g_screenHeight = height;
    Clay_SetLayoutDimensions((Clay_Dimensions) { width, height });
}

// JNI function for beginLayout
JNIEXPORT void JNICALL Java_App_beginLayout(JNIEnv * env, jobject thisObj) {
    Clay_BeginLayout();
    RenderDebugLayout();
}

// JNI function for endLayout
JNIEXPORT jobject JNICALL Java_App_endLayout(JNIEnv * env, jobject thisObj) {
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    jclass renderCommandArrayClass = (*env)->FindClass(env, "ClayJava/Clay_RenderCommandArray");
    if (renderCommandArrayClass == NULL) { fprintf(stderr, "Error: Could not find class ClayJava/Clay_RenderCommandArray\n"); return NULL; }
    jfieldID capacityField = (*env)->GetFieldID(env, renderCommandArrayClass, "capacity", "I");
    jfieldID lengthField = (*env)->GetFieldID(env, renderCommandArrayClass, "length", "I");
    jfieldID internalArrayField = (*env)->GetFieldID(env, renderCommandArrayClass, "internalArray", "[LClayJava/Clay_RenderCommand;");
    if (capacityField == NULL || lengthField == NULL || internalArrayField == NULL) {
        fprintf(stderr, "Error: Could not find fields in ClayJava/Clay_RenderCommandArray\n");
        return NULL;
    }

    jobject renderCommandArrayObj = (*env)->AllocObject(env, renderCommandArrayClass);
    if (renderCommandArrayObj == NULL) { fprintf(stderr, "Error: Could not allocate ClayJava/Clay_RenderCommandArray\n"); return NULL; }

    (*env)->SetIntField(env, renderCommandArrayObj, capacityField, renderCommands.capacity);
    (*env)->SetIntField(env, renderCommandArrayObj, lengthField, renderCommands.length);

    jclass renderCommandClass = (*env)->FindClass(env, "ClayJava/Clay_RenderCommand");
    if (renderCommandClass == NULL) { fprintf(stderr, "Error: Could not find class ClayJava/Clay_RenderCommand\n"); return NULL; }

    jobjectArray internalArray = (*env)->NewObjectArray(env, renderCommands.length, renderCommandClass, NULL);
    if (internalArray == NULL) { fprintf(stderr, "Error: Could not create jobjectArray for Clay_RenderCommand\n"); return NULL; }

    for (int i = 0; i < renderCommands.length; ++i) {
        Clay_RenderCommand *cmd = &renderCommands.internalArray[i];
        jobject renderCommandObj = (*env)->AllocObject(env, renderCommandClass);
        if (renderCommandObj == NULL) { fprintf(stderr, "Error: Could not allocate ClayJava/Clay_RenderCommand\n"); return NULL; }

        jfieldID boundingBoxField = (*env)->GetFieldID(env, renderCommandClass, "boundingBox", "LClayJava/Clay_BoundingBox;");
        jfieldID configField = (*env)->GetFieldID(env, renderCommandClass, "config", "LClayJava/Clay_ElementConfigUnion;");
        jfieldID textField = (*env)->GetFieldID(env, renderCommandClass, "text", "LClayJava/Clay_String;");
        jfieldID idField = (*env)->GetFieldID(env, renderCommandClass, "id", "I");
        jfieldID commandTypeField = (*env)->GetFieldID(env, renderCommandClass, "commandType", "LClayJava/Clay_RenderCommandType;");
        jfieldID nameField = (*env)->GetFieldID(env, renderCommandClass, "name", "LClayJava/Clay_String;");

        if (!boundingBoxField || !configField || !textField || !idField || !commandTypeField || !nameField) {
            fprintf(stderr, "Error: Could not find fields in ClayJava/Clay_RenderCommand\n");
            return NULL;
        }

        jobject boundingBoxObj = createJavaClayBoundingBox(env, cmd->boundingBox);
        jobject configObj = createJavaClayElementConfigUnion(env, cmd->config, cmd->commandType);
        jobject textObj = createJavaClayString(env, cmd->text);
        jobject nameObj = NULL;
        #ifdef CLAY_DEBUG
                nameObj = createJavaClayString(env, cmd->name);
                jfieldID nameField = (*env)->GetFieldID(env, renderCommandClass, "name", "LClayJava/Clay_String;");
                if(nameObj && nameField) (*env)->SetObjectField(env, renderCommandObj, nameField, nameObj);
        #endif
        jobject commandTypeObj = createJavaClayRenderCommandType(env, cmd->commandType);

        if(boundingBoxObj) (*env)->SetObjectField(env, renderCommandObj, boundingBoxField, boundingBoxObj);
        if(configObj) (*env)->SetObjectField(env, renderCommandObj, configField, configObj);
        if(textObj) (*env)->SetObjectField(env, renderCommandObj, textField, textObj);
        (*env)->SetIntField(env, renderCommandObj, idField, cmd->id);
        if(commandTypeObj) (*env)->SetObjectField(env, renderCommandObj, commandTypeField, commandTypeObj);
        if(nameObj) (*env)->SetObjectField(env, renderCommandObj, nameField, nameObj);

        (*env)->SetObjectArrayElement(env, internalArray, i, renderCommandObj);
        (*env)->DeleteLocalRef(env, renderCommandObj);
        if(boundingBoxObj) (*env)->DeleteLocalRef(env, boundingBoxObj);
        if(configObj) (*env)->DeleteLocalRef(env, configObj);
        if(textObj) (*env)->DeleteLocalRef(env, textObj);
        #ifdef CLAY_DEBUG
                if(nameObj) (*env)->DeleteLocalRef(env, nameObj);
        #endif
        if(commandTypeObj) (*env)->DeleteLocalRef(env, commandTypeObj);
    }

    (*env)->SetObjectField(env, renderCommandArrayObj, internalArrayField, internalArray);
    (*env)->DeleteLocalRef(env, internalArray);

    return renderCommandArrayObj;
}