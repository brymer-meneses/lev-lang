
import subprocess
import os

from typing import List, Optional, Tuple
from difflib import SequenceMatcher
from pathlib import Path

RUN_ARGS_DELIM = r"// RUN-ARGS:"
CHECK_STDOUT_DELIM = r"// CHECK-STDOUT:"


class TestFile:
    path: str
    run_args: List[str]
    lev_exe: str

    def __init__(self, path: str, lev_exe) -> None:
        self.path = path
        self.lev_exe = lev_exe

        try:
            with open(path, 'r') as f:
                self.lines = f.readlines()
        except OSError:
            raise RuntimeError("Could not open file")

    def run_test(self) -> bool:
        got_lines = self.get_output_lines()
        expected_lines = self.parse_expected_lines()
        line_start = self.get_source_end_line()

        did_succeed = True
        diff = None

        for (got, expected) in zip(got_lines, expected_lines):
            if got != expected:
                did_succeed = False
                diff = check_diff(got, expected)
                break
            line_start += 1

        if did_succeed:
            print(f"{self.path} ... [OKAY]")
        else:
            print(f"{self.path} ... [ERR]")
            print(f"{line_start+1} | {diff}")

        return did_succeed

    def update_expected_lines(self) -> None:
        got_lines = self.get_output_lines()
        source_end = self.get_source_end_line()

        for i, line in enumerate(got_lines):
            got_lines[i] = f"{CHECK_STDOUT_DELIM} {line}\n"

        lines = self.lines
        lines = lines[0:(source_end)] + got_lines
        
        with open(self.path, 'w') as f:
            f.writelines(lines)
        

    def get_output_lines(self) -> List[str]:
        run_args = self.parse_run_args()
        if run_args is None:
            raise RuntimeError(f"The test file {self.path} has no run args")

        output = subprocess.check_output(f"{self.lev_exe} {run_args} {self.path}", shell=True, encoding='utf-8', stderr=subprocess.DEVNULL)
        return output.splitlines()

    def parse_expected_lines(self) -> List[str]:
        expected_lines = []

        for line in self.lines:
            if line.startswith(CHECK_STDOUT_DELIM):
                expected_lines.append(line[len(CHECK_STDOUT_DELIM):].strip())

        return expected_lines

    def parse_run_args(self) -> Optional[str]:
        for line in self.lines:
            if line.startswith(RUN_ARGS_DELIM):
                contents = line[len(RUN_ARGS_DELIM):]
                return contents.strip()
        return None

    def get_source_end_line(self) -> int:
        i = 0
        for line in self.lines:
            if line.startswith(CHECK_STDOUT_DELIM):
                return i
            i += 1
        return i


def check_diff(a: str, b: str) -> str:
    output = []
    matcher = SequenceMatcher(None, a, b)

    green = '\x1b[38;5;16;48;5;2m'
    red = '\x1b[38;5;16;48;5;1m'
    reset = '\x1b[0m'

    for opcode, a0, a1, b0, b1 in matcher.get_opcodes():
        if opcode == 'equal':
            output.append(a[a0:a1])
        elif opcode == 'insert':
            output.append(f'{green}{b[b0:b1]}{reset}')
        elif opcode == 'delete':
            output.append(f'{red}{a[a0:a1]}{reset}')
        elif opcode == 'replace':
            output.append(f'{green}{b[b0:b1]}{reset}')
            output.append(f'{red}{a[a0:a1]}{reset}')

    return ''.join(output)
