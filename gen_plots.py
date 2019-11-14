# -*- coding: utf-8 -*-
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

LS_NAMES = [
    "greedyLocalSearch",
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

Ns = [
    12,
    30,
    40,
    64,
    81,
    100,
    150,
    256,
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
        parse_n_alg_inst(data, instance_n_map, 5, inst_name, alg_name)


# +
data_big = {}

for inst_name in INSTANCE_NAMES:
    for alg_name in ALG_NAMES:
        parse_n_alg_inst(data_big, instance_n_map, 40, inst_name, alg_name, num_iter=10)

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
            )[alg_name] = func(inst_data[metric_name][alg_name], axis=1)


# ### Quality
#
# opt_obj_val / obj_val

len(data_big[INSTANCE_NAMES[0]]['end_obj_val'][ALG_NAMES[0]][0])

# +
import numpy as np

apply_metric_func(data, "start_obj_val", np.mean, "mean")
apply_metric_func(data, "start_obj_val", np.std, "std")
apply_metric_func(data, "end_obj_val", np.mean, "mean")
apply_metric_func(data, "end_obj_val", np.std, "std")
apply_metric_func(data, "end_obj_val", np.min, "min")

def minimum_accumulate(data, axis):
    return np.minimum.accumulate(np.array(data).reshape(-1))

# Cumulative minimum Cumulative mean
apply_metric_func(data_big, "end_obj_val", minimum_accumulate, "min_acc")

def mean_accumulate(data, axis):
    data = np.array(data).reshape(-1)
    return np.cumsum(data) / (np.arange(len(data)) + 1)

def std_accumulate(data, axis):
    data = np.array(data).reshape(-1)
    cumsum = np.cumsum(data)
    return [
        np.sum((data[:i] - cumsum[:i]) ** 2) / i
        for i in range(1, len(data) + 1)
    ]
    
    
apply_metric_func(data_big, "end_obj_val", mean_accumulate, "mean_acc")
apply_metric_func(data_big, "end_obj_val", std_accumulate, "std_acc")
# -

data_big[INSTANCE_NAMES[0]]['mean_acc_end_obj_val'][ALG_NAMES[0]].shape


def calc_quality(data, opts):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "quality", {}
            )[alg_name] = (
                opts[inst_name]["opt_val"]
                / np.array(inst_data["end_obj_val"][alg_name])
            )
calc_quality(data, opts)
calc_quality(data_big, opts)


def calc_quality_start(data, opts):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "quality_start", {}
            )[alg_name] = (
                opts[inst_name]["opt_val"]
                / np.array(inst_data["start_obj_val"][alg_name])
            )
calc_quality_start(data, opts)
calc_quality_start(data_big, opts)


def calc_quality_best(data, opts):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "quality_best", {}
            )[alg_name] = (
                opts[inst_name]["opt_val"] / 
                min(inst_data["min_end_obj_val"][alg_name])
            )
calc_quality_best(data, opts)


def calc_quality_best_per_numit(data, opts):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "quality_best_numit", {}
            )[alg_name] = (
                opts[inst_name]["opt_val"] / 
                np.min(inst_data["end_obj_val"][alg_name], axis=1)
            )
calc_quality_best_per_numit(data_big, opts)



data[INSTANCE_NAMES[0]]['quality_best'][ALG_NAMES[0]]


# +
def apply_efficiency(data: dict):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "efficiency", {}
            )[alg_name] = (
                inst_data["quality"][alg_name] / 
                    np.array(inst_data["mean_time"][alg_name])[:, np.newaxis]
            )

apply_efficiency(data)
# -

data[INSTANCE_NAMES[0]]['efficiency'][ALG_NAMES[0]]

# ### Gen plots

# +
import matplotlib.pyplot as plt

FIGURES_DIR = "figures/"

TICKS_ALG_NAMES_MAP = {
    "greedyLocalSearch": "GLS",
    "heuristic": "H",
    "lessNaiveRandomSearch": "LNRS",
    "naiveRandomSearch": "NRS",
    "steepestLocalSearch": "SLS",
}

TICKS_ALG_NAMES = [TICKS_ALG_NAMES_MAP[alg_name] for alg_name in ALG_NAMES]

MARKERS = ".<ov^"
MARKERS = [marker for marker in MARKERS]

X_MOVE = np.linspace(-5, 5, len(ALG_NAMES))
# -

len(INSTANCE_NAMES)

# ### Quality ALL

# +
metric_name = "quality"
plt.figure(figsize=(10,10))

for i, inst_name in enumerate(INSTANCE_NAMES):
    box_plot_data = []
    for alg_name in ALG_NAMES:
        box_plot_data.append(
            np.array(data[inst_name][metric_name][alg_name])[0]
        )
    plt.subplot(4, 2, i + 1)
    plt.boxplot(box_plot_data)
    plt.title("Nazwa instancji " + inst_name)
    plt.xticks(list(range(1, len(ALG_NAMES) + 1)), TICKS_ALG_NAMES)
    plt.ylabel("Jakość rozwiązania")
    plt.xlabel("Nazwa algorytmu")
plt.tight_layout()
plt.savefig(FIGURES_DIR + 'quality_mean.pdf')
plt.show()
# -

# ### Best quality

# +
metric_name = "quality_best"
plt.figure(figsize=(10,5))

for alg_name, marker, x_move in zip(ALG_NAMES, MARKERS, X_MOVE):
    best_qal_data = []
    for inst_name in INSTANCE_NAMES:
        best_qal_data.append(data[inst_name][metric_name][alg_name])
    plt.plot(
        Ns + x_move,
        best_qal_data,
        'o-',
        alpha=0.6,
        label=TICKS_ALG_NAMES_MAP[alg_name],
        marker=marker
    )
plt.xticks(Ns, INSTANCE_NAMES, rotation="vertical")
plt.ylabel("Jakość rozwiązania")
plt.xlabel("Nazwa instacji")
plt.legend()
plt.savefig(FIGURES_DIR + metric_name + '.pdf')
plt.show()
# -

# ### Time

# +
metric_name = "mean_time"
plt.figure(figsize=(10,5))

def add_eff_bars(is_moved=True):
    for alg_name, marker, x_move in zip(ALG_NAMES, MARKERS, X_MOVE):
        best_qal_data = []
        for inst_name in INSTANCE_NAMES:
            best_qal_data.append(data[inst_name][metric_name][alg_name])
        plt.errorbar(
            # range(len(INSTANCE_NAMES)), 
            np.array(Ns) + (x_move if is_moved else 0),
            np.mean(best_qal_data, axis=1), 
            np.std(best_qal_data, axis=1),
            marker=marker,
            linestyle="None",
            label=TICKS_ALG_NAMES_MAP[alg_name],
        )
    plt.xticks(Ns, INSTANCE_NAMES, rotation="vertical")
    plt.ylabel("Czas [s]")
    plt.xlabel("Nazwa instacji")
add_eff_bars()
plt.legend()

plt.savefig(FIGURES_DIR + metric_name + '.pdf')

plt.show()
# -

# ### Efficiency

# +
metric_name = "efficiency"

plt.figure(figsize=(10,5))

for alg_name, marker, x_move in zip(ALG_NAMES, MARKERS, X_MOVE):
    Y = []
    for inst_name in INSTANCE_NAMES:
        Y.append(data[inst_name][metric_name][alg_name])
    Y = np.array(Y).reshape(len(INSTANCE_NAMES), -1)
    plt.errorbar(
            np.arange(len(INSTANCE_NAMES)) + x_move, 
            np.mean(Y, axis=1), 
            np.std(Y, axis=1),
            marker=marker,
            linestyle="None",
            label=TICKS_ALG_NAMES_MAP[alg_name],
        )
plt.xticks(list(range(len(INSTANCE_NAMES))), INSTANCE_NAMES)
plt.ylabel("Efektywność [1/s]")
plt.xlabel("Nazwa instacji")
plt.legend()
plt.savefig(FIGURES_DIR + metric_name + '.pdf')
plt.show()
# -

# ### num obj calls

# +
metric_name = "num_obj_val_calls"

plt.figure(figsize=(10,5))

for alg_name, marker, x_move in zip(ALG_NAMES, MARKERS, X_MOVE):
    Y = []
    for inst_name in INSTANCE_NAMES:
        Y.append(data[inst_name][metric_name][alg_name])
    Y = np.array(Y).reshape(len(INSTANCE_NAMES), -1)
    plt.errorbar(
            Ns + x_move, 
            np.mean(Y, axis=1), 
            np.std(Y, axis=1),
            marker=marker,
            linestyle="None",
            label=TICKS_ALG_NAMES_MAP[alg_name],
        )
plt.xticks(Ns, INSTANCE_NAMES, rotation="vertical")
plt.ylabel("Liczba ocenionych rozwiązań")
plt.xlabel("Nazwa instacji")
plt.legend()
plt.savefig(FIGURES_DIR + metric_name + '.pdf')
plt.show()
# -

# ### Start quality vs END Qality

# +
metric_name_Y = "quality"
metric_name_X = "quality_start"
plt.figure(figsize=(10,5))

for i, inst_name in enumerate(["chr12a", "sko100a"]):
    plt.subplot(1, 2, i + 1)
    for alg_name in LS_NAMES:
        X = (
            np.array(data_big[inst_name][metric_name_X][alg_name]).reshape(-1)
        )
        Y = (
            np.array(data_big[inst_name][metric_name_Y][alg_name]).reshape(-1)
        )
        plt.plot(
            X,
            Y,
            "o",
            markersize=2,
            label=TICKS_ALG_NAMES_MAP[alg_name],
            alpha=0.6
        )
    plt.xlabel("Jakość rozwiązania początkowego")
    plt.ylabel("Jakość rozwiązania końcowego")
    plt.title(inst_name)
plt.tight_layout()
plt.legend()
plt.savefig("%s%s_vs_%s.pdf" % (FIGURES_DIR, metric_name_X, metric_name_Y))
plt.show()
# -

# #### 4 Cumulative metrics

# +
metric_name = "min_acc_end_obj_val"
plt.figure(figsize=(10,10))

for i, inst_name in enumerate(["chr12a", "tho150", "tai256c", "sko100a"]):
    plt.subplot(2, 2, i + 1)
    for alg_name in LS_NAMES:
        Y = (
            np.array(data_big[inst_name][metric_name][alg_name]).reshape(-1)
        )
        X = np.arange(len(Y))
        plt.plot(
            X,
            Y,
            "o",
            markersize=2,
            label=TICKS_ALG_NAMES_MAP[alg_name]
        )
    plt.xlabel("liczba restartów")
    plt.ylabel("Wartość funkcji celu")
    plt.title(inst_name)
plt.tight_layout()
plt.legend()
plt.savefig("%s%s.pdf" % (FIGURES_DIR, metric_name))
plt.show()

# +
metric_name = "mean_acc_end_obj_val"
std_metric_name = "std_acc_end_obj_val"
plt.figure(figsize=(10,10))

for i, inst_name in enumerate(["chr12a", "tho150", "lipa40b", "sko100a"]):
    plt.subplot(2, 2, i + 1)
    for alg_name in LS_NAMES:
        Y = np.array(data_big[inst_name][metric_name][alg_name]).reshape(-1)
        X = np.arange(len(Y))
        e = np.array(data_big[inst_name][std_metric_name][alg_name]).reshape(-1)
        plt.plot(
            X,
            Y,
#             e,
            "o",
            markersize=2,
            label=TICKS_ALG_NAMES_MAP[alg_name]
        )
    plt.xlabel("liczba restartów")
    plt.ylabel("Wartość funkcji celu")
    plt.title(inst_name)
plt.tight_layout()
plt.legend()
plt.savefig("%s%s.pdf" % (FIGURES_DIR, metric_name))
plt.show()


# -

# ### Ocena podobieństwa

# +
def calc_similarity(perm1, perm2):
    return sum([1 if a==b else 0 for a, b in zip(perm1, perm2)])

def calc_similarity_list(perm1_list, perm2):
    return [
        calc_similarity(perm1, perm2)
        for perm1 in perm1_list
    ]

def apply_similarity_to_opt(data, opts):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "similarity_opt", {}
            )[alg_name] = calc_similarity_list(
                inst_data["best_permutation"][alg_name],
                opts[inst_name]["opt_per"]
            )
apply_similarity_to_opt(data_big, opts)

# +
metric_X = "quality_best_numit"
metric_Y = "similarity_opt"

plt.figure(figsize=(10,5))

for i, inst_name in enumerate(["chr12a", "tai256c"]):
    plt.subplot(1, 2, i + 1)
    for alg_name in LS_NAMES:
        X = (
            np.array(data_big[inst_name][metric_X][alg_name]).reshape(-1)
        )
        Y = (
            np.array(data_big[inst_name][metric_Y][alg_name]).reshape(-1)
        )
        plt.plot(
            X,
            Y,
            "o",
            markersize=2,
            label=TICKS_ALG_NAMES_MAP[alg_name]
        )
    plt.xlabel("jakość rozwiązania")
    plt.ylabel("podobieństwo do opt")
    plt.title(inst_name)
    plt.legend()
plt.tight_layout()
plt.savefig("%s%s_vs_%s.pdf" % (FIGURES_DIR, metric_X, metric_Y))
plt.show()


# +
def apply_similarity_interior(data):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "similarity", {}
            )[alg_name] = [
                calc_similarity(
                            permutation1,
                            permutation2
                        )
                for i, permutation1 in enumerate(inst_data["best_permutation"][alg_name])
                for j, permutation2 in enumerate(inst_data["best_permutation"][alg_name])
                if i != j
            ]
                        
apply_similarity_interior(data_big)


# +
def delta_quality_interior(data):
    for inst_name in INSTANCE_NAMES:
        for alg_name in ALG_NAMES:
            inst_data = data[inst_name]
            inst_data.setdefault(
                "delta_quality_interior", {}
            )[alg_name] = [
                abs(quality1 - quality2)
                for i, quality1 in enumerate(inst_data["quality_best_numit"][alg_name])
                for j, quality2 in enumerate(inst_data["quality_best_numit"][alg_name])
                if i != j
            ]
                        
apply_similarity_interior(data_big)
delta_quality_interior(data_big)

# +
metric_X = "delta_quality_interior"
metric_Y = "similarity"

plt.figure(figsize=(10,10))

for i, inst_name in enumerate(["chr12a", "tai256c"]):
    plt.subplot(2, 1, i + 1)
    for alg_name in LS_NAMES:
        X = (
            np.array(data_big[inst_name][metric_X][alg_name]).reshape(-1)
        )
        Y = (
            np.array(data_big[inst_name][metric_Y][alg_name]).reshape(-1)
        )
        plt.plot(
            X,
            Y,
            "o",
            alpha=0.2,
            markersize=2,
            label=TICKS_ALG_NAMES_MAP[alg_name]
        )
    plt.xlabel("różnica jakości")
    plt.ylabel("wartość miary podobieństwa")
    plt.title(inst_name)
    plt.legend()
plt.tight_layout()
plt.savefig("%s%s_vs_%s.pdf" % (FIGURES_DIR, metric_X, metric_Y))
plt.show()
