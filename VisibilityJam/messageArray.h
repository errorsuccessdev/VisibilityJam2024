#pragma once
#include <Windows.h>

typedef struct
{
    UINT id;
    wchar_t* name;
} WindowMessage;

WindowMessage getMessage(UINT msg);