import os
import json
import subprocess

ANSII_RED = "\033[0;31m"
ANSII_RED_BOLD = "\033[1;31m"
ANSII_RESET = "\033[0m"
ANSII_CLEAR = "\033[2J"


class RDShell:
    def __init__(self, filename="info.json"):
        with open(filename, "r") as f:
            data = json.load(f)
        self.commands = data["Rdshell"]["Commands"]
        os.chdir("..")

    def run_commands(self):
        for cmd in self.commands:
            print(f"Running: {cmd}")
            try:
                result=subprocess.run(cmd,shell=True,check=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
                print(result.stdout)
            except subprocess.CalledProcessError as e:
                print(f"{ANSII_RED_BOLD}Error{ANSII_RED}: Command '{cmd}' failed with error:\n{e.stderr}{ANSII_RESET}")

    def run(self):
        while True:
            cmd = input("rdshell> ").strip()
            if cmd == "exit":
                break
            elif cmd == "list":
                print(self.commands)
            elif cmd == "run":
                self.run_commands()
            elif cmd in ["cls", "clear"]:
                print(ANSII_CLEAR)
            else:
                print(f"{ANSII_RED_BOLD}Error{ANSII_RED}: Unknown command '{cmd}'{ANSII_RESET}")

rds = RDShell()
rds.run()
