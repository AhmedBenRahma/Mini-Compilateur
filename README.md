# Mini-Compilateur

Un compilateur simplifié conçu pour traiter un sous-ensemble d'un langage de programmation (mini-Pascal). Il permet de comprendre et d'implémenter les principales phases d'un compilateur réel.

## 📋 Description

Ce projet implémente un compilateur éducatif qui gère les phases principales de compilation : 

- **Analyse lexicale** (scanner / nextToken)
- **Analyse syntaxique** (descente récursive)
- **Analyse sémantique** (vérification des identificateurs déclarés)
- **Génération de code** pour une machine à pile

## 🚀 Fonctionnalités

### Mots-clés supportés
- `program`, `var`, `begin`, `end`
- `if`, `then`, `else`
- `while`, `do`
- `read`, `readln`, `write`, `writeln`
- `integer`, `char`

### Opérateurs
- **Arithmétiques** : `+`, `-`, `*`, `/`, `%`
- **Relationnels** : `=`, `<>`, `<`, `<=`, `>`, `>=`
- **Logiques** : `||` (OR), `&&` (AND)

### Structures de contrôle
- Affectation :  `id := expression`
- Conditionnelle : `if ... then ... else ... `
- Boucle : `while ... do ...`
- Entrées/Sorties : `read()`, `readln()`, `write()`, `writeln()`

### Commentaires
Les commentaires sont délimités par `(* ...  *)`

## 📁 Structure du projet

```
Mini-Compilateur/
├── main.c              # Code source du compilateur
├── source.txt          # Programme source à compiler (entrée)
├── code.txt            # Code intermédiaire généré (sortie)
├── mini_compilateur.exe # Exécutable compilé
└── README.md           # Ce fichier
```

## 🛠️ Compilation

```bash
gcc main.c -o mini_compilateur
```

## 💻 Utilisation

1. Écrire votre programme dans `source.txt`
2. Exécuter le compilateur : 
   ```bash
   ./mini_compilateur
   ```
   ou sur Windows :
   ```bash
   mini_compilateur.exe
   ```
3. Le code intermédiaire généré sera disponible dans `code.txt`

## 📝 Exemple de programme

```pascal
program exemple;
var
  x, y, z : integer;
begin
  read(x);
  read(y);
  z := x + y * 2;
  write(z)
end.
```

## 🔍 Instructions de la machine à pile

Le compilateur génère du code pour une machine à pile virtuelle avec les instructions suivantes :

- `PUSH n` : Empiler une constante
- `LOAD var` : Charger une variable
- `STORE var` : Stocker dans une variable
- `ADD`, `SUB`, `MUL`, `DIV`, `MOD` : Opérations arithmétiques
- `EQ`, `NE`, `LT`, `LE`, `GT`, `GE` : Comparaisons
- `AND`, `OR` : Opérations logiques
- `JMP label`, `JZ label` : Sauts conditionnels et inconditionnels
- `READ var`, `READLN` : Lecture
- `WRITE var`, `WRITELN` : Écriture

## 🎓 Contexte pédagogique

Ce projet est conçu à des fins éducatives pour : 
- Comprendre le fonctionnement interne d'un compilateur
- Apprendre l'analyse lexicale et syntaxique
- Découvrir la génération de code intermédiaire
- Manipuler les tables de symboles

## 👥 Auteurs

- Abidi Emna
- Ben Rahma Ahmed

## 📄 Licence

Ce projet est libre d'utilisation à des fins éducatives.

## 🐛 Gestion des erreurs

Le compilateur détecte et signale : 
- Erreurs lexicales (caractères invalides)
- Erreurs syntaxiques (structure incorrecte)
- Erreurs sémantiques (variables non déclarées)
- Commentaires non terminés

## 🔧 Améliorations possibles

- Support de plus de types de données
- Optimisation du code généré
- Gestion des fonctions et procédures
- Tableaux et structures
- Meilleure gestion des erreurs avec numéros de ligne
