# Silakka54 — Ergo-L émulé dans le firmware (QMK)

L'ordinateur reste en **AZERTY français** : c'est le clavier qui traduit Ergo-L en touches AZERTY.
Aucun pilote à installer, aucun droit admin requis. Fonctionne sous Windows, Linux et macOS.

Keymap : [`keyboards/silakka54/keymaps/ergol/keymap.c`](keyboards/silakka54/keymaps/ergol/keymap.c)

## Disposition

```
Base (Ergo-L)
 Esc   1  2  3  4  5        6  7  8  9  0   Suppr
 Tab   q  c  o  p  w        j  m  d  ★  y   ⌫
 Ctrl  a  s  e  n  f        l  r  t  i  u   Entrée
 Maj   z  x  -  v  b        .  h  g  ,  k   Maj
            Win Maj Nav/⌫   Espace Sym Alt

Maj + chiffres : € « » $ % ^ & * # @      Maj + - . , ★ : ? : ; !

★ (touche morte, un appui puis la lettre ; □ = la lettre normale sort)
 â ç œ ô □        □ µ _ ¨ û          Maj : Â Ç Œ Ô / Û, ¡ sur ★
 à é è ê ñ        ( ) î ï ù          Maj : À É È Ê Ñ / Î Ï Ù
 æ ß - – —        … □ □ · □          Maj : Æ, ¿ sur -, • sur ,
 ★ puis Espace : ’

Sym (pouce droit, = couche AltGr d'Ergo-L)
 ^ < > $ %        @ & * ' `
 { ( ) } =        \ + - / "
 ~ [ ] _ #        | ! ; : ?

Nav (maintenir ⌫ au pouce gauche)
      F1 … F5              F6 … F10  F11
       Esc Début ↑ Fin PgPréc    / 7 8 9 *   F12
  Ctrl+A ← ↓ → PgSuiv            - 4 5 6 0   ImprÉcr
  Annul Couper Copier Coller Rétablir   , 1 2 3 .   +
                                 pouce Espace : Suppr

Nav + Sym (les deux pouces, dans n'importe quel ordre) : choix de l'OS, mémorisé dans le clavier
 W → Windows    L → Linux    M → macOS     (touches w, l, m d'Ergo-L)
 Esc → mode flash (BOOT)
```

Les raccourcis suivent la position des lettres Ergo-L (Ctrl+C = touche `c` d'Ergo-L), comme avec le vrai pilote.
Ceux de la couche Nav (annuler, copier…) utilisent Cmd sous macOS.

Avec Ctrl, Alt ou Win tenu, une touche de symbole ou de chiffre envoie la touche AZERTY qui porte ce
caractère, sans Maj ni AltGr : Ctrl+`1` donne Ctrl+1, Ctrl+`/` donne Ctrl+`:` (commenter une ligne
dans VS Code en AZERTY). Revers : sous Windows et Linux, Ctrl+`@` donne Ctrl+0.

## Choisir l'OS

Par défaut le clavier est en mode **Windows**. Pour changer : maintenir Nav (⌫) et Sym (dans n'importe quel ordre), puis taper
`w`, `l` ou `m`. Le choix est gardé même après débranchement.

Disposition attendue côté OS :

| OS | Disposition | Caractères hors AZERTY |
|---|---|---|
| Windows | Français (France) | codes Alt `Alt+0nnn` (Verr Num activé automatiquement) |
| Linux | `fr` (Français, variante par défaut) | AltGr (æ ß « » ’ • · ¡ ¿), sinon `Ctrl+Maj+U` (œ ñ – — …) |
| macOS | **Français** (pas « Français – PC ») | touches Option |

Les majuscules À É È Ç Ù passent par Verr Maj sous Linux et macOS (Verr Maj + é = É).

## Répétition

Toutes les touches se répètent quand on les maintient. Les caractères d'une seule touche (chiffres,
ponctuation, symboles) sont répétés par l'OS ; les séquences (ê, œ sous Windows…) sont rejouées par
le clavier (après 500 ms, puis toutes les 80 ms).

## Limites de l'émulation

- **Windows** : les codes Alt marchent dans la grande majorité des applis, mais pas forcément dans
  les terminaux, en bureau à distance (RDP/Citrix) ou dans une VM.
- **Linux** : `Ctrl+Maj+U` est géré par GTK et IBus (GNOME, Ubuntu…). Sous KDE sans IBus/Fcitx,
  œ ñ – — … ne sortent pas.
- **macOS** : si `<` `>` et `@` `#` sont inversés, macOS a pris le clavier pour un ANSI. Le corriger
  dans Réglages › Clavier › Type de clavier → **ISO (Europe)**. La puce `•` reste à vérifier.
- **Verr Maj** (Linux, macOS) : À É È Ù Ç passent par Verr Maj. Si cette touche est réaffectée
  (options xkb `caps:escape` ou `ctrl:nocaps`, touches de modification macOS, « Verr Maj change de
  source de saisie »), ces majuscules ne sortent pas (Échap puis é, par exemple).
- **★ incomplet** : la rangée des chiffres de ★ („ “ ” ‘ ’ ¢ ‰ § ¶ °), la touche morte grecque (★ g),
  le trait d'union insécable (★ - donne un simple -) et les espaces insécables (Maj+Espace) d'Ergo-L
  ne sont pas émulés.
- Les dispositions « Français – PC » (macOS), « oss » / « latin9 » (Linux) ou « AZERTY NF » ne sont
  pas visées : certains symboles seront faux.
- Pas de Vial : toute modification passe par le keymap C et un nouveau flash.

## Compiler

Compilation locale avec Docker (aucune installation QMK nécessaire) :

```sh
docker run --rm -v "$PWD:/userspace" ghcr.io/qmk/qmk_cli bash -c '
  git clone -q --depth 1 --recurse-submodules --shallow-submodules https://github.com/qmk/qmk_firmware /qmk_firmware &&
  qmk config user.qmk_home=/qmk_firmware user.overlay_dir=/userspace &&
  cd /qmk_firmware && qmk compile -kb silakka54 -km ergol'
```

Sinon, une fois le dépôt poussé sur GitHub, le workflow `.github/workflows/build_binaries.yaml`
compile le firmware et le publie en release (`silakka54_ergol.uf2`).

## Flasher (depuis le PC perso)

À faire **sur les deux moitiés**, avec le même fichier `.uf2` :

1. Débrancher le clavier et le câble entre les deux moitiés.
2. Maintenir le bouton **BOOT** du RP2040 et brancher la moitié en USB : un lecteur `RPI-RP2` apparaît.
3. Copier `silakka54_ergol.uf2` dessus. Le lecteur disparaît et le clavier redémarre.
4. Recommencer avec l'autre moitié.

Ensuite, brancher l'USB sur la **moitié gauche**. Pour les flashs suivants, `Nav + Sym + Esc`
passe directement en mode bootloader, mais seulement la moitié branchée en USB (la gauche) : la
droite demande toujours le bouton BOOT. Comme tout le keymap est traité par la moitié gauche,
la reflasher seule suffit en général ; en cas de comportement étrange, reflasher les deux.
