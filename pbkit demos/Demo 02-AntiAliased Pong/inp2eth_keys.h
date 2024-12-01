
#ifndef __INP2ETH_KEYS_H
#define __INP2ETH_KEYS_H

//Constants you can use as index of keymap[] array to test if a key or button is pressed

//"To turn wiimote on itself" means you turn it like it was a key to unlock a door

//GlovePie script inp2eth.PIE will translate wiimote1 input into these keys (scancode):
#define WIIMOTE1_ONE	112
#define WIIMOTE1_TWO	113
#define WIIMOTE1_A	114
#define WIIMOTE1_B	115
#define WIIMOTE1_PLUS	116
#define WIIMOTE1_MINUS	117
#define WIIMOTE1_HOME	118
#define WIIMOTE1_UP	119
#define WIIMOTE1_DOWN	120
#define WIIMOTE1_LEFT	122
#define WIIMOTE1_RIGHT	123

//Pointing wiimote1 upward will push msey into negative values
//Pointing wiimote1 downward will push msey into positive values
//Turning wiimote1 (on itself) leftward will push msex into negative values
//Turning wiimote1 (on itself) rightward will push msex into positive values

//GlovePie script inp2eth_2wiimotes.PIE will translate wiimote2 input into these keys:
#define WIIMOTE2_ONE	'E'
#define WIIMOTE2_TWO	'R'
#define WIIMOTE2_A	'T'
#define WIIMOTE2_B	'Y'
#define WIIMOTE2_PLUS	'U'
#define WIIMOTE2_MINUS	'I'
#define WIIMOTE2_HOME	'O'
#define WIIMOTE2_UP	'F'
#define WIIMOTE2_DOWN	'G'
#define WIIMOTE2_LEFT	'H'
#define WIIMOTE2_RIGHT	'J'

//Pointing wiimote1 upward will push msey into negative values
//Pointing wiimote1 downward will push msey into positive values
//Pointing wiimote2 upward will push msex into negative values
//Pointing wiimote2 downward will push msex into positive values

//Turning wiimote1 or wiimote2 (on itself) will trigger these keys:
#define WIIMOTE1_TURNLEFT	'<'
#define WIIMOTE1_TURNRIGHT	'>'
#define WIIMOTE2_TURNLEFT	'^'
#define WIIMOTE2_TURNRIGHT	'v'

#endif  /* __INP2ETH_KEYS_H */

