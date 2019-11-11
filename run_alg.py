# ---
# jupyter:
#   jupytext:
#     formats: ipynb,py
#     text_representation:
#       extension: .py
#       format_name: light
#       format_version: '1.4'
#       jupytext_version: 1.2.4
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# +
INSTANCES = [
    "chr12a",
    "lipa30a",
    "lipa40b",
    "tai64c",
    "sko81",
    "sko100a",
    "tho150",
    "tai256c",
]

BUILD_DIR = "build/"

# +
import subprocess
NUM_TIMES = 10

for i in range(NUM_TIMES):
    print("%d / %d" % (i + 1, NUM_TIMES))
    for name in INSTANCES:
        args = [BUILD_DIR + "main", name]
        popen = subprocess.Popen(args, stdout=subprocess.PIPE)
        popen.wait()
    
print("Cool")
