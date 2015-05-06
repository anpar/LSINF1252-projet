Struture :
- core.c : contient les fonctions utilisées par les threads ;
- trial.c : contient l'algorithme "trial division" de factorisation ; 
- perf.c : contient la fonction permettant de calculer les performances du programme ;
- io.c : contient les fonctions auxiliaire pour l'interaction avec l'utilisateur ;
- stack.c : contient les fonctions permettant de gérer les buffers (représenté par des piles) ;
- upfact.c : programme principal, permet l'interaction avec l'utilisateur.

Pour compiler libcurl :
$ make curl
Cette opération peut prendre un petit peu de temps.

Pour compiler :
$ make

Pour nettoyer les fichiers :
$ make mr-proper

Usage :
Nécessite libcurl.

./upfact [OPTIONS] [FILE1] [FILE2] ...

[OPTIONS] :
- -maxthreads n où n est un entier positif (valeur par défaut : 4) 
- -stdin permet d'indiquer au programme qu'il doit lire son entrée
depuis l'entrée standard.

[FILE] : peut un être un fichier local ou une URL.
Remarque : le comportement est indéfini pour une URL non-existante.

Quelques petits fichiers de tests sont présents dans le dossier data :
* Les fichiers files1 à 4 doivent retourner 1997.
* 4k : 7919
* 40k : 104729
* 400k : 1299709
Les fichiers 4k, 40k et 400k ont été générés par Mathieu Xhonneux.
Les fichiers ayant servis à établir nos graphes de performances
contenaient des nombres beaucoup plus gros.
