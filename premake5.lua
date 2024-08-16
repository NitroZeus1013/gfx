workspace "MyGLFWProject"
    configurations { "Debug", "Release" }
    architecture "x86_64"

project "MyGLFWProject"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**.cpp", "include/**.h" }

    includedirs { "include" }

    libdirs { "lib" }

    links { "glfw3","GLEW" }

    filter "system:linux"
        links  { "X11", "Xrandr", "Xinerama", "Xcursor", "Xi", "dl", "pthread", "GL", "GLU" }

    filter "system:windows"
        links { "opengl32", "gdi32" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    -- Copy assets to output directory
    postbuildcommands {
        "{COPY} assets %{cfg.buildtarget.directory}/assets"
    }