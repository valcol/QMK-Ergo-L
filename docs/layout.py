#!/usr/bin/env python3
# Génère les schémas de la disposition (docs/*.svg), clair et sombre.
# À relancer après une modification du keymap : python docs/layout.py

from pathlib import Path
from xml.sax.saxutils import escape

OUT = Path(__file__).parent

PITCH = 64  # écart entre deux touches
KEY = 58  # côté d'une touche
DEPTH = 3  # épaisseur de la touche
MARGIN = 12
FONT = "-apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, sans-serif"

THEMES = {
    "": {
        "key": "#ffffff", "edge": "#d1d9e0", "depth": "#d1d9e0", "mod": "#f6f8fa",
        "text": "#1f2328", "muted": "#59636e",
        "odk": "#bc4c00", "odk_bg": "#fff1e5",
        "sym": "#0969da", "sym_bg": "#ddf4ff",
        "nav": "#1a7f37", "nav_bg": "#dafbe1",
    },
    "-dark": {
        "key": "#212830", "edge": "#3d444d", "depth": "#010409", "mod": "#151b23",
        "text": "#f0f6fc", "muted": "#9198a1",
        "odk": "#ffa657", "odk_bg": "#3d2a17",
        "sym": "#4493f8", "sym_bg": "#132339",
        "nav": "#3fb950", "nav_bg": "#12261e",
    },
}

# Colonnes des touches : 6 par moitié, une colonne vide au milieu ; pouces sous les colonnes 3-5 et 7-9
COLS = [0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12]
THUMB_COLS = [3, 4, 5, 7, 8, 9]


# Types de touches
def letter(upper, odk=None, odk_shift=None):
    return {"tl": upper, "tl_big": True, "br": odk, "tr": odk_shift}


def char(base, shift=None, odk=None, odk_shift=None, style=None):
    return {"bl": base, "tl": shift, "br": odk, "tr": odk_shift, "style": style}


def mod(label, sub=None, style=None, odk=None):
    return {"center": label, "sub": sub, "style": style, "br": odk, "small": True}


def big(label):
    return {"center": label}


def held(label, style):
    return {"center": label, "sub": "maintenu", "style": style, "small": True, "fill": True}


def faded(label=""):
    return {"center": label, "small": True, "faded": True}


BLANK = faded()

BASE = [
    [mod("Échap"), char("1", "€"), char("2", "«"), char("3", "»"), char("4", "$"), char("5", "%"),
     char("6", "^"), char("7", "&"), char("8", "*"), char("9", "#"), char("0", "@"), mod("Suppr")],
    [mod("Tab"), letter("Q", "â"), letter("C", "ç"), letter("O", "œ"), letter("P", "ô"), letter("W"),
     letter("J"), letter("M", "µ"), letter("D", "_"), char("★", "!", "¨", "¡", style="odk"), letter("Y", "û"), mod("⌫")],
    [mod("Ctrl"), letter("A", "à"), letter("S", "é"), letter("E", "è"), letter("N", "ê"), letter("F", "ñ"),
     letter("L", "("), letter("R", ")"), letter("T", "î"), letter("I", "ï"), letter("U", "ù"), mod("Entrée")],
    [mod("⇧ Maj"), letter("Z", "æ"), letter("X", "ß"), char("-", "?", "-", "¿"), letter("V", "–"), letter("B", "—"),
     char(".", ":", "…"), letter("H"), letter("G"), char(",", ";", "·", "•"), letter("K"), mod("⇧ Maj")],
    [mod("Win"), mod("⇧ Maj"), mod("⌫", "Nav", "nav"),
     mod("Espace", odk="’"), mod("Sym", style="sym"), mod("Alt")],
]

SYM = [
    [faded("Échap")] + [BLANK] * 10 + [faded("Suppr")],
    [faded("Tab")] + [big(c) for c in "^<>$%@&*'`"] + [faded("⌫")],
    [faded("Ctrl")] + [big(c) for c in '{()}=\\+-/"'] + [faded("Entrée")],
    [faded("⇧ Maj")] + [big(c) for c in "~[]_#|!;:?"] + [faded("⇧ Maj")],
    [faded("Win"), faded("⇧ Maj"), faded("⌫\nNav"), faded("Espace"), held("Sym", "sym"), faded("Alt")],
]

NAV = [
    [faded("Échap")] + [big(f"F{n}") for n in range(1, 12)],
    [faded("Tab"), big("Échap"), big("Début"), big("↑"), big("Fin"), big("Pg\npréc"),
     big("/"), big("7"), big("8"), big("9"), big("*"), big("F12")],
    [faded("Ctrl"), big("Tout\nsélect."), big("←"), big("↓"), big("→"), big("Pg\nsuiv"),
     big("-"), big("4"), big("5"), big("6"), big("0"), big("Impr\nécran")],
    [faded("⇧ Maj"), big("Annuler"), big("Couper"), big("Copier"), big("Coller"), big("Rétablir"),
     big(","), big("1"), big("2"), big("3"), big("."), big("+")],
    [faded("Win"), faded("⇧ Maj"), held("Nav", "nav"), big("Suppr"), faded("Sym"), faded("Alt")],
]


def text(x, y, s, size, color, anchor="start", weight=400):
    return (f'<text x="{x:g}" y="{y:g}" font-size="{size}" fill="{color}" text-anchor="{anchor}" '
            f'font-weight="{weight}">{escape(s)}</text>')


def centered(cx, cy, label, size, color, weight=400):
    lines = label.split("\n")
    step = size * 1.15
    top = cy - step * (len(lines) - 1) / 2 + size * 0.35
    return "".join(text(cx, top + i * step, line, size, color, "middle", weight) for i, line in enumerate(lines))


def center_size(label, small):
    longest = max(len(line) for line in label.split("\n"))
    if longest <= 2:
        return 17 if small else 21
    if longest <= 4 and not small:
        return 15
    return 12.5 if longest <= 6 else 11.5


def key(col, row, spec, t):
    x, y = MARGIN + col * PITCH, MARGIN + row * PITCH
    style = spec.get("style")
    accent = t[style] if style else None
    is_mod = spec.get("small") and not spec.get("faded")

    fill = t[f"{style}_bg"] if style == "odk" or spec.get("fill") else t["mod"] if is_mod else t["key"]
    edge = accent if style == "odk" or spec.get("fill") else t["edge"]
    out = [f'<g opacity="0.45">' if spec.get("faded") else "<g>",
           f'<rect x="{x}" y="{y + DEPTH}" width="{KEY}" height="{KEY}" rx="8" fill="{t["depth"]}"/>',
           f'<rect x="{x}" y="{y}" width="{KEY}" height="{KEY}" rx="8" fill="{fill}" stroke="{edge}"/>']

    left, right, top, bottom = x + 8, x + KEY - 8, y + 19, y + KEY - 10
    if spec.get("center"):
        label, sub = spec["center"], spec.get("sub")
        cy = y + KEY / 2 - (6 if sub or spec.get("br") else 0)
        # Touche de couche : nom en couleur, sauf ⌫ dont seul « Nav » est coloré
        strong = accent and (not sub or spec.get("fill"))
        color = accent if strong else t["muted"] if spec.get("small") else t["text"]
        out.append(centered(x + KEY / 2, cy, label, center_size(label, spec.get("small")), color,
                            600 if strong else 400))
        if sub:
            out.append(text(x + KEY / 2, y + KEY - 9, sub, 11, accent or t["muted"], "middle"))
    if spec.get("tl"):
        if spec.get("tl_big"):
            out.append(text(left, y + 24, spec["tl"], 19, t["text"], weight=500))
        else:
            out.append(text(left, top + 1, spec["tl"], 15, t["muted"]))
    if spec.get("bl"):
        out.append(text(left, bottom, spec["bl"], 18, accent or t["text"], weight=600 if accent else 500))
    if spec.get("tr"):
        out.append(text(right, top + 1, spec["tr"], 15, t["odk"], "end"))
    if spec.get("br"):
        out.append(text(right, bottom, spec["br"], 16, t["odk"], "end", 500))
    out.append("</g>")
    return "".join(out)


def legend(t):
    # Touche d'exemple (la virgule) dans le coin vide en bas à gauche
    col, row = 1, 4
    x, y = MARGIN + col * PITCH, MARGIN + row * PITCH
    sample = key(col, row, char(",", ";", "·", "•"), t)
    top, bottom = y + 19, y + KEY - 10
    return sample + "".join([
        text(x - 8, top, "Maj", 11, t["muted"], "end"),
        text(x - 8, bottom, "touche", 11, t["muted"], "end"),
        text(x + KEY + 8, top, "★ puis Maj", 11, t["odk"]),
        text(x + KEY + 8, bottom, "★ puis", 11, t["odk"]),
    ])


def render(rows, t, extra=""):
    width = 2 * MARGIN + 12 * PITCH + KEY
    height = 2 * MARGIN + 4 * PITCH + KEY + DEPTH
    keys = []
    for r, row in enumerate(rows[:4]):
        keys += [key(c, r, spec, t) for c, spec in zip(COLS, row)]
    keys += [key(c, 4, spec, t) for c, spec in zip(THUMB_COLS, rows[4])]
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
            f'viewBox="0 0 {width} {height}" font-family="{FONT}">'
            + "".join(keys) + extra + "</svg>\n")


for suffix, theme in THEMES.items():
    (OUT / f"base{suffix}.svg").write_text(render(BASE, theme, legend(theme)), encoding="utf-8")
    (OUT / f"sym{suffix}.svg").write_text(render(SYM, theme), encoding="utf-8")
    (OUT / f"nav{suffix}.svg").write_text(render(NAV, theme), encoding="utf-8")
