// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#ifndef PCH_H
#define PCH_H

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include "framework.h"
#include "MinHook/include/MinHook.h"
#include "Sig.hpp"
#include <Psapi.h>
#include "signatures.h"
#include "thread"
#include "SDK.h"
#include <unordered_map>
#include <string>
#include "CBaseCombatCharacter.h"
#include "Netvars.h"
#include <cstdint>
#include "CDotaPlayer.h"
#include "IInputSystem.h"

#endif //PCH_H
