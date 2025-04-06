# Projeto-TAI-2
Segundo projeto Teoria Algorítmica da Informação

## Overview
This repository contains five programs:
- `models_generator`: Generates models from a given file.
- `main`: Main program that uses the models to compute NRC values and return the top sequences.
- `similarities_levenshtein`: Computes Levenshtein similarities between sequences.
- `similarities_models`: Computes similarities using models.
- `complexity_profile`: Generates a complexity profile for a given sequence.

## Dependencies
To compile and run these programs, ensure you have the following tools installed on your Linux system:

- **g++ Compiler**
- **Make**
- **C++ Compiler with C++17 support** (e.g., `g++` with `-std=c++17`)

Install using the following command:

```bash
sudo apt update
sudo apt install build-essential
```

Make sure your compiler supports the C++17 standard. This project uses the following flags:
  
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

Alternatively, you can compile the files individually:

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
./src/bin/models_generator.out -meta txt_files/meta.txt -k 13
```

- `-meta`: Path to the meta file.
- `-k`: Context size.

The `models_generator` program saves the trained model to a file called `model_k13.bin` in `models` folder, which can be used later.

### Running `main`

Example command:

```bash
./src/bin/main.out -db txt_files/db.txt -m models/k13.bin -a 0.01 -t 20
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

The `similarities_levenshtein` program computes the Levenshtein similarity between two sequences.

### Running `similarities_models`

Example command:

```bash
./src/bin/similarities_models.out -db txt_files/db.txt -id1 "gi|49169782|ref|NC_005831.2| Human Coronavirus NL63, complete genome" -id2 "NC_005831.2 Human Coronavirus NL63, complete genome" -a 0.01 -k 13
```

- `-db`: Path to the database file.
- `-id1`: First sequence ID.
- `-id2`: Second sequence ID.
- `-a`: Smoothing parameter (alpha).
- `-k`: Context size.
- 
The `similarities_models` program computes the similarity between two sequences using a model trained with one of the sequences.

### Running `complexity_profile`

Example command:

```bash
./src/bin/complexity_profile.out -meta txt_files/meta.txt -db txt_files/db.txt -k 10 -a 0.01 -id "NC_005831.2 Human Coronavirus NL63, complete genome"
```

- `-meta`: Path to the meta file.
- `-db`: Path to the database file.
- `-k`: Context size.
- `-a`: Smoothing parameter (alpha).
- `-id`: Sequence ID.

The `complexity_profile` program generates a CSV file with values representing a complexity profile for the specified sequence ID, using the given model and parameters. The results are saved in `analysis` folder.

To visualize the complexity profile plot, run the Jupyter notebook:

```bash
jupyter notebook complexity_profile.ipynb
```