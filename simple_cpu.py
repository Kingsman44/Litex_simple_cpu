#!/usr/bin/env python3

#
# Simple CPU to Perform Add, Substract, Multiply, Divide.
#
import os
import sys
import argparse

from litex.build.tools import replace_in_file

def main():
    parser = argparse.ArgumentParser(description="LiteX Simple CPU")
    parser.add_argument("--build-path",                      help="Target's build path (ex build/board_name).", required=True)
    parser.add_argument("--with-cxx",   action="store_true", help="Enable CXX support.")
    parser.add_argument("--mem",        default="main_ram",  help="Memory Region where code will be loaded/executed.")
    args = parser.parse_args()

    # Create demo directory
    os.makedirs("simple_cpu", exist_ok=True)

    # Copy contents to demo directory
    os.system(f"cp {os.path.abspath(os.path.dirname(__file__))}/* simple_cpu")
    os.system("chmod -R u+w simple_cpu") # Nix specific: Allow linker script to be modified.

    # Update memory region.
    replace_in_file("simple_cpu/linker.ld", "main_ram", args.mem)

    # Compile demo
    build_path = args.build_path if os.path.isabs(args.build_path) else os.path.join("..", args.build_path)
    os.system(f"export BUILD_DIR={build_path} && {'export WITH_CXX=1 &&' if args.with_cxx else ''} cd simple_cpu && make")

    # Copy demo.bin
    os.system("cp simple_cpu/simple_cpu.bin ./")

    # Prepare flash boot image.
    python3 = sys.executable or "python3" # Nix specific: Reuse current Python executable if available.
    os.system(f"{python3} -m litex.soc.software.crcfbigen simple_cpu.bin -o simple_cpu.fbi --fbi --little") # FIXME: Endianness.

if __name__ == "__main__":
    main()

