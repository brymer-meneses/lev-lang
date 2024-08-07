
import os
import subprocess

from pathlib import Path
from sys import get_coroutine_origin_tracking_depth, stdout
from typing import List, Optional

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

    def run(self) -> bool:
        got_lines = self.get_output_lines()
        expected_lines = self.parse_expected_lines()

        if len(expected_lines) != len(got_lines):
            return False

        for (expected, got) in zip(expected_lines, got_lines):
            if expected != got:
                return False
        return True

    def update_expected_lines(self) -> None:
        got_lines = self.get_output_lines()
        source_end = self.get_source_end_line()

        for i, line in enumerate(got_lines):
            got_lines[i] = f"{CHECK_STDOUT_DELIM} {line}\n"

        lines = self.lines
        lines = lines[0:(source_end+1)] + got_lines
        
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
                expected_lines.append(line[len(CHECK_STDOUT_DELIM):])

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


