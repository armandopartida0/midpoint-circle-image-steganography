Name: Armando Partida Sanabia
Class: CPSC 4040

To encode a file:
./final-project -e <cover image name> <data file>

This operation will output a new image named "encoded-image.png"

To decode a file:
./final-project -d <encoded image>

Working examples:
(Hiding ASCII art file)
./final-project -e images/high-res.jpg hidden-message
./final-project -d encoded-image.png

(Hiding tiny image)
./final-project -e images/high-res.jpg images/tiny.png
./final-project -d encoded-image.png
rename decoded-file to decoded-file.png


Bugs:
-Some larger files like higher res images may not be decoded correctly.
-There is no checking to see if a data file can fit within the image. If vector goes out of range, then
data was too big
-File extensions must be manually added for the decoded file

Tips:
-High res images will have more space for embedding data
-Text files work best as hidden data
-Compressing the encoded-image.png will break the embedded data