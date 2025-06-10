workspace "LearnOpenGLGame"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
include "Vendor/GLAD"
include "Vendor/GLFW"
include "Vendor/GLM"
--include "Vendor/ImGui"
include "Vendor/stb"

-- Only uncomment the project you want to use/work on

-- include "DoomWADLoader" -- Doom WAD Loader Project
include "VoxelGame" -- Voxel Game Project
-- include "Game" -- 2D Game Project