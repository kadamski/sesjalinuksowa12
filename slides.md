% Debugowanie jądra Linux
% Krzysztof Adamski
% 17.04.2015

## Proces debugowania

- Odkrycie błędu.
- **Wyizolowanie oraz identyfikacja przyczyny błędu.**
- Usunięcie defektu.

---

## Techniki debugowania

- printf debugging
- post-mortem debugging
- interactive debugging

# printf debugging

## Najprostsze użycie tej techniki

1. Szukamy okolic kodu gdzie jest problem.
2. Dodajemy kilka wywołań printk().
3. Rekompilacja, reboot.
4. Obserwujemy log (dmesg).
5. Poznaliśmy pewne szczegóły odnośnie ścieżki kodu prowadzącej do błędu,
potrzebujemy trochę więcej informacji.
6. Powrót do kroku 2.

## printk

*Documentation/printk-formats.txt*

- wyjście zapisywane w buforze cyklicznym (16KB)
- kilka poziomów logowania (`/proc/sys/kernel/printk`)
- do odczytania przez `dmesg`, `/proc/kmsg`, `klogd` oraz na konsoli
- `printk_ratelimited`, `printk_once`
- przydatny trik: `echo msg > /dev/kmsg`


## printk, problemy

- szum informacyjny
- duży koszt printk może ukryć błąd
- długie cykle wymagające rekompilacji i reboot

## dynamic debug

*Documentation/dynamic-debug-howto.txt*

- wyłączone wyrażenia mają znikomy overhead
- dużo jest już dodanych do kernela
- brak potrzeby kompilacji i rebootu
- `<debugfs>/dynamic_debug/control`
- `echo "file net/ipv4/ping.c line 696 +p"`

## ftrace-printk

- zapisuje tylko do bufora cyklicznego, pomijając konsolę
- bezpieczny w kontekście przerwania
- `<debugfs>/tracing/trace`
- Łatwa synchronizacja z userspace dzięki `trace_marker` oraz `tracing_on`/`tracing_off`
- integracja z ftrace

## kprobetrace

*Linux/Documentation/trace/kprobetrace.txt*

- dynamicznie włączane eventy przy wejściu/wyjściu z funkcji
- możliwość wypisania argumentów funkcji oraz kodu powrotu
- używa bufora ftrace
- `<debugfs>/tracing/kprobe_events`

# Post-mortem debugging

## Definicja

Analiza przyczyny powstania błędu po tym jak program, który uległ awarii
przestał już działać. Najczęściej analizowany jest zrzut pamięci (core dump)
oraz stacktrace (backtrace).

---

## Kdump

*Documentation/kdump/kdump.txt*

* Część pamięci jest alokowana na tzw. dump kernel
* Za pomcą kexec ładowany jest dump kernel
* W przypadku wyjątku bootowany jest (kexec) dump kernel
* Obraz pamięci (`crash dump`) dostępny w postaci ELF w `/proc/vmcore`
* Analiza pliku `crash dump` za pomocą `crash util` ([http://people.redhat.com/anderson/](http://people.redhat.com/anderson/))

---

## Kdump + Crash DEMO

# Przykład z życia wzięty

## analiza

<pre><code>crash&gt; dmesg |grep -A29 Oops | sed -e '1,10p' -e '$p' -e 'd' | cut -d ' ' -f 2-
Internal error: Oops - undefined instruction: 0 [#1] PREEMPT SMP ARM
Modules linked in: wlan(O) texfat(PO) mpq_dmx_hw_plugin mpq_adapter dvb_core tspp mhl_sii8620_8061_drv(O) [last unloaded: wlan]
CPU: 2    Tainted: P           O  (3.4.0-perf-g44071e7 #1)
PC is at kgsl_ioctl+0x204/0x344
LR is at 0xffffffff
pc : <b style='color: yellow'>[&lt;c0444080&gt;]</b>    lr : [&lt;ffffffff&gt;]    psr: 00070013
sp : e3e7bea0  ip : eabdf8c0  fp : c044748c
r10: 81a9a874  r9 : c0f70980  r8 : de43b100
r7 : e3e7beac  r6 : 400c0907  r5 : ffffffff  r4 : eabdf8c0
r3 : 00000000  r2 : 00000000  r1 : e3e7a000  r0 : c0f7096c
Code: e1530005 01520004 0a00000b e2800fe5 (ed8d1004)
</code></pre>

Wyjątek 'undefined instruction' wystąpił kiedy `PC=0xc0444080`.

## analiza c.d.

Sprawdźmy co znajduje się pod adresem wskazywanym przez PC:

```
crash> rd c0444080
c0444080:  e58d1004                              ....
crash> dis c0444080
0xc0444080 <kgsl_ioctl+516>:    str     r1, [sp, #4]
```

Instrukcja wygląda w porządku jednak procesor nie mógł jej zdekodować. Czemu?

## analiza c.d.

Sprawdźmy jeszcze raz jaki kod wykonywał procesor:

<pre><code>crash&gt; dmesg |grep -A29 Oops | tail -1 | cut -d ' ' -f 2-
Code: e1530005 01520004 0a00000b e2800fe5 (<b style="color: yellow">ed8d1004</b>)
</code></pre>

I jaki jest pod PC:

```
crash> rd c0444080
c0444080:  e58d1004                              ....
```

## analiza c.d.

Szybkie porównanie obu wartości:

<pre><code>crash&gt; eval ed8d1004 | grep bin
     binary: 1110<b style="color: yellow">1</b>101100011010001000000000100
crash&gt; eval e58d1004 | grep bin
     binary: 1110<b style="color: yellow">0</b>101100011010001000000000100
</pre></code>

. . .

Jeden bit się różni !

## analiza c.d.

Jako jaką instrukcja zdekoduje się ta wartość?

<pre><code>crash> search ed8d1004
e3e7bd5c: ed8d1004
crash> dis e3e7bd5c
dis: WARNING: e3e7bd5c: no associated kernel symbol found
   0xe3e7bd5c:  <b style="color: yellow">stc     0, cr1, [sp, #16]</b>
</code></pre>

`STC` to instrukcja zapisu do koprocesora, w tym przypadku nieistniejącego CR1.

. . .

## Konkluzja:

Bitflip w cache CPU!

# interactive debugging

## KDB / KGDB

# Pytania?

## Dziękuję za uwagę

Kontakt:

[k@japko.eu](mailto:k@japko.eu)

CV?

[krzysztof.adamski@tieto.com](mailto:krzysztof.adamski@tieto.com)
