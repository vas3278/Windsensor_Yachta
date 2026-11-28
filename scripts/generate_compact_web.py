#!/usr/bin/env python3
"""Generate compact gzip web resources from the original C++ String builders.

This runs before PlatformIO. It does not modify the original web pages.
It fails closed if a selected source builder gains unrecognized dynamic content.
"""
import ast
import gzip
import re
from pathlib import Path

SOURCE = Path(__file__).resolve().parents[1] / "src"
REGEX = re.compile(r'^\s*(\w+)\s*\+=\s*F\s*\(\s*("(?:\\.|[^"\\])*")\s*\)\s*;\s*(?://.*)?$')


def extract(filename, names, allow_nonce=False):
    data = (SOURCE / filename).read_text(encoding="utf-8")
    data = re.sub(r'/\*.*?\*/', '', data, flags=re.DOTALL)
    parts = {name: [] for name in names}
    for number, line in enumerate(data.splitlines(), 1):
        if line.lstrip().startswith("//"):
            continue
        match = REGEX.match(line)
        if match and match.group(1) in parts:
            parts[match.group(1)].append(ast.literal_eval(match.group(2)))
            continue
        if allow_nonce and line.strip() == "content += String(transactionID);":
            parts["content"].append("window.windsensorTransactionID")
            continue
        if re.match(r'^\s*(?:' + '|'.join(names) + r')\s*\+=', line):
            raise ValueError(f"{filename}:{number}: unsupported web-content expression: {line}")
    out = {k: "".join(v).encode("utf-8") for k, v in parts.items()}
    for name, value in out.items():
        if len(value) < 100:
            raise ValueError(f"{filename} {name} is unexpectedly small")
    return out


assets = {}
for name, data in extract("css_html.h", ["blackcontent", "redcontent", "whitecontent"]).items():
    assets["compact_css_" + name.removesuffix("content") + "_gz"] = data
assets["compact_js_gz"] = extract("js_html.h", ["content"])["content"]
assets["compact_md5_gz"] = extract("MD5_html.h", ["content"], allow_nonce=True)["content"]

header = [
    "#pragma once",
    "#include <Arduino.h>",
    "// Auto-generated from the original HTML builders. DO NOT EDIT.",
]
for symbol, plain in assets.items():
    packed = gzip.compress(plain, compresslevel=9, mtime=0)
    if gzip.decompress(packed) != plain:
        raise ValueError("gzip roundtrip failed: " + symbol)
    chunks = [packed[i:i+16] for i in range(0, len(packed), 16)]
    header.append("static const uint8_t " + symbol + "[] PROGMEM = {")
    header.extend("  " + ", ".join("0x%02X" % c for c in chunk) + "," for chunk in chunks)
    header.append("};")
    print(f"{symbol}: {len(plain)} -> {len(packed)} bytes")
(SOURCE / "compact_web_assets.h").write_text("\n".join(header) + "\n", encoding="ascii")
