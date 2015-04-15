% Debugowanie jądra Linuksa
% Krzysztof Adamski
% 17.04.2015

---

Proces debugowania:

- Odkrycie błędu.
- Wyizolowanie oraz identyfikacja przyczyny błędu.
- Usunięcie defektu.

---

Techniki debugowania:

- printf debugging
- interactive debugging
- post-mortem debugging

---

# Post-mortem debugging

Analiza przyczyny powstania błędu po tym jak program, który uległ awarii
przestał już działać. Najczęściej analizowany jest zrzut pamięci (core dump)
oraz stacktrace (backtrace).

Documentation/kdump/kdump.txt

---

Przykład analizy Kdump:

```
crash> dmesg |grep -A29 Oops | sed -e '1,10p' -e '$p' -e 'd' | cut -d ' ' -f 2-
Internal error: Oops - undefined instruction: 0 [#1] PREEMPT SMP ARM
Modules linked in: wlan(O) texfat(PO) mpq_dmx_hw_plugin mpq_adapter dvb_core tspp mhl_sii8620_8061_drv(O) [last unloaded: wlan]
CPU: 2    Tainted: P           O  (3.4.0-perf-g44071e7 #1)
PC is at kgsl_ioctl+0x204/0x344
LR is at 0xffffffff
pc : [<c0444080>]    lr : [<ffffffff>]    psr: 00070013
sp : e3e7bea0  ip : eabdf8c0  fp : c044748c
r10: 81a9a874  r9 : c0f70980  r8 : de43b100
r7 : e3e7beac  r6 : 400c0907  r5 : ffffffff  r4 : eabdf8c0
r3 : 00000000  r2 : 00000000  r1 : e3e7a000  r0 : c0f7096c
Code: e1530005 01520004 0a00000b e2800fe5 (ed8d1004)
```

Wyjątek 'undefined instruction' wystąpił kiedy `PC=0xc0444080`.

---

Sprawdźmy co znajduje się pod adresem wskazywanym przez PC:

```
crash> rd c0444080
c0444080:  e58d1004                              ....
crash> dis c0444080
0xc0444080 <kgsl_ioctl+516>:    str     r1, [sp, #4]
```

Instrukcja wygląda w porządku jednak procesor nie mógł jej zdekodować. Czemu?

---

Sprawdźmy jeszcze raz jaki kod wykonywał procesor:

```
crash> dmesg |grep -A29 Oops | tail -1 | cut -d ' ' -f 2-
Code: e1530005 01520004 0a00000b e2800fe5 (ed8d1004)
```

I jaki jest pod PC:

```
crash> rd c0444080
c0444080:  e58d1004                              ....
```

. . .

Szybkie porównanie obu wartości:

```
crash> eval ed8d1004 | grep bin
     binary: 11101101100011010001000000000100
crash> eval e58d1004 | grep bin
     binary: 11100101100011010001000000000100
```

. . .

Jeden bit się różni !

---

Jako jaka instrukcja zdekoduje się ta wartość?

```
crash> search ed8d1004
e3e7bd5c: ed8d1004
crash> dis e3e7bd5c
dis: WARNING: e3e7bd5c: no associated kernel symbol found
   0xe3e7bd5c:  stc     0, cr1, [sp, #16]
```

`STC` to instrukcja zapisu do koprocesora, w tym przypadku nieistniejącego CR1.

. . .

## Konkluzja:

Bitflip w cache CPU!

---

# Kontakt

[k@japko.eu](mailto:k@japko.eu)

CV?

[krzysztof.adamski@tieto.com](mailto:krzysztof.adamski@tieto.com)
