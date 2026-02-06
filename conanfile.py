from conan import ConanFile


class AnimittaConan(ConanFile):
    name = "animitta"
    version = "0.1.0"

    settings = "os", "arch", "compiler", "build_type"
    generators = ("CMakeToolchain", "CMakeDeps")

    # Default to static dependency graph so produced app binaries are easier to distribute.
    default_options = {
        "*:shared": False,
        "*:fPIC": True,
        "libffi/*:shared": True,
        "wayland/*:shared": True,
        "xkbcommon/*:shared": True,
        "sdl/*:alsa": False,
        "sdl/*:pulse": False,
        "sdl/*:jack": False,
        "sdl/*:sndio": False,
        "sdl/*:nas": False,
        "sdl/*:esd": False,
        "sdl/*:arts": False,
        "sdl/*:x11": False,
        "sdl/*:xcursor": False,
        "sdl/*:xinerama": False,
        "sdl/*:xinput": False,
        "sdl/*:xrandr": False,
        "sdl/*:xscrnsaver": False,
        "sdl/*:xshape": False,
        "sdl/*:xvm": False,
        "sdl/*:wayland": True,
        "sdl/*:opengles": False,
        "sdl/*:vulkan": False,
        "sdl/*:libunwind": False,
        "sdl/*:hidapi": False,
        "xkbcommon/*:with_x11": False,
        "xkbcommon/*:with_wayland": True,
    }

    def requirements(self):
        # Keep ranges broad for bootstrap; lockfile can pin exact revisions later.
        self.requires("sdl/[>=2.30 <3]")
        self.requires("sdl_ttf/[>=2.24 <3]")
