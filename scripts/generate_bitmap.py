import os
import re
import glob
import bisect
from collections import defaultdict

Import("env")

def parse_hex_bytes(text):
    return [int(m, 16) for m in re.findall(r'0x[0-9a-fA-F]{1,2}', text)]

def compress_legacy_lzss(data_bytes):
    out = bytearray()
    i = 0
    n = len(data_bytes)

    while i < n:
        run_len = 1
        while i + run_len < n and run_len < 128 and data_bytes[i + run_len] == data_bytes[i]:
            run_len += 1

        if run_len >= 3:
            out.append(run_len - 1)
            out.append(data_bytes[i])
            i += run_len
        else:
            lit_start = i
            lit_len = 0
            while i < n and lit_len < 128:
                if i + 2 < n and data_bytes[i] == data_bytes[i + 1] == data_bytes[i + 2]:
                    break
                lit_len += 1
                i += 1
            if lit_len > 0:
                out.append(0x80 | (lit_len - 1))
                out.extend(data_bytes[lit_start: lit_start + lit_len])

    return out

def _build_hash_chains(data):
    chains = defaultdict(list)
    n = len(data)
    for pos in range(max(0, n - 2)):
        chains[bytes(data[pos:pos + 3])].append(pos)
    return chains

def _find_best_match(i, data, chains, n):
    if i + 3 > n:
        return None
    key = bytes(data[i:i + 3])
    positions = chains.get(key)
    if not positions:
        return None

    idx = bisect.bisect_left(positions, i) - 1
    if idx < 0:
        return None

    limit = min(n, i + 273)
    max_possible = limit - i
    best_len = 0
    best_dist = 0
    tried = 0

    while idx >= 0 and tried < 512:
        j = positions[idx]
        dist = i - j
        if dist > 4096:
            break
        l = 0
        while l < max_possible and data[j + l] == data[i + l]:
            l += 1
        if l > best_len:
            best_len = l
            best_dist = dist
            if best_len >= 273:
                break
        idx -= 1
        tried += 1

    if best_len >= 3:
        return (best_dist, best_len)
    return None
 
def compress_lzss(data):
    n = len(data)
    if n == 0:
        return bytearray()

    chains = _build_hash_chains(data)
    cache = {}

    def get_match(pos):
        if pos not in cache:
            cache[pos] = _find_best_match(pos, data, chains, n)
        return cache[pos]

    tokens = []
    i = 0
    while i < n:
        m = get_match(i)
        if m:
            dist, length = m
            if i + 1 < n:
                m2 = get_match(i + 1)
                if m2 and m2[1] > length:
                    tokens.append(('lit', data[i]))
                    i += 1
                    continue
            tokens.append(('match', dist, length))
            i += length
        else:
            tokens.append(('lit', data[i]))
            i += 1

    out = bytearray()
    idx = 0
    while idx < len(tokens):
        chunk = tokens[idx: idx + 8]
        control = 0
        payload = bytearray()
        for bit, tok in enumerate(chunk):
            if tok[0] == 'lit':
                control |= (1 << bit)
                payload.append(tok[1])
            else:
                _, dist, length = tok
                offset_val = dist - 1
                if length <= 17:
                    length_code = length - 3
                    payload.append(offset_val & 0xFF)
                    payload.append(((offset_val >> 8) & 0x0F) << 4 | length_code)
                else:
                    extra = length - 18
                    payload.append(offset_val & 0xFF)
                    payload.append(((offset_val >> 8) & 0x0F) << 4 | 0x0F)
                    payload.append(extra & 0xFF)
        out.append(control)
        out.extend(payload)
        idx += 8

    return out

def compress(data_bytes):
    orig_len = len(data_bytes)
    header = bytearray([orig_len & 0xFF, (orig_len >> 8) & 0xFF])
    if orig_len == 0:
        return header + bytearray([1])

    legacy = compress_legacy_lzss(data_bytes)
    lzss = compress_lzss(data_bytes)

    if len(lzss) <= len(legacy):
        return header + bytearray([1]) + lzss
    else:
        return header + bytearray([0]) + legacy

def generate_bitmap_header():
    project_dir = env.get("PROJECT_DIR")
    pio_env = env.get("PIOENV")
    bitmap_dir = os.path.join(project_dir, "bitmap")
    output_header = os.path.join(project_dir, "include", pio_env, "bitmap.h")

    if not os.path.exists(bitmap_dir):
        print(f"Warning: '{bitmap_dir}' directory not found.")
        return

    header_content = (
        "// AUTO-GENERATED FILE\n"
        "#ifndef BITMAP_H\n"
        "#define BITMAP_H\n\n"
        "#include <Arduino.h>\n"
        '#include "config.h"\n\n'
        "#ifndef LZSS_BUF_SIZE\n"
        "#define LZSS_BUF_SIZE 1024\n"
        "#endif\n\n"
        "inline void decodeLZSS(const uint8_t* lzss_data, uint8_t* dest) {\n"
        "    uint16_t orig_len = pgm_read_byte(&lzss_data[0]) | ((uint16_t)pgm_read_byte(&lzss_data[1]) << 8);\n"
        "    uint8_t mode = pgm_read_byte(&lzss_data[2]);\n"
        "    uint16_t out_idx = 0;\n"
        "    uint16_t in_idx = 3;\n\n"
        "    if (mode == 0) {\n"
        "        while (out_idx < orig_len) {\n"
        "            uint8_t header = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "            if (header & 0x80) {\n"
        "                uint8_t count = (header & 0x7F) + 1;\n"
        "                while (count-- > 0 && out_idx < orig_len) {\n"
        "                    dest[out_idx++] = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "                }\n"
        "            } else {\n"
        "                uint8_t count = header + 1;\n"
        "                uint8_t val = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "                while (count-- > 0 && out_idx < orig_len) {\n"
        "                    dest[out_idx++] = val;\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    } else {\n"
        "        while (out_idx < orig_len) {\n"
        "            uint8_t control = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "            for (uint8_t bit = 0; bit < 8 && out_idx < orig_len; bit++) {\n"
        "                if (control & (1 << bit)) {\n"
        "                    dest[out_idx++] = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "                } else {\n"
        "                    uint8_t b0 = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "                    uint8_t b1 = pgm_read_byte(&lzss_data[in_idx++]);\n"
        "                    uint16_t offset = (uint16_t)b0 | ((uint16_t)(b1 & 0xF0) << 4);\n"
        "                    uint8_t len_code = b1 & 0x0F;\n"
        "                    uint16_t length;\n"
        "                    if (len_code == 0x0F) {\n"
        "                        length = 18 + pgm_read_byte(&lzss_data[in_idx++]);\n"
        "                    } else {\n"
        "                        length = len_code + 3;\n"
        "                    }\n"
        "                    uint16_t match_pos = out_idx - offset - 1;\n"
        "                    for (uint16_t k = 0; k < length && out_idx < orig_len; k++) {\n"
        "                        dest[out_idx] = dest[match_pos + k];\n"
        "                        out_idx++;\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n\n"
        "inline const uint8_t* decodeLZSS(const uint8_t* lzss_data) {\n"
        "    static uint8_t lzss_decode_buf[LZSS_BUF_SIZE];\n"
        "    decodeLZSS(lzss_data, lzss_decode_buf);\n"
        "    return lzss_decode_buf;\n"
        "}\n\n"
    )

    bitmap_files = sorted(glob.glob(os.path.join(bitmap_dir, "**", "*.*"), recursive=True))

    for file_path in bitmap_files:
        if not os.path.isfile(file_path):
            continue

        rel_path = os.path.relpath(file_path, bitmap_dir)
        folder_name = os.path.dirname(rel_path)
        filename = os.path.basename(file_path)
        file_stem = os.path.splitext(filename)[0]

        if folder_name:
            var_name = f"bitmap_{file_stem}_{folder_name}".replace(".", "_").replace("-", "_").replace(os.sep, "_")
        else:
            var_name = f"bitmap_{file_stem}".replace(".", "_").replace("-", "_")

        with open(file_path, "r", encoding="utf-8") as f:
            raw_bytes = parse_hex_bytes(f.read())

        if not raw_bytes:
            continue

        compressed = compress(raw_bytes)

        header_content += f"const unsigned char {var_name}[] PROGMEM = {{\n"

        hex_lines = []
        for i in range(0, len(compressed), 12):
            chunk = compressed[i:i+12]
            hex_lines.append("    " + ", ".join(f"0x{b:02x}" for b in chunk))

        header_content += ",\n".join(hex_lines) + "\n};\n\n"

    header_content += "#endif\n"

    if os.path.exists(output_header):
        with open(output_header, "r", encoding="utf-8") as f:
            if f.read() == header_content:
                return

    os.makedirs(os.path.dirname(output_header), exist_ok=True)
    with open(output_header, "w", encoding="utf-8") as f:
        f.write(header_content)
        print(f"Successfully generated: {output_header}")

generate_bitmap_header()