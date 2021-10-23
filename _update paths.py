import os


DEFAULT_SETTINGS_PATH = r".\\_settings.json"
SETTINGS_PATH = r".\\data\\user-data\\User\\settings.json"
KEY_WORD = "__VSCODE_PATH__"

VSCODE_PATH = os.path.abspath(".").replace(os.sep, "\\\\")

with open(DEFAULT_SETTINGS_PATH, "r", encoding="utf-8") as f:
    DATA = f.read()

DATA = DATA.replace(KEY_WORD, VSCODE_PATH)

with open(SETTINGS_PATH, "w", encoding="utf-8") as f:
    f.write(DATA)

print(f"Setted VSCode PATH to {VSCODE_PATH}")
