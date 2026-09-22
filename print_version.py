Import("env")
import re
import os
import shutil

def get_version(env):
    build_flags = env.get("BUILD_FLAGS", [])
    version = "unknown"
    for flag in build_flags:
        if "APP_VERSION" in flag:
            raw = flag.split("=", 1)[-1]
            version = re.sub(r'[\'"\\]', '', raw).strip()
    return version

def print_version_and_copy(source, target, env):
    version = get_version(env)

    print("==== GearIndicator firmware build successfull =====")
    print("===================================================")
    print(f"           FIRMWARE VERSION: {version}")
    print("===================================================")
    print("***************************************************")

    version_suffix = version.replace(".", "_")

    build_dir = env.subst("$BUILD_DIR")
    progname = env.subst("$PROGNAME")
    project_dir = env.subst("$PROJECT_DIR")

    elf_src = os.path.join(build_dir, f"{progname}.elf")
    hex_src = os.path.join(build_dir, f"{progname}.hex")

    elf_dst = os.path.join(project_dir, f"firmware_v{version_suffix}.elf")
    hex_dst = os.path.join(project_dir, f"firmware_v{version_suffix}.hex")

    for src, dst in [(elf_src, elf_dst), (hex_src, hex_dst)]:
        if os.path.exists(src):
            shutil.copy(src, dst)
           
# Spustí sa LEN ak sa .hex úspešne vygeneruje (t.j. build prešiel bez chýb)
env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", print_version_and_copy)