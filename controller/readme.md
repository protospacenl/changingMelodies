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

## interactive mode
Vanuit deze modes kunnen posities worden uitgelezen en opgeslagen in een template playlist

```bash
python3 ./controller.py -c config.json -p playlist.json -i
```

### command menu:
```
 ?: help, print command list
 h: hold, hold all joints using the holding torque defined in config.json
 r: release, release all joints
 p: print positions and store for saving
 n: new file to save positions to
 s: save positions received with the `p` command to the file created with the `n` command
 w: pulse the water pump for one second
 q: quit
```

Met `s` worden de posities bewaard die met `p` opgehaald zijn. Je kunt opgeven welke joints opgeslagen worden door de joint ids in te typen met een spatie ertussen, `0` is alle joints. Het programma onthoud de laatste opgegeven joints. Als die onveranderd is is `enter` voldoende.

## stop de player
`CTRL-C`

# Config
Let op: alle is __case sensitive__!!

Er zijn twee nieuwe opties bijgekomen in de configuratie file `config.json`
## holding_torque
Torque om de joints in een statische positie te houden. 

Kies een waarde hoog genoeg die de arm en hoofd in positie kan houden, maar laag genoeg zodat die nog handmatige verplaatst kan worden. Schouders en nek rotatie kunnen heel laag gezet worden (10 of 20 bijv.)

## PGain
Dit is de P gain van de PID regeling en geld alleen voor de arm joints, niet voor de nek.

Hoe lager deze gezet wordt hoe minder de joint zijn best doet om de doel positie te bereiken. Dit zorgt voor een minder agressieve beweging en minder terugduwen. Goede waarden tussen de 32 en 1. 0 is geen beweging.

# Playlist
Let op: alle is __case sensitive__!!

## Posities
Commentaar is toe te voegen door een onbekende tag te gebruiken bijv: `"#": "Dit is commentaar"`


* `__naam__` is de naam van de positie
* `__positie__` is de numerice positie:
  * positie neck_ud, neck_rot van `0-1023`
  * positie shoulder_ud, shoulder_rot, wrist van `0-4095`
* `__torque__` is de torque waarde tussen 0 en 1023


```json
    "positions": {
        "arm": {
            __naam__: { "joints": [ { "name": "wrist", "pos": __positie__, "torque": __torque__ } ], "#": "Positie met 1 gewricht" },
            __naam__: { "joints": [ { "name": "shoulder_rot", "pos": __positie__, "torque": __torque__ },
                                      { "name": "shoulder_ud",  "pos": __positie__, "torque": __torque__ },
                                      { "name": "wrist",        "pos": __positie__, "torque": __torque__ } ], "#": "Positie met 3 gewrichten" }
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
        { "target": "tool", "cmd": "home" }
        
        { "target": "touch", "cmd": "_wait_for_trigger", "timeout": 10, "on_timeout": "restart", "#": "wacht op trigger en begin by begin playlist bij een timeout en ga verder bij trigger"},
        { "target": "touch", "cmd": "_wait_for_trigger", "timeout": 10, "on_trigger": "restart", "#": "wacht op trigger en begin by begin playlist bij een trigger en ga verder bij een timeout"},
        { "target": "touch", "cmd": "_wait_for_trigger", "#": "wacht oneinding op trigger en ga verder zodra getriggered"},

        { "target": "head", "cmd": "send", "data": "a", "#": "Stuur data naar het hoofd"},

        { "target": "arm",  "cmd": "move",   "position": "wrist_up", "speed": 300, "#": "Beweeg de arm naar positie wrist_up met een snelheid van 300 (snelheid loopt van 0-1023, 0 is max. Waarden tussen 100 en 300 zijn vaak prima" },

        { "target": "_",    "cmd": "delay",  "seconds": 5, "#": "Wacht voor seconds seconden" },

        { "target": "tool", "cmd": "move",   "position": "center", "speed": 1, "#": "beweeg de tool arm naar positie center, snelheid is op dit moment genegeerd" },

        { "target": "arm",  "cmd": "relax",  "joint": "all", "#": "relax de gehele arm" },

        { "target": "arm",  "cmd": "relax",  "joint": "wrist", "#": "relax de pols" },

        { "target": "arm",  "cmd": "hold",  "joint": "all", "#": "hold alle joints met holding torque uit config.json" },

        { "target": "arm",  "cmd": "hold",  "joint": "wrist", "#": "hold de pols met holding torque uit config.json" },
        
        { "target": "head", "cmd": "say",    "path": "/path/to/file.wave", "#": "Speel wav file" },
        
        { "target": "pump", "cmd": "on", "#": "turn on pump" }
        
        { "target": "pump", "cmd": "off", "#": "turn off pump" }
        
        { "target": "pump", "cmd": "pulse", "on_time": 1, "off_time": 1, "n": 5, "#": "Pulse pump for n times" }
    ]
}
```
