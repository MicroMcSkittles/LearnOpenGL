workspace "LearnOpenGLGame"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "Game"

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
include "Vendor/GLAD"
include "Vendor/GLFW"
include "Vendor/GLM"
--include "Vendor/ImGui"
include "Vendor/stb"

include "Game" -- Main Project