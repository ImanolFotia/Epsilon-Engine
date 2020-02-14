#pragma once

static bool g_Running = true;
static bool g_Settings = false;
static void endgame() { g_Running = false; }
static void settings() { g_Settings = true; }
