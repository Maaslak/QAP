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

# ## Consts def

# +
RESULTS_DIR = "results"
DATA_DIR = "data"

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
# -
# ## Read metrics data from files

# +
import os

def get_names_n_newest_numiter(instance_name, alg_name, n, num_iter=10):
    filenames = os.listdir(RESULTS_DIR)
    filename = filenames[0]
    inst_alg_filenames = sorted([filename for filename in filenames if (instance_name, alg_name, str(num_iter)) == 
                                 tuple((*filename.split('_')[:2], filename.split('_')[-1]))])[::-1]
    
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

def parse_n_alg_inst(data, instance_n_map, n, inst_name, alg_name, num_iter=10):
    for file_name in get_names_n_newest_numiter(inst_name, alg_name, n, num_iter):
            metrics = {}
            with open(os.path.join(RESULTS_DIR, file_name), "r") as file:
                file_lines = [line.split(" ") for line in file.read().splitlines()]
                try:
                    parsed = parse_file(file_lines)
                    metrics.update(parsed)
                    instance_n_map[inst_name] = file_lines[0][0]
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
        parse_n_alg_inst(data, instance_n_map, 10, inst_name, alg_name)


# +
data_300 = {}

for inst_name in INSTANCE_NAMES:
    for alg_name in ALG_NAMES:
        parse_n_alg_inst(data_300, instance_n_map, 1, inst_name, alg_name, num_iter=300)

# +
opts = {}

def read_sln(opts, inst_name):
    filename = os.path.join(DATA_DIR, inst_name) + ".sln"
    with open(filename, "r") as file:
        file_lines = [
            cast_to_type(row.split(" "), int) for row in file.read().splitlines()
        ]
        opts.setdefault(inst_name, {}).update({
            "opt_val": file_lines[0][1],
            "opt_per": file_lines[1]
        })
for inst_name in INSTANCE_NAMES:
    read_sln(opts, inst_name)
# -



# ## Calc required metics

def apply_metric_func(data: dict, metric_name, func, func_name):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                func_name + "_" + metric_name, {}
            )[alg_name] = func(inst_data[metric_name][alg_name])


# ### Quality

# +
import numpy as np

apply_metric_func(data, "start_obj_val", np.mean, "mean")
apply_metric_func(data, "start_obj_val", np.std, "std")
apply_metric_func(data, "end_obj_val", np.mean, "mean")
apply_metric_func(data, "end_obj_val", np.std, "std")
apply_metric_func(data, "end_obj_val", np.min, "min")


# -

def calc_quality(data, opts):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "quality", {}
            )[alg_name] = (
                inst_data["min_end_obj_val"][alg_name] / 
                opts[inst_name]["opt_val"]
            )
calc_quality(data, opts)

print(instance_n_map[INSTANCE_NAMES[0]])
data[INSTANCE_NAMES[0]]["quality"]
