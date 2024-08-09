#!/usr/bin/env python

import os
from _test_file import TestFile

def main() -> None:
    lev_exe = "build/lev"
    test_dir = "tests"

    for file in os.listdir(test_dir):
        path = os.path.join(test_dir, file)
        test = TestFile(path, lev_exe)
        did_succeed = test.run_test()
        if not did_succeed:
            break


if __name__ == '__main__':
    main()
