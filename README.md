<div align="center">
  <h1>Greenhouse-project-OK</h1>
  <p>Projekt zaliczeniowy na przedmiot Optymalizacja Kombinatoryczna</p>
</div> 

# Opis problemu
W szklarni znajduje się zadana liczba rzędów (w każdym rzędzie określona liczba grządek). Rośliny wymagają podlewania, woda jest w obiegu (prąd wody idzie cały czas w jedną stronę). Program ma za zadanie wyznaczyć kolejność podlewania 2 roślin jednocześnie dla jednego cyklu podlewania, przy założeniu, że jedną roślinę wybieramy przez najkrótszy czas podlewania, a drugą przez najdłuższy czas. Odległość podlewanych w danym momencie roślin jest równa kosztowi utrzymania otwartych zaworów. 

Aby wyznaczyć czas podlewania należy skorzystać ze wzoru:

<div align="center">
  <h2>$T = \frac{W \cdot S}{Q}$</h2>
</div> 

gdzie:
  * $T$ - czas podlewania (w sekundach)
  * $W$ - zapotrzebowanie na wodę (w litrach na $m^2$)
  * $S$ - wielkość pojedynczej grządki (w $m^2$)
  * $Q$ - przepływ wody (w litrach na sekundę)

<div align="center">
  <h2>$Q = \frac{k \cdot P}{60}$</h2>
</div> 

gdzie:
  * $k$ - współczynnik zależny od średnicy węża
  * $P$ - ciśnienie w barach

Wzór na współczynnik $k$ można zapisać w uproszczonej wersji:

<div align="center">
  <h2>$k = d - 4$</h2>
</div> 

gdzie:
  * $d$ - średnica węża w milimetrach (minimum 5mm)

Przyjęte w programie stałe:
  * $S=0.25m^2$
  * $P=6B$
  * $d=5mm$
