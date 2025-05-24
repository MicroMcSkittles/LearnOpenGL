workspace "LearnOpenGLGame"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Game" -- Main Project