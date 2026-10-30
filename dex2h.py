# Classes DEX to C++ Header Converter
#
# This script converts classes.dex into classes_dex.h.
# Android: Install Pydroid 3 (https://play.google.com/store/apps/details?id=ru.iiec.pydroid3),
# open this script and tap Run. Keep classes.dex in the same folder as this script.
#
# Windows PC: Install Python, open CMD, navigate to this script's folder using:
#   cd path\to\folder
# Make sure classes.dex is in the same folder, then run:
#   py dex2c.py
#
# The generated classes_dex.h will be saved in the same folder as the script.
# Optional: You can specify custom input and output paths:
#   py dex2c.py input.dex output.h

import sys, os

src = sys.argv[1] if len(sys.argv) > 1 else "classes.dex"
dst = sys.argv[2] if len(sys.argv) > 2 else "classes_dex.h"
name = "classes_dex"

d = open(src, "rb").read()
lines = []
for i in range(0, len(d), 12):
    lines.append("  " + ", ".join("0x%02x" % x for x in d[i:i+12]))

with open(dst, "w", newline="\n") as f:
    f.write("unsigned char %s[] = {\n" % name)
    f.write(",\n".join(lines))
    f.write("\n};\n")
    f.write("unsigned int %s_len = %d;\n" % (name, len(d)))

print("wrote %s (%d bytes)" % (dst, len(d)))