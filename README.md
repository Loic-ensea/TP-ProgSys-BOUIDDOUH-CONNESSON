# EnseaSH – TP Programmation Système

## `enseash_q1.c` – Message de bienvenue et premier prompt

### Comportement

1. Affiche :
    - « Bienvenue dans le Shell ENSEA. »
    - « Pour quitter, tapez 'exit'. »
2. Affiche ensuite : `enseash %`
3. Le programme **s’arrête** : il n’y a pas de boucle, ni de lecture du clavier.

## `enseash_q2a.c` – Première lecture au clavier (étape intermédiaire)

### Comportement

1. Lit une **seule fois** une ligne tapée par l’utilisateur dans `buf`.
2. Supprime le retour à la ligne final si présent.
3. Affiche le prompt `enseash %`.
4. Le programme se termine.


## `enseash_q2.c` – Boucle + exécution d’une commande simple (sans arguments)

### À chaque tour de boucle

1. Affichage du prompt.
2. Lecture de la ligne tapée par l’utilisateur :
    - `n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN);`
3. Ajout du caractère nul pour terminer la chaîne :
4. Suppression éventuelle du retour à la ligne final :
    - `if (n > 0 && cmd_line[n - 1] == '\n') { cmd_line[n - 1] = '\0'; }`
5. Si la ligne est vide (juste Entrée) :
    - on recommence la boucle sans exécuter de commande.

### Création du fils et exécution de la commande

- On crée un **processus fils** :
- Dans le **fils** (`pid == 0`) :
    - on prépare un petit tableau d’arguments :
        - `argv[0] = cmd_line;`
        - `argv[1] = NULL;`
    - on exécute la commande saisie avec :
        - `execvp(argv[0], argv);`
    - si `execvp` échoue, on termine le fils avec `_exit(1);`.
- Dans le **père** (`pid > 0`) :
    - on attend la fin du fils :
        - `waitpid(pid, &status, 0);`
- Si `fork` échoue (`pid < 0`) :
    - on affiche un message d’erreur :

### Comportement

- Le shell :
    - affiche un message de bienvenue,
    - affiche le prompt,
    - lit une commande simple (sans arguments),
    - lance un processus fils qui exécute cette commande (`ls`, `date`, etc.),
    - attend la fin de la commande,
    - puis revient au prompt.
- Le shell tourne en boucle **tant qu’on ne le quitte pas** (il n’y a pas encore de commande de sortie dans cette version).

## `enseash_q3.c` – Sortie propre avec `exit` ou Ctrl+D

### Gestion de Ctrl + D

Après la lecture :

- Si `n == 0`, cela signifie que l’utilisateur a utilisé un `Ctrl + D` :
    - le code teste :
      - `if (n == 0) { print_str(bye); break; }`
    - on affiche alors `Bye bye...` puis on sort de la boucle, ce qui termine le programme.

### Gestion de la commande `exit`

Juste après le test sur `n == 0`, on vérifie si la commande saisie est `"exit\n"` :
Dans ce cas :
- on affiche le message `Bye bye...`,
- puis on quitte la boucle et donc le shell.

### Comportement

Avec `enseash_q3.c`, le shell permet :

- d’exécuter des commandes simples comme :
    - `ls`
    - `date`
- de quitter proprement en tapant :
    - `exit`
- de quitter aussi en faisant :
    - `Ctrl + D` au prompt (EOF)

Dans les deux cas (`exit` ou `Ctrl + D`), le message `Bye bye...` est affiché avant la fin du programme.

## `enseash_q4.c` – Affichage du code de retour ou du signal dans le prompt

### Résumé du code

- Deux variables sont ajoutées :
  - `int last_status = 0;` : contient le **status** de la dernière commande (valeur renvoyée par `waitpid`).
  - `int has_status = 0;` : indique si au moins une commande a déjà été exécutée (au tout début on n’a pas de status à afficher).

- Avant chaque lecture de commande, on **construit le prompt** dans un buffer `prompt` avec `sprintf` :

### Comportement

- Au lancement, comme aucune commande n’a encore été exécutée, le prompt est simple :
  ```text
  enseash %

- Après une commande qui réussit (par exemple ls) => enseash [exit:0] %
- Après une commande qui retourne un code d’erreur (par exemple false ou une commande invalide) => enseash [exit:1] %
- Si la commande s’arrête à cause d’un signal (par exemple un SIGKILL) => enseash [sign:9] % 


