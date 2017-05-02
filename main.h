#pragma once

#include <memory>

template <typename T>
using shr_ptr = std::shared_ptr<T>;

static Map s_map;
edict_t *g_pEditor;

short g_sprBeam1, g_sprBeam4, g_sprArrow;