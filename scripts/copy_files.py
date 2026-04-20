Import("env")
import os
import shutil

def copy_all_binaries(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    project_dir = env.subst("$PROJECT_DIR")
    env_name = env.subst("$PIOENV")

    target_folder = os.path.join(project_dir, "bin", env_name)
    os.makedirs(target_folder, exist_ok=True)

    files_to_copy = [
        "firmware.bin",
        "partitions.bin",
        "bootloader.bin"
    ]

    for filename in files_to_copy:
        src_path = os.path.join(build_dir, filename)
        if os.path.exists(src_path):
            dst_path = os.path.join(target_folder, filename)
            shutil.copy(src_path, dst_path)
            print(f"Copied {filename} -> {dst_path}")
        else:
            print(f"Warning: {filename} not found in {build_dir}")

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copy_all_binaries)