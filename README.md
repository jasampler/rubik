# rubik
Console program to play with the Rubik's cube and save any position.

                 ___5_____5_____5__ (5      __1_____1_____1__ (1
               3/  1  /  1  /  1  /|       |     |     |     |
               /_____/_____/_____/ |5     4|  5  |  5  |  5  |\1
             3/  1  /  1  /  1  /|4|       |_____|_____|_____|3\
             /_____/_____/_____/ |/|       |     |     |     | |\1
           3/  1  /  1  /  1  /|4/ |5     4|  5  |  5  |  5  |\|3\
        3) /_____/_____/_____/ |/|4|       |_____|_____|_____|3\ |\1
           |     |     |     |4/ |/|       |     |     |     | |\|3\
          3|  2  |  2  |  2  |/|4/ |5     4|  5  |  5  |  5  |\|3\ |2
           |_____|_____|_____| |/|4/       |_____|_____|_____|3\ |\|
           |     |     |     |4/ |/6    4) \  6  \  6  \  6  \ |\|3|
          3|  2  |  2  |  2  |/|4/         4\_____\_____\_____\|3\ |2
           |_____|_____|_____| |/6           \  6  \  6  \  6  \ |\|
           |     |     |     |4/             4\_____\_____\_____\|3|
          3|  2  |  2  |  2  |/6               \  6  \  6  \  6  \ |2
           |_____|_____|_____|                 4\_____\_____\_____\|
              6     6     6   (6                   2     2     2    (2

        AAAAAAAAAAAAAAAAAAAA

Shows a 3D representation of the Rubik's Cube in ASCII and
allows to turn its faces by default entering the digits 1-6.

    -c,--chars UFLRBD     6 characters to represent the colors
    -i,--initial POSITION the 20 uppercase letters (A-X) printed after
                          each move to recover again the same position
    -s,--silent           prints only the POSITION and not the ASCII

Entering the character shown in the center of a face turns
that face clockwise one-quarter turn, and entering -N or N'
turns the face N anticlockwise one-quarter turn. Applying
the minus or the apostrophe again to the same number will
have no effect. Any other unrecognized symbol is ignored.
