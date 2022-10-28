# vgmplay
Erstes Progrämmchen für den NKC (Abspielen einer VGM-Datei auf SOUND3)

Das ist ein erster Programmierversuch in C für den NDR-Klein-Computer (NKC).

Aufruf: VGMPLAY Dateiname.VGM

Spielt den Sound der VGM-Datei ab, wenn er denn für den Sound-Chip des NKC codiert ist (AY-3-8910).

Das Main-Theme von Moonpatrol findet sich in 02.VGM. Vor der Sound-Ausgabe erfolgt eine kleine Status-Anzeige zur Datei, hinterher ein Hexdump des Beginns der Nutzdaten und am Ende folgt ein Nikolaus-Häuschen... :)


Die Test-VGMs stammen von hier: https://vgmrips.net/packs/pack/moon-patrol-irem-m52 (dort finden sich VGZs, die mit gzip entpackt werden können)

Die SOUND3 muss auf Port $50 eingestellt sein.


Das Ganze ist wirklich nur ein erster Versuch, der aber immerhin eine erkennbare Melodie ausgibt.

Ich vermute, dass die Initialisierung des Sound-Chips noch einer Optimierung bedarf. Auch die Delay-Routine ist noch nicht die, die mir vorschwebt. Allerdings funktionierten die Lösungen aus anderen Beispielen nicht wirklich. Muss ich noch mal gucken.

Vielen Dank an das NKC-Forum (https://hschuetz.selfhost.eu/forumdrc) für Support und Beispiele!