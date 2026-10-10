import os
import glob
import gzip

Import("env")

def generate_html_header():
    project_dir = env.get("PROJECT_DIR")
    pio_env = env.get("PIOENV")
    html_dir = os.path.join(project_dir, "html")
    output_header = os.path.join(project_dir, "include", pio_env, "html.h")

    if not os.path.exists(html_dir):
        print(f"Warning: '{html_dir}' directory not found.")
        return

    header_content = (
        "// AUTO-GENERATED FILE\n"
        "#ifndef HTML_H\n"
        "#define HTML_H\n\n"
        "#include <pgmspace.h>\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n"
    )

    html_files = sorted(glob.glob(os.path.join(html_dir, "*.*")))
    
    for file_path in html_files:
        filename = os.path.basename(file_path)
        var_name = os.path.splitext(filename)[0].replace(".", "_").replace("-", "_")

        with open(file_path, "rb") as f:
            content = f.read()

        compressed = gzip.compress(content, compresslevel=9)
        
        bytes_str = ", ".join(f"0x{b:02x}" for b in compressed)

        header_content += f'static const uint8_t {var_name}[] PROGMEM = {{ {bytes_str} }};\n'
        header_content += f'static const size_t {var_name}_len = {len(compressed)};\n\n'

    header_content += "#endif\n"

    if os.path.exists(output_header):
        with open(output_header, "r", encoding="utf-8") as f:
            if f.read() == header_content:
                return

    os.makedirs(os.path.dirname(output_header), exist_ok=True)
    with open(output_header, "w", encoding="utf-8") as f:
        f.write(header_content)
        print(f"Successfully generated: {output_header}")

generate_html_header()