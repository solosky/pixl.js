#!/usb/bin/env python3

import subprocess
import os
import json
from datetime import date, datetime


class GitVersion:
    def __init__(self, source_dir):
        self.source_dir = source_dir

    def get_version_info(self):
        commit = "unknown"
        try:
            commit = self._exec_git("rev-parse --short HEAD") or "unknown"
        except subprocess.CalledProcessError as e:
            print("git rev-parse error:\n" + e.output.decode('utf-8'))

        dirty = False
        try:
            self._exec_git("diff --quiet")
        except subprocess.CalledProcessError as e:
            if e.returncode == 1:
                dirty = True

        # If WORKFLOW_BRANCH_OR_TAG is set in environment, is has precedence
        # (set by CI)
        branch = (
            os.environ.get("WORKFLOW_BRANCH_OR_TAG", None)
            or self._exec_git("rev-parse --abbrev-ref HEAD")
            or "unknown"
        )

        branch_num = self._exec_git("rev-list --count HEAD") or "n/a"

        try:
            version = self._exec_git("describe --tags --abbrev=0 --exact-match")
        except subprocess.CalledProcessError:
            version = "unknown"

        return {
            "GIT_COMMIT": commit,
            "GIT_BRANCH": branch,
            "GIT_BRANCH_NUM": branch_num,
            "VERSION": version,
            "BUILD_DIRTY": dirty and 1 or 0,
        }

    def _exec_git(self, args):
        cmd = ["git"]
        cmd.extend(args.split(" "))
        return (
            subprocess.check_output(cmd, cwd=self.source_dir, stderr=subprocess.STDOUT)
            .strip()
            .decode()
        )


class Main(object):

    def get_root_dir(self):
        dirname, filename = os.path.split(os.path.abspath(__file__)) 
        return os.path.normpath(os.path.join(dirname,".."))

    def generate(self):
        current_info = GitVersion(".").get_version_info()

        if "SOURCE_DATE_EPOCH" in os.environ:
            build_date = datetime.utcfromtimestamp(int(os.environ["SOURCE_DATE_EPOCH"]))
        else:
            build_date = date.today()

        current_info.update(
            {
                "BUILD_DATE": build_date.strftime("%Y-%m-%d"),
                "TARGET": 0,
            }
        )

        if current_info["VERSION"] == 'unknown':
            if "FW_VERSION" in os.environ:
                current_info["VERSION"] = os.environ["FW_VERSION"]
            else:
                current_info["VERSION"] = current_info["GIT_BRANCH"] + "_" + current_info["GIT_COMMIT"]

        version_values = []
        for key in current_info:
            val = current_info[key]
            if isinstance(val, str):
                val = f'"{val}"'
            version_values.append(f"#define {key} {val}")

        new_version_info_fmt = "\n".join(version_values) + "\n"

        current_version_info = None
        outdir = os.path.join(self.get_root_dir(), "src") 
        version_header_name = os.path.join(outdir, "version.inc.h")
        version_json_name = os.path.join(outdir, "version.json")

        try:
            with open(version_header_name, "r") as file:
                current_version_info = file.read()
        except EnvironmentError as e:
            print(e)

        if current_version_info != new_version_info_fmt:
            print("old: ", current_version_info)
            print("new: ", new_version_info_fmt)
            with open(version_header_name, "w", newline="\n") as file:
                file.write(new_version_info_fmt)
            # os.utime("../lib/toolbox/version.c", None)
            print("Version information updated")
        else:
            print("Version information hasn't changed")

        version_json = {
            "firmware_build_date": current_info["BUILD_DATE"],
            "firmware_commit": current_info["GIT_COMMIT"],
            "firmware_branch": current_info["GIT_BRANCH"],
            "firmware_target": current_info["TARGET"],
        }
        with open(version_json_name, "w", newline="\n") as file:
            json.dump(version_json, file, indent=4)
        return 0


if __name__ == "__main__":
    m = Main()
    m.generate()
