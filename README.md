# 📋 JNIDemo — Anti-Debug — By SAAD

Extension du lab JNI de base avec couche défensive native.
Les fonctions natives sont déverrouillées uniquement après vérification
de l'environnement d'exécution par l'utilisateur.

---

## 📽️ Démonstration


https://github.com/user-attachments/assets/2be11d97-a877-443d-91b6-bace63f01a4a


---

# 🏗️ Architecture
Bouton "Vérifier" → isDebugDetected() → C++
→ isBeingTraced()
→ containsSuspiciousLibraryNames()
→ jboolean → UI active ou bloque les fonctions

---

# 🖼️ Interface — `activity_main.xml`

| Élément       | Rôle                                              |
| ------------- | ------------------------------------------------- |
| `btnCheck`    | Lance la vérification de sécurité native          |
| `tvStatus`    | Affiche l'état (OK en vert / suspect en rouge)    |
| `btnHello`    | Appelle `helloFromJNI()` si environnement sûr     |
| `etFactInput` | Saisie de l'entier pour le factoriel              |
| `btnFact`     | Envoie la valeur à `factorial(n)` si sûr          |

👉 Les boutons et champs sont désactivés par défaut, activés uniquement si `isDebugDetected()` retourne `false`.

---

# 🔧 Code natif — `native-lib.cpp`

## `isBeingTraced()`

```cpp
long resultat = ptrace(PTRACE_TRACEME, 0, 0, 0);
if (resultat == -1) return true;
```

## `containsSuspiciousLibraryNames()`

```cpp
FILE* maps = fopen("/proc/self/maps", "r");
while (fgets(ligne, sizeof(ligne), maps)) {
    if (strstr(ligne, "frida") || strstr(ligne, "xposed") ...) return true;
}
```

| Signature    | Outil associé               |
| ------------ | --------------------------- |
| `frida`      | Framework d'instrumentation |
| `xposed`     | Framework de hook Java      |
| `gdbserver`  | Débogueur GDB               |
| `magisk`     | Outil de root               |

## `isDebugDetected()`

```cpp
if (traced || suspiciousMaps) return JNI_TRUE;
return JNI_FALSE;
```

---

# ☕ Logique — `MainActivity.java`

## Déverrouillage conditionnel

```java
btnCheck.setOnClickListener(v -> {
    boolean suspect = isDebugDetected();
    if (suspect) {
        // UI bloquee
    } else {
        environnementSur = true;
        // UI activee
    }
});
```

## Protection des actions

```java
if (!environnementSur) {
    Toast.makeText(this, "Environnement non verifie", ...).show();
    return;
}
```

👉 Même si les boutons sont activés, un second contrôle logique est présent.

---

# 🔍 Logs — Logcat (tag : `ANTI_DEBUG`)
isBeingTraced : aucun traceur detecte
containsSuspiciousLibraryNames : aucune signature suspecte
isDebugDetected : environnement OK

Ou en contexte suspect :
isBeingTraced : trace active detectee
isDebugDetected : environnement suspect confirme

---

# ⚠️ Limites

| Limite               | Explication                                        |
| -------------------- | -------------------------------------------------- |
| Détection imparfaite | Aucun contrôle simple ne couvre tous les cas       |
| Faux positifs        | Certains environnements de dev peuvent déclencher  |
| Contournement        | Un attaquant expérimenté peut neutraliser ces contrôles |

---

# 📚 Concepts clés

| Concept                   | Explication                                   |
| ------------------------- | --------------------------------------------- |
| `ptrace(PTRACE_TRACEME)`  | Détecte si le processus est déjà tracé        |
| `/proc/self/maps`         | Cartographie mémoire du processus             |
| `JNI_TRUE / JNI_FALSE`    | Booléens retournés vers Java                  |
| `environnementSur`        | Flag Java contrôlant l'accès aux fonctions    |
| Désactivation UI          | `setEnabled(false)` tant que non vérifié      |

---

*Projet réalisé dans le cadre d'un apprentissage Android — SAAD* 🚀
