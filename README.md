# Morse code decoder

My semestral work for BI-ARD at [FIT CTU](https://fit.cvut.cz/en).

## How to use

1. Turn on or restart your Arduino Esplora.
2. Wait until you see "Flash S to Start".
3. To the right of the display is a light sensor. To send a dash(long signal), change light intensity for at least 1 second, to send a (short signal), change light intensity for less than 1 second. To finish sending a letter, don't change light intensity for 2 seconds.
4. To start transmission, flash *S*(...).
5. If you enter a character that doesn't correspond to *S*, the red diode will flash, but you can try to start transmission again. Otherwise, at the top of the screen will appear "Listening".
6. Now you can send your Morse code, and it will be translated and displayed on the screen from left to right and from top to bottom.
7. The transmission will be paused if no signal was sent in 5 second or if the character limit was reached. While paused, received message will be still visible.
8. Now you can select from following options, each one is represented by one character, to select it, send corresponding character. If another character is entered, the red diode will flash and you can select again. 
   - Continue(C): The transmission will be resumed.
   - Finish(F): Transmission will end, the application will get back to initial state.
   - Backspace(B): The last character of the received message will be removed.

## Specifications

### Morse code

Characters that can be decoded and their representation in Morse code are defined in the following table.

|||||
----|----|----|----
A | .- | K | -.- | U | ..- | 4 | ....-
B | -... | L | .-.. | V | ...- | 5 | .....
C | -.-. | M | -- | W | .-- | 6 | -....
D | -.. | N | -. | X | -..- | 7 | --...
E | . | O |  --- | Y | -.-- | 8 | ---..
F | ..-. | P | .--. | Z | --.. | 9 | ----.
G | --. |  Q | --.- | 0 | ----- | + | .-.-.
H | .... | R | .-. | 1 | .---- | = | -...-
I | .. |  S | ... | 2 | ..--- | / | -..-.
J | .--- |  T | - | 3 | ...-- | |

### Display

The screen has only 160x128 pixels, the font size is 10 pixels and additional information on displays further decrease the space we have. In total only 126 (14x9) characters can be displayed, there is no possibility to have a message containing more than 126 symbols.

### Light sensor

Detecting of the light signals made by the user is done by comparing current light intensity with past light intensity. Thanks to this approach both decreasing and increasing light intensity can be used to send messages.
Unfortunately, the light sensor cannot distinguish increase of light intensity when there is already lots of light.
Before and after sending signal (message) must be approximately the same light intensity, otherwise the receiving will never end.

## License

Licensed under the [MIT License](LICENSE).
