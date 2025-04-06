# Projeto-TAI-2
Segundo projeto Teoria Algorítmica da Informação

Exemplo:
````bash
make

./src/bin/models_generator.out -i txt_files/meta.txt -k 13

./src/bin/main.out -db txt_files/db.txt -m models/k13.bin -a 0.01 -t 20

./src/bin/similarities_levenshtein.out -db txt_files/db.txt -id1 "gi|49169782|ref|NC_005831.2| Human Coronavirus NL63, complete genome" -id2 "NC_005831.2 Human Coronavirus NL63, complete genome"

./src/bin/similarities_models.out -db txt_files/db.txt -id1 "gi|49169782|ref|NC_005831.2| Human Coronavirus NL63, complete genome" -id2 "NC_005831.2 Human Coronavirus NL63, complete genome" -a 0.01 -k 13
````

## Correr complexity profiler
````bash
make clean
make complexity_profile
./src/bin/complexity_profile.out txt_files/meta.txt txt_files/db.txt 10 0.01 "NC_005831.2 Human Coronavirus NL63, complete genome"
````

Run ``complexity_profile.ipynb`` to see the results.