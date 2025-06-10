project "VoxelGame"
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
        rootdir .. "Vendor/stb/stb/stb_image.h",

        -- The ImGui premake file did not want to work so here we are
        rootdir .. "Vendor/ImGui/ImGui/*.h",
        rootdir .. "Vendor/ImGui/ImGui/*.cpp",
        rootdir .. "Vendor/ImGui/ImGui/misc/cpp/*.h",
        rootdir .. "Vendor/ImGui/ImGui/misc/cpp/*.cpp",

        rootdir .. "Vendor/ImGui/ImGui/backends/imgui_impl_glfw.h",
        rootdir .. "Vendor/ImGui/ImGui/backends/imgui_impl_glfw.cpp",
        rootdir .. "Vendor/ImGui/ImGui/backends/imgui_impl_opengl3.cpp",
        rootdir .. "Vendor/ImGui/ImGui/backends/imgui_impl_opengl3.cpp",
    }

    includedirs {
        "src",
        rootdir .. "Vendor/GLFW/GLFW/include",
        rootdir .. "Vendor/GLAD/GLAD/include",
        rootdir .. "Vendor/GLM/GLM",
        rootdir .. "Vendor/stb",
        rootdir .. "Vendor/ImGui/ImGui"
    }

    defines {
        "STB_IMAGE_IMPLEMENTATION",
        "GLM_ENABLE_EXPERIMENTAL"
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
