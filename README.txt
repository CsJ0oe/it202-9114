Pour compiler la bibliothèque de thread et installer celle-ci, ainsi que les tests
dans le dossier "install/", exécuter "make".
_________________________________________________________________________________________________________

Pour compiler la bibliothèque avec pthread, exécuter "make pthreads".
Cette commande créera également les exécutables de tests de cette bibliothèque
dans le dossier "install/bin/", suffixés de "_pthread".
_________________________________________________________________________________________________________

Pour afficher les graphes de comparaison entre les deux librairies, exécuter "make graphs".
Cette commande produira au préalable les deux librairies ainsi que leurs tests respectifs,
puis lancera le script python produisant les différents graphes.
_________________________________________________________________________________________________________

Pour supprimer les librairies, ainsi que les tests, exécuter "make clean".
Cette commande permet de supprimer le dossier "install/", ainsi que celui "obj/".
_________________________________________________________________________________________________________

Pour lancer les tests de la librairie "libthread.a", exécuter "make check".

Pour lancer les tests de la librairie "libthread.a" sous valgrind, effectuer "make valgrind".
_________________________________________________________________________________________________________
