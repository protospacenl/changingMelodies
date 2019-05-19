# Connect met de PI

## via netwerk
IP adres van de PI is 192.168.1.2

Login is standaard rpi login:
login: pi
pw: raspberry

## Of via beeldscherm en toetsenbord!


# Start controller:
In directory `/home/pi/changingMelodies`:

## playlist playback mode
```bash
python3 ./controller.py -c config.json -p playlist.json
```

## monitoring mode
Monitoring mode leest de positie van de arm uit en print deze op het scherm
`schouder_rot, schouder_ud, pols`

```bash
python3 ./controller.py -m
```

## stop de player
`CTRL-C`


# Playlist
Let op: alle is __case sensitive__!!

## Posities
Commentaar is toe te voegen door een onbekende tag te gebruiken bijv: `"#": "Dit is commentaar"`


* `__naam__` is de naam van de positie
* `__positie__` is de numerice positie:
  * positie shoulder_rot en wrist van `0-1023`
  * positie shoulder_ud van `0-4095`


```json
    "positions": {
        "arm": {
            __naam__: { "joints": [ { "name": "wrist", "pos": __positie__ } ], "#": "Positie met 1 gewricht" },
            __naam__: { "joints": [ { "name": "shoulder_rot", "pos": __positie__ },
                                      { "name": "shoulder_ud",  "pos": __positie__ },
                                      { "name": "wrist",        "pos": __positie__ } ], "#": "Positie met 3 gewrichten" }
        },

        "tool": {
            __naam__: { "x": __positie__, "z": __positie__, "#": "Tool positie heeft altijd een x en z in millimeters" }
        }
    },
```

## Playlist
De __position__ velden in de move commando's verwijzen naar de posities gemaakt in de __positions__ entries voor arm en tool.

```json
 "playlist": [
        { "target": "touch", "cmd": "_wait_for_trigger", "timeout": 10, "on_timeout": "restart", "#": "wacht op trigger en begin by begin playlist bij een timeout en ga verder bij trigger"},
        { "target": "touch", "cmd": "_wait_for_trigger", "timeout": 10, "on_trigger": "restart", "#": "wacht op trigger en begin by begin playlist bij een trigger en ga verder bij een timeout"},
        { "target": "touch", "cmd": "_wait_for_trigger", "#": "wacht oneinding op trigger en ga verder zodra getriggered"},

        { "target": "head", "cmd": "send", "data": "a", "#": "Stuur data naar het hoofd"},

        { "target": "arm",  "cmd": "move",   "position": "wrist_up", "speed": 300, "#": "Beweeg de arm naar positie wrist_up met een snelheid van 300 (snelheid loopt van 0-1023, 0 is max. Waarden tussen 100 en 300 zijn vaak prima" },

        { "target": "_",    "cmd": "delay",  "seconds": 5, "#": "Wacht voor seconds seconden" },

        { "target": "tool","cmd": "move",   "position": "center", "speed": 1, "#": "beweeg de tool arm naar positie center, snelheid is op dit moment genegeerd" },

        { "target": "arm",  "cmd": "relax",  "joint": "all", "#": "relax de gehele arm" },

        { "target": "arm",  "cmd": "relax",  "joint": "wrist", "#": "relax de pols" },

        { "target": "head","cmd": "say",    "text": "hallo", "#": "Zeg iets. TODO:nog niet geimplementeerd!" },
    ]
}
```
