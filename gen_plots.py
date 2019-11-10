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

# **Consts def**

# +
RESULTS_DIR = "results"

ALG_NAMES = [
    "greedyLocalSearch",
    "heuristic",
    "lessNaiveRandomSearch",
    "naiveRandomSearch",
    "steepestLocalSearch",
]

INSTANCE_NAMES = [
    "chr12a",
    "lipa30a",
    "lipa40b",
    "tai64c",
    "sko81",
    "sko100a",
    "tho150",
    "tai256c",
]

METRIC_NAMES = [
    "mean_time",
    "start_obj_val",
    "end_obj_val",
    "num_obj_val_calls",
    "best_permutation"
]
# + {}
import os

def get_names_n_newest(instance_name, alg_name, n):
    filenames = os.listdir(RESULTS_DIR)
    inst_alg_filenames = sorted([filename for filename in filenames if (instance_name, alg_name) == tuple(filename.split('_')[:2])])[::-1]
    
    n_first_filenames = inst_alg_filenames[:min(n, len(inst_alg_filenames))]
    if len(n_first_filenames) != n:
        print("WARNING: Not enaugh measurements (required %s) for %s %s" % (n, instance_name, alg_name))
    return n_first_filenames

def cast_to_type(line, dtype):
    return [dtype(val) for val in line if val]

def parse_file(file_lines):
    return {
        "mean_time": float(file_lines[0][1]),
        "start_obj_val": cast_to_type(file_lines[1], int),
        "end_obj_val": cast_to_type(file_lines[2], int),
        "num_obj_val_calls": cast_to_type(file_lines[3], int),
        "best_permutation": cast_to_type(file_lines[4], int)
    }

def parse_n_alg_inst(data, n, inst_name, alg_name):
    for file_name in get_names_n_newest(inst_name, alg_name, n):
            metrics = {}
            with open(os.path.join(RESULTS_DIR, file_name), "r") as file:
                file_lines = [line.split(" ") for line in file.read().splitlines()]
                try:
                    parsed = parse_file(file_lines)
                    metrics.update(parsed)
                except KeyError:
                    print("Wrong file %s format" % (file_name, ))
            if not file_lines:
                print("Error reading files for: %s %s from %s" % (inst_name, alg_name, file_name))
                continue
            for metric_name in METRIC_NAMES:
                data.setdefault(
                    inst_name, {}
                ).setdefault(
                    metric_name, {}
                ).setdefault(
                    alg_name, []
                ).append(metrics[metric_name])


# +
data = {}
instance_n_map = {}

for inst_name in INSTANCE_NAMES:
    for alg_name in ALG_NAMES:
        parse_n_alg_inst(data, 5, inst_name, alg_name)
# -



