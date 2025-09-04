
### TODO

Du har et setpoint, som vil bli endret av calibrering/turning.
For eksempel calibrate => setpoint += 10, og turn90 => setpoint += 1024

Da er dette eneste eksterne funksjonene du trenger (og update, og constructor, fuck deg)
Så internt kan du ha at delay blir påvirket av forrige delay, og at hvis det er
mer enn x antall steg igjen skal du akselerere, og om det er færre enn y skal du
sakke ned.