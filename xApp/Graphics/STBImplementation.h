#pragma once

#include "Common/Core.h"
#include "Platform/Platform.h"

// We will only be implementing PNG when we remove STB image
#define STBI_ONLY_PNG
#define STBI_NO_GIF
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "External/STBImage.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "External/STBTrueType.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "External/STBImageWrite.h"

