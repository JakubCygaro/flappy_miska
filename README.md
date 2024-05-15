Fanowska gra o moim ukochanym polskim streamerze Dawid Kamil Patryk Kurczak Minecraft Jasper Wójcik ok ok ok????? i jego psie Miśce.

![image](https://github.com/JakubCygaro/flappy_miska/assets/57292664/aa2834f6-3cce-4754-a326-cdc11d10957f)

![image](https://github.com/JakubCygaro/flappy_miska/assets/57292664/dad5c320-c2ff-4dd9-b770-b0bce7cb88e5)

# Jak w to zagrać?

Kochani widzowie musicie jedynie pobrać najnowsze wydanie gry po prawej stronie, wypakować gdzieś tego zipa do jakiegoś folderu dwa razy kliknąć myszką na plik `flappy_miska.exe` no i gra powinna działać. Jeśli nie działa to szlag mnie trafi, proszę wtedy zgłaszać problemy na git albo nie wiem pisać do lokalnego kapłana/wójta/policjanta.

# Jak grać?

Klikasz "Graj" i nawalasz spację.

# Chciałbym to sam skompilować ok ok to pilne???

No to musisz ogarnąć sobie:
- SDL2
- SDL_ttf
- SDL_mixer
- SDL_img

no i zbuildować wszystko co jest w `src/` linkując z tymi bibliotekami. Pamiętaj też o flagach `-Wl,-subsystem,windows`. Ja używam MINGW nie wiem jak to będzie wyglądało na kompilatorach innych niż gcc.

Nie dałem żadnego build skryptu dlatego, że używałem czystego make w bardzo nieprzenośnej konfiguracji, która działa tylko u mnie na komputerku.

# Czy w boljerze jest bomba?

JEZU W BOLJERZE NIE MA BOMBY
