import subprocess
import os
import re
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns


# Set ranges to explore
K_VALUES = [7, 9, 11, 13]
ALPHA_VALUES = [0.01, 0.05, 0.1, 0.2]
TARGET_IDS = ["New1", "New2"]
META_FILE = "txt_files/meta_4.txt"
DB_FILE = "txt_files/db_4.txt"
MODELS_DIR = "models_heat"
RESULTS = {}

os.makedirs("results_heat", exist_ok=True)
os.makedirs(MODELS_DIR, exist_ok=True)

def run_models_generator(k):
    model_path = f"{MODELS_DIR}/k{k}.bin"
    print(f"Training model for k={k}...")
    try:
        subprocess.run([
            "./src/bin/models_generator.out",
            "-i", META_FILE,
            "-k", str(k)
        ], check=True)

        # Move model to correct path
        original = f"models/k{k}.bin"
        target = f"{MODELS_DIR}/k{k}.bin"
        os.rename(original, target)

    except subprocess.CalledProcessError as e:
        print(f"Error running MetaClass for k={k}, alpha={alpha}: {e}")
        return None  # Or skip this combo

    return model_path

def run_main(model_path, alpha, k):
    output_file = f"results_heat/result_k{k}_a{alpha}.txt"
    with open(output_file, "w") as out:
        subprocess.run([
            "./src/bin/main.out",
            "-db", DB_FILE,
            "-m", model_path,
            "-a", str(alpha),
            "-t", "100"  # use high enough t to include New1, New2
        ], stdout=out, check=True)
    return output_file

def extract_nrc_for_targets(file_path, targets):
    nrc_map = {name: None for name in targets}
    with open(file_path, "r") as f:
        for line in f:
            for name in targets:
                if name in line:
                    match = re.search(r'NRC:\s*([\d.]+)', line)
                    if match:
                        nrc_map[name] = float(match.group(1))
    return nrc_map

# Run combinations
for k in K_VALUES:
    model_path = run_models_generator(k)
    for alpha in ALPHA_VALUES:
        out_file = run_main(model_path, alpha, k)
        nrcs = extract_nrc_for_targets(out_file, TARGET_IDS)
        for target in TARGET_IDS:
            RESULTS.setdefault(target, {})[(k, alpha)] = nrcs[target]

            
# Plotting heatmaps
for target in TARGET_IDS:
    print(f"Lowest NRC for {target}:")
    z = []
    for k in K_VALUES:
        row = []
        for a in ALPHA_VALUES:
            val = RESULTS[target][(k, a)]
            row.append(val if val is not None else np.nan)
        z.append(row)
    
    z = np.array(z)
    plt.figure(figsize=(8, 6))
    plt.imshow(z, cmap='viridis', interpolation='nearest', aspect='auto')
    plt.colorbar(label='NRC')
    plt.xticks(ticks=range(len(ALPHA_VALUES)), labels=ALPHA_VALUES)
    plt.yticks(ticks=range(len(K_VALUES)), labels=K_VALUES)
    plt.xlabel("Alpha")
    plt.ylabel("k (context size)")
    plt.title(f"NRC values for {target}")
    
    # Annotate values on heatmap
    for i in range(len(K_VALUES)):
        for j in range(len(ALPHA_VALUES)):
            val = z[i, j]
            if not np.isnan(val):
                plt.text(j, i, f"{val:.3f}", ha='center', va='center', color='white' if val < 0.3 else 'black')

    plt.tight_layout()
    plt.savefig(f"nrc_heatmap_{target}.png")
    plt.show()

