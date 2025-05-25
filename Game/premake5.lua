project "Game"
    kind "ConsoleApp"
    language "C++"

-- Output Directories ===============
    rootdir = "../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
        "src/**.c",
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp",
        rootdir .. "Vendor/stb/stb/stb_image.h"
    }

    includedirs {
        "src",
        rootdir .. "Vendor/GLFW/GLFW/include",
        rootdir .. "Vendor/GLAD/GLAD/include",
        rootdir .. "Vendor/GLM/GLM",
        rootdir .. "Vendor/stb"
    }

    defines {
        "STB_IMAGE_IMPLEMENTATION"
    }

-- Link libraries ===================
    links {
        "GLFW",
        "GLAD"
    }

-- Windows ==========================
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS"
        }

-- Configuations ====================
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "RELEASE"
        optimize "On"
