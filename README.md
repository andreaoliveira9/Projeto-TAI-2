# Projeto-TAI-2
Segundo projeto Teoria Algorítmica da Informação

## Overview
This repository includes five programs:
- `models_generator`: Generates models from a given file.
- `main`: Main program that uses the models to compute NRC values and return the top sequences.
- `similarities_levenshtein`: Computes Levenshtein similarities between sequences.
- `similarities_models`: Computes similarities using models.
- `complexity_profile`: Generates a complexity profile for a given sequence.

## Dependencies
To compile and run these programs, ensure the following tools are installed on your Linux system:

- **g++ Compiler**
- **Make**
- **C++ Compiler with C++17 support** (e.g., `g++` with `-std=c++17`)

Install using the following command:

```bash
sudo apt update
sudo apt install build-essential
```

Make sure your compiler supports the C++17 standard. The project uses the following flags:
  
```
-std=c++17 -Wall -Wextra -O2
```

## Installation Instructions
Clone this repository and compile the programs using the provided `Makefile`:

```bash
git clone https://github.com/andreaoliveira9/Projeto-TAI-2
cd Projeto-TAI-2
make
```

Alternatively, compile the files individually:

```bash
make models_generator
make main
make similarities_levenshtein
make similarities_models
make complexity_profile
```

## Running the Programs

### Running `models_generator`

Example command:

```bash
./src/bin/models_generator.out -meta txt_files/meta.txt -k 11
```

- `-meta`: Path to the meta file.
- `-k`: Context size.

The `models_generator` program saves the trained model to a file named `model_k13.bin` in the `models` folder, which can be used later.

### Running `main`

Example command:

```bash
./src/bin/main.out -db txt_files/db.txt -m models/k11.bin -a 0.001 -t 20
```

- `-db`: Path to the database file.
- `-m`: Path to the model file.
- `-a`: Smoothing parameter (alpha).
- `-t`: Top k results to display.

The `main` program computes NRC values for the sequences in the database using the specified model and parameters.

### Running `similarities_levenshtein`

Example command:

```bash
./src/bin/similarities_levenshtein.out -db txt_files/db.txt -id1 "gi|49169782|ref|NC_005831.2| Human Coronavirus NL63, complete genome" -id2 "NC_005831.2 Human Coronavirus NL63, complete genome"
```

- `-db`: Path to the database file.
- `-id1`: First sequence ID.
- `-id2`: Second sequence ID.

The `similarities_levenshtein` program calculates the Levenshtein similarity between two sequences.

### Running `similarities_models`

Example command:

```bash
./src/bin/similarities_models.out -db txt_files/db.txt -id1 "gi|49169782|ref|NC_005831.2| Human Coronavirus NL63, complete genome" -id2 "NC_005831.2 Human Coronavirus NL63, complete genome" -a 0.001 -k 11
```

- `-db`: Path to the database file.
- `-id1`: First sequence ID.
- `-id2`: Second sequence ID.
- `-a`: Smoothing parameter (alpha).
- `-k`: Context size.

The `similarities_models` program calculates the similarity between two sequences using a model trained on one of them.

### Running `complexity_profile`

Example command:

```bash
./src/bin/complexity_profile.out -meta txt_files/meta.txt -db txt_files/db.txt -k 11 -a 0.001 -id "NC_005831.2 Human Coronavirus NL63, complete genome"
```

- `-meta`: Path to the meta file.
- `-db`: Path to the database file.
- `-k`: Context size.
- `-a`: Smoothing parameter (alpha).
- `-id`: Sequence ID.

The `complexity_profile` program generates a CSV file containing values that represent a complexity profile for the specified sequence ID, using the provided model and parameters. The results are saved in the `analysis` folder.

### Visualizing the Complexity Profile
To visualize the complexity profile using the provided Jupyter Notebook (`analysis/complexity_profile.ipynb`), follow these steps:

1. **Create a virtual environment with Python 3.11** (if not already created):

    ```bash
    python3.11 -m venv venv
    source venv/bin/activate
    ```

2. **Install the required Python packages** from `analysis/requirements.txt`:

    ```bash
    pip install -r analysis/requirements.txt
    ```

3. **Open and connect the Jupyter kernel to the virtual environment**

4. **Run** to explore and visualize the complexity data.

### Optimizing Parameters (`k_alpha_optimization.ipynb`)

The `analysis/k_alpha_optimization.ipynb` notebook automates the process of finding the optimal parameters `k` (context size) and `alpha` (smoothing) for training the MetaClass model.

It evaluates combinations of `k` and `alpha`, extracts NRC values for selected sequences, and generates heatmaps to help identify the best configuration.

#### To run the notebook:

1. Ensure `main.out` and `models_generator.out` are compiled and available in `./src/bin/`.
2. Launch Jupyter Notebook in the project root or in the `analysis` folder:

    ```bash
    jupyter notebook
    ```

3. Open `analysis/k_optimization.ipynb`.
4. Follow the steps in the notebook. You’ll be prompted to:
    - Provide the path to the `meta` and `db` files
    - Enter the sequence IDs to analyze (e.g., `New1,New2`)
5. The notebook will train models, run NRC evaluations, and generate NRC heatmaps in the root folder.

#### Dependencies:

Install if needed (within your virtual environment):

```bash
pip install matplotlib numpy pandas seaborn