# EnseaSH – TP Programmation Système

Ce projet contient plusieurs versions d’un mini-shell appelé **EnseaSH**.  
Chaque fichier `enseash_qX.c` correspond à une étape de la construction du shell :

- `enseash_q1.c`
- `enseash_q2a.c`
- `enseash_q2.c`
- `enseash_q3.c`

Les explications ci-dessous décrivent chaque version de façon simple, à destination de débutants.

---

## `enseash_q1.c` – Message de bienvenue et premier prompt

### Objectif

Afficher un **message de bienvenue** puis un **prompt**, puis terminer immédiatement le programme.

### Résumé du code

- Les bibliothèques utilisées sont :
    - `<unistd.h>` pour la fonction système `write` (écriture sur la sortie standard).
    - `<string.h>` pour `strlen` (longueur des chaînes de caractères).
- Deux chaînes constantes sont définies :
    - `welcome` : message de bienvenue avec un retour à la ligne.
    - `prompt` : texte du prompt `enseash %` suivi d’un retour à la ligne.
- Dans `main` :
    - `write(STDOUT_FILENO, welcome, strlen(welcome));`
        - affiche le message de bienvenue.
    - `write(STDOUT_FILENO, prompt, strlen(prompt));`
        - affiche le prompt.
    - Le programme retourne ensuite `0` et se termine.

### Comportement

1. Affiche :
    - « Bienvenue dans le Shell ENSEA. »
    - « Pour quitter, tapez 'exit'. »
2. Affiche ensuite : `enseash %`
3. Le programme **s’arrête** : il n’y a pas de boucle, ni de lecture du clavier.

---

## `enseash_q2a.c` – Première lecture au clavier (étape intermédiaire)

### Objectif

Commencer à lire ce que tape l’utilisateur au clavier, **nettoyer** la fin de la ligne, puis afficher un prompt.  
C’est une étape intermédiaire avant d’avoir un shell en boucle.

### Résumé du code

- Les bibliothèques utilisées sont :
    - `<unistd.h>` pour `read` et `write`.
    - `<string.h>` pour `strlen`.
- Un `BUFSIZE` de 128 est défini et un buffer `char buf[BUFSIZE];` est utilisé.
- Le programme :
    - définit à l’intérieur de `main` deux chaînes constantes `welcome` et `prompt` (mais n’affiche que le prompt).
    - lit depuis l’entrée standard (clavier) :
        - `n = read(STDIN_FILENO, buf, BUFSIZE);`
    - si au moins un caractère a été lu (`n > 0`) et que le dernier est un retour à la ligne (`'\n'`), il remplace ce caractère par `'\0'` pour terminer proprement la chaîne :
        - `if (n > 0 && buf[n - 1] == '\n') { buf[n - 1] = '\0'; }`
    - affiche ensuite le prompt avec `write(STDOUT_FILENO, prompt, strlen(prompt));`.

### Comportement

1. Lit une **seule fois** une ligne tapée par l’utilisateur dans `buf`.
2. Supprime le retour à la ligne final si présent.
3. Affiche le prompt `enseash %`.
4. Le programme se termine.

Remarques :

- Le message de bienvenue `welcome` existe mais n’est pas affiché dans ce code.
- Il n’y a **pas de boucle** et aucune commande n’est exécutée.
- Cette version sert à s’entraîner à :
    - utiliser `read` sur l’entrée standard,
    - manipuler un buffer,
    - enlever le `'\n'` à la fin de la chaîne.

---

## `enseash_q2.c` – Boucle + exécution d’une commande simple (sans arguments)

### Objectif

Mettre en place une vraie **boucle de shell** qui :

1. Affiche un message de bienvenue.
2. Affiche un prompt.
3. Lit une commande entrée par l’utilisateur (une seule chaîne, sans arguments).
4. Crée un processus fils qui exécute cette commande avec `execvp`.
5. Attend la fin de la commande, puis revient au prompt.

### Résumé du code

- Bibliothèques utilisées :
    - `<unistd.h>` pour `read`, `write`, `execvp`.
    - `<string.h>` pour `strlen`.
    - `<sys/types.h>` et `<sys/wait.h>` pour `pid_t` et `waitpid`.
- Une fonction utilitaire est définie :
    - `print_str(const char *s)` :
        - calcule la longueur de la chaîne avec `strlen`,
        - affiche la chaîne avec `write` sur `STDOUT_FILENO`.
- Dans `main` :
    - `welcome` contient le message de bienvenue.
    - `prompt` contient le texte `enseash % `.
    - un buffer `cmd_line[MAX_CMD_LEN]` est utilisé pour lire la commande.
    - on affiche d’abord le message de bienvenue avec `print_str(welcome);`.
    - on entre ensuite dans une boucle infinie `while (1)`.

### À chaque tour de boucle

1. Affichage du prompt :
    - `print_str(prompt);`
2. Lecture de la ligne tapée par l’utilisateur :
    - `n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN);`
3. Ajout du caractère nul pour terminer la chaîne :
    - `cmd_line[n] = '\0';`
4. Suppression éventuelle du retour à la ligne final :
    - `if (n > 0 && cmd_line[n - 1] == '\n') { cmd_line[n - 1] = '\0'; }`
5. Si la ligne est vide (juste Entrée) :
    - `if (cmd_line[0] == '\0') { continue; }`
    - on recommence la boucle sans exécuter de commande.

### Création du fils et exécution de la commande

- On crée un **processus fils** :
    - `pid = fork();`
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
        - `print_str("Erreur : fork a échoué.\n");`

### Comportement

- Le shell :
    - affiche un message de bienvenue,
    - affiche le prompt,
    - lit une commande simple (sans arguments),
    - lance un processus fils qui exécute cette commande (`ls`, `date`, etc.),
    - attend la fin de la commande,
    - puis revient au prompt.
- Le shell tourne en boucle **tant qu’on ne le quitte pas** (il n’y a pas encore de commande de sortie dans cette version).

---

## `enseash_q3.c` – Sortie propre avec `exit` ou Ctrl+D

### Objectif

Améliorer la Q2 pour que l’utilisateur puisse **quitter proprement** le shell :

- en tapant la commande `exit`,
- ou en envoyant un EOF via `Ctrl + D`.

### Résumé du code

- Bibliothèques utilisées :
    - `<unistd.h>`, `<string.h>`, `<sys/types.h>`, `<sys/wait.h>` comme en Q2.
- Une fonction `print_str(const char *s)` identique à celle de Q2 est utilisée.
- Dans `main` :
    - `welcome` : message de bienvenue.
    - `prompt`  : texte `enseash % `.
    - `bye`     : message de sortie `"Bye bye...\n"`.
    - `cmd_line[MAX_CMD_LEN]` : buffer pour la ligne de commande.
    - on affiche `welcome` au début, puis on entre dans une boucle `while (1)`.

### Gestion de Ctrl + D

Après la lecture :

- `n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN);`
- `cmd_line[n] = '\0';`
- Si `n == 0`, cela signifie que l’utilisateur a envoyé un EOF (par exemple avec `Ctrl + D`) :
    - le code teste :
      - `if (n == 0) { print_str(bye); break; }`
    - on affiche alors `Bye bye...` puis on sort de la boucle, ce qui termine le programme.

### Gestion de la commande `exit`

Juste après le test sur `n == 0`, on vérifie si la commande saisie est `"exit"` :

- `if (strcmp(cmd_line, "exit") == 0) { print_str(bye); break; }`

Dans ce cas :

- on n’essaie pas d’exécuter une commande externe,
- on affiche le message `Bye bye...`,
- puis on quitte la boucle et donc le shell.

### Nettoyage de la ligne et exécution des autres commandes

Ensuite :

- on enlève éventuellement le retour à la ligne final :
    - `if (n > 0 && cmd_line[n - 1] == '\n') { cmd_line[n - 1] = '\0'; }`
- si la ligne est vide :
    - `if (cmd_line[0] == '\0') { continue; }`
    - on revient au début de la boucle sans exécuter de commande.
- sinon, on exécute la commande exactement comme en Q2 :
    - `fork()` pour créer un fils,
    - dans le fils, préparation de `argv` et appel à `execvp(cmd_line, argv);`
    - dans le père, `waitpid(pid, &status, 0);`
    - en cas d’échec de `fork`, affichage d’un message d’erreur.

### Rôle de `status`

- `int status;` est une variable passée à `waitpid` qui contient des informations sur la fin du processus fils.
- Dans cette version, on ne l’exploite pas encore (on ne l’affiche pas), mais il sera utile dans les étapes suivantes du TP (par exemple pour afficher le code de retour dans le prompt).

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

---
