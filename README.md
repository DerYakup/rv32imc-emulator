# rv32imc-emulator
digitale systeme Projekt 2026
# Erklärungen zu Denkweisen und Design Entscheidungen


## C-Erweiterung

### SW
Die ersten 5 Bits von uimm sind durch die Maske 0x60 null. Beim Verschieben um 20 Stellen landen diese Nullen im Bereich von rs2, sodass sie dessen Wert beim | nicht verändern. Nur die Bits uimm[6:5] werden in die Bits [26:25] gesetzt.