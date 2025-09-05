import os
import re
import ctypes
import tkinter as tk
from tkinter import filedialog

ctypes.windll.shcore.SetProcessDpiAwareness(1)

signatures = {
    lambda d: d.startswith(b"\x89PNG\r\n\x1a\n"): ".png",
    lambda d: d.startswith(b"\xFF\xD8\xFF"): ".jpg",
    lambda d: d.startswith(b"GIF8"): ".gif",
    lambda d: d.startswith(b"BM"): ".bmp",
    lambda d: d.startswith(b"RIFF") and b"WEBP" in d[8:16]: ".webp",
    lambda d: d.startswith(b"<svg") or d.startswith(b"<?xml"): ".svg",
}

def write_image_header(out_stream, data: bytes, image_name: str) -> None:
    out_stream.write(f"unsigned char {image_name}[] = {{")
    for i, byte in enumerate(data):
        if i % 12 == 0:
            out_stream.write("\n  ")
        out_stream.write(f"0x{byte:02x}, ")
    out_stream.write("\n};\n")
    out_stream.write(f"unsigned int {image_name}_len = {len(data)};\n")

def image_to_memory(input_file: str, output_file: str, image_name: str) -> None:
    with open(input_file, "rb") as in_file:
        data = in_file.read()
    with open(output_file, "w") as out_file:
        write_image_header(out_file, data, image_name)

def folder_to_headers(input_folder: str, output_folder: str, single_file: bool = False) -> None:
    os.makedirs(output_folder, exist_ok = True)

    if single_file:
        with open(os.path.join(output_folder, "all_images.h"), "w") as combined_file:
            for filename in os.listdir(input_folder):
                input_path = os.path.join(input_folder, filename)
                if not os.path.isfile(input_path):
                    continue
                name = os.path.splitext(filename)[0]
                with open(input_path, "rb") as img:
                    data = img.read()
                write_image_header(combined_file, data, name)
                combined_file.write("\n")
    else:
        for filename in os.listdir(input_folder):
            input_path = os.path.join(input_folder, filename)
            if not os.path.isfile(input_path):
                continue
            name = os.path.splitext(filename)[0]
            output_path = os.path.join(output_folder, f"{name}.h")
            image_to_memory(input_path, output_path, name)

def memory_to_image(header_file: str, output_folder: str) -> None:
    os.makedirs(output_folder, exist_ok=True)

    with open(header_file, "r") as file:
        content = file.read()
    
    matches = re.finditer(r"\b(\w+)\s*\[\]\s*=\s*\{([^}]*)\}", content, re.S)
    for match in matches:
        image_name = match.group(1)
        hex_values = re.findall(r"0x[0-9a-fA-F]{2}", match.group(2))
        data = bytes(int(h, 16) for h in hex_values)
        ext = next((ext for test, ext in signatures.items() if test(data)), ".bin")
        output_path = os.path.join(output_folder, image_name + ext)

        with open(output_path, "wb") as img_file:
            img_file.write(data)

def select_file():
    root = tk.Tk()
    root.withdraw()
    file = filedialog.askopenfilename(
        title = "Select a file",
        filetypes = [("Todos los archivos", "*.*")]
    )
    root.destroy()
    return file

def select_save_path():
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)
    return filedialog.asksaveasfilename(
        title = "Save a .h file",
        defaultextension = ".h",
        filetypes = [("Header file", "*.h")]
    )

def select_folder():
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)
    return filedialog.askdirectory(
        title="Select folder"
    )

def main():
    while True:
        print("\n=== OPTIONS MENU ===")
        print("1. Convert a image to header")
        print("2. Convert a image folder to headers")
        print("3. Convert a header to images")
        print("4. Leave")
        option = input("Select an option: ").strip()

        if option == "1":
            print("Select an image")
            input_file = select_file()
            print("Choose where to save the output file (.h)")
            output_file = select_save_path()
            image_name = input("Enter the variable name for the image data: ").strip()
            image_to_memory(input_file, output_file, image_name)
            print("✅ Complete.")
        
        elif option == "2":
            print("Select the source folder")
            input_folder = select_folder()
            print("Select the output folder to stor .h files")
            output_folder = select_folder()
            single_file = not input("Do you want a separate file for each image? (y/n): ").strip().lower() == "y"
            folder_to_headers(input_folder, output_folder, single_file)
            print("✅ Complete.")
        
        elif option == "3":
            print("Select a header file (.h)")
            header_file = select_file()
            print("Select the output folder to store .h files")
            output_folder = select_folder()
            memory_to_image(header_file, output_folder)
            print("✅ Complete.")
        
        elif option == "4":
            print("👋 Leaving...")
            break
        else:
            print("❌ Invalid option.")

if __name__ == "__main__":
    main()